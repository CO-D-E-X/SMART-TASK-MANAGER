#include "Controller/AppController.hpp"
#include "View/Theme.hpp"

#include <imgui.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace ToDo {

AppController::AppController(std::string dataFilePath)
    : dataFilePath_(std::move(dataFilePath)) {
}

AppController::~AppController() {
    Shutdown();
}

void AppController::Initialize() {
    todayDateStr_ = GetCurrentDateString();

    // Default sorting and filter options
    currentFilters_.sortMode = SortMode::Urgency;
    currentFilters_.sortDirection = SortDirection::Descending;
    currentFilters_.statusFilter = StatusFilter::All;
    currentFilters_.priorityFilter = PriorityFilter::All;

    // Load existing tasks or seed demo data
    LoadTasks();

    // Automatically trigger save on any model modification
    model_.SetChangeListener([this]() {
        SaveTasks();
    });
}

void AppController::Shutdown() {
    SaveTasks();
}

std::string AppController::GetFormattedTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << tm.tm_hour << ":"
        << std::setfill('0') << std::setw(2) << tm.tm_min << ":"
        << std::setfill('0') << std::setw(2) << tm.tm_sec;
    return oss.str();
}

void AppController::SaveTasks() {
    const bool success = Persistence::SaveToFile(dataFilePath_, model_.GetAllTasks(), model_.GetHistoryTasks());
    if (success) {
        lastSaveStatus_ = "Saved (" + GetFormattedTimestamp() + ")";
    } else {
        lastSaveStatus_ = "Save Error!";
    }
}

void AppController::LoadTasks() {
    std::vector<Task> loadedTasks;
    std::vector<Task> loadedHistory;
    if (Persistence::LoadFromFile(dataFilePath_, loadedTasks, loadedHistory)) {
        model_.LoadTasks(std::move(loadedTasks), std::move(loadedHistory));
        lastSaveStatus_ = "Loaded (" + std::to_string(model_.GetAllTasks().size()) + " active, " +
                          std::to_string(model_.GetHistoryTasks().size()) + " history)";
    } else {
        // Start fresh with an empty task list as requested
        SaveTasks();
        lastSaveStatus_ = "Ready";
    }
}

void AppController::RenderUI() {
    // Setup full-screen root container
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    const ImGuiWindowFlags rootFlags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18.0f, 18.0f));
    ImGui::Begin("SmartToDoMainWindow", nullptr, rootFlags);

    // Compute live filtered & sorted view and productivity stats
    const auto filteredTasks = model_.GetFilteredAndSortedTasks(currentFilters_, todayDateStr_);
    const auto stats = model_.GetStatistics(todayDateStr_);
    const auto& historyTasks = model_.GetHistoryTasks();

    // Reserve vertical space for the Status Bar footer (approx. 40px)
    const float footerHeight = 42.0f;
    const float mainAreaHeight = ImGui::GetContentRegionAvail().y - footerHeight;

    ImGui::BeginChild("MainContentArea", ImVec2(0, mainAreaHeight), false);

    // Main Navigation Tab Bar
    char activeTabTitle[64];
    std::snprintf(activeTabTitle, sizeof(activeTabTitle), "Active Tasks (%zu)###TabActive", model_.GetAllTasks().size());
    char historyTabTitle[64];
    std::snprintf(historyTabTitle, sizeof(historyTabTitle), "Task History (%zu)###TabHistory", historyTasks.size());

    if (ImGui::BeginTabBar("MainAppTabBar", ImGuiTabBarFlags_None)) {
        // ==========================================
        // TAB 1: ACTIVE TASKS
        // ==========================================
        if (ImGui::BeginTabItem(activeTabTitle)) {
            ImGui::Spacing();

            // 1. Control Panel
            View::ControlPanelCallbacks cpCallbacks;
            cpCallbacks.onOpenAddTask = [this]() {
                modalsView_.OpenAddTaskModal(todayDateStr_);
            };
            cpCallbacks.onSaveNow = [this]() {
                SaveTasks();
            };
            cpCallbacks.onClearCompleted = [this]() {
                model_.ClearCompletedTasks();
            };
            cpCallbacks.onOpenStats = [this]() {
                modalsView_.OpenStatsModal();
            };
            cpCallbacks.onFilterChanged = [this](const FilterOptions& opts) {
                currentFilters_ = opts;
            };

            controlPanelView_.Render(currentFilters_, todayDateStr_, stats.completedTasks, lastSaveStatus_, cpCallbacks);

            // 2. Active Tasks Table
            const float tableHeight = ImGui::GetContentRegionAvail().y;
            ImGui::BeginChild("ActiveTableContainer", ImVec2(0, tableHeight), false);

            View::TaskTableCallbacks tableCallbacks;
            tableCallbacks.onToggleComplete = [this](uint64_t id) {
                model_.ToggleTaskStatus(id);
            };
            tableCallbacks.onEdit = [this](const Task& task) {
                modalsView_.OpenEditTaskModal(task);
            };
            tableCallbacks.onDelete = [this](uint64_t id, const std::string& title) {
                modalsView_.OpenDeleteModal(id, title);
            };

            taskTableView_.Render(filteredTasks, todayDateStr_, tableCallbacks);

            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        // ==========================================
        // TAB 2: TASK HISTORY (Done & Cleared Tasks)
        // ==========================================
        if (ImGui::BeginTabItem(historyTabTitle)) {
            ImGui::Spacing();

            View::HistoryTableCallbacks histCallbacks;
            histCallbacks.onRestore = [this](uint64_t id) {
                model_.RestoreTaskFromHistory(id);
            };
            histCallbacks.onDelete = [this](uint64_t id) {
                model_.DeleteTaskFromHistory(id);
            };
            histCallbacks.onClearAllHistory = [this]() {
                model_.ClearAllHistory();
            };

            historyTableView_.Render(historyTasks, histCallbacks);

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndChild();

    // 3. Status Bar Footer
    statusBarView_.Render(stats, lastSaveStatus_);

    // 4. Modals (Add, Edit, Delete Confirmation, Productivity Stats)
    View::ModalCallbacks modalCallbacks;
    modalCallbacks.onConfirmAdd = [this](const std::string& title, const std::string& desc, Priority priority, const std::string& deadline) {
        model_.AddTask(title, desc, priority, deadline);
    };
    modalCallbacks.onConfirmEdit = [this](uint64_t id, const std::string& title, const std::string& desc, Priority priority, const std::string& deadline) {
        model_.UpdateTask(id, title, desc, priority, deadline);
    };
    modalCallbacks.onConfirmDelete = [this](uint64_t id) {
        model_.DeleteTask(id);
    };

    modalsView_.Render(todayDateStr_, stats, modalCallbacks);

    ImGui::End();
    ImGui::PopStyleVar();
}

} // namespace ToDo
