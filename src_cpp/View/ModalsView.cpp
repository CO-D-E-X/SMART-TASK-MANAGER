#include "View/ModalsView.hpp"
#include "View/Theme.hpp"

#include <imgui.h>
#include <cstring>
#include <cstdio>

namespace ToDo::View {

ModalsView::ModalsView() = default;

void ModalsView::OpenAddTaskModal(const std::string& defaultDateStr) {
    addTitle_[0] = '\0';
    addDesc_[0] = '\0';
    addPriority_ = static_cast<int>(Priority::Medium);
    std::strncpy(addDeadline_, defaultDateStr.c_str(), sizeof(addDeadline_) - 1);
    addDeadline_[sizeof(addDeadline_) - 1] = '\0';
    addErrorMsg_.clear();
    showAddModal_ = true;
}

void ModalsView::OpenEditTaskModal(const Task& task) {
    editId_ = task.id;
    std::strncpy(editTitle_, task.title.c_str(), sizeof(editTitle_) - 1);
    editTitle_[sizeof(editTitle_) - 1] = '\0';

    std::strncpy(editDesc_, task.description.c_str(), sizeof(editDesc_) - 1);
    editDesc_[sizeof(editDesc_) - 1] = '\0';

    editPriority_ = static_cast<int>(task.priority);

    std::strncpy(editDeadline_, task.deadline.c_str(), sizeof(editDeadline_) - 1);
    editDeadline_[sizeof(editDeadline_) - 1] = '\0';

    editErrorMsg_.clear();
    showEditModal_ = true;
}

void ModalsView::OpenDeleteModal(uint64_t id, const std::string& title) {
    deleteId_ = id;
    deleteTitle_ = title;
    showDeleteModal_ = true;
}

void ModalsView::OpenStatsModal() {
    showStatsModal_ = true;
}

void ModalsView::Render(const std::string& todayDateStr,
                        const TaskStatistics& stats,
                        const ModalCallbacks& callbacks) {
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    // ====================================================
    // 1. ADD NEW TASK MODAL
    // ====================================================
    if (showAddModal_) {
        ImGui::OpenPopup("Add New Task");
    }

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(520.0f, 430.0f), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Add New Task", &showAddModal_, ImGuiWindowFlags_NoResize)) {
        ImGui::TextColored(ThemeColors::AccentPrimary, "Create Smart Task");
        ImGui::Separator();
        ImGui::Spacing();

        // Title Input
        ImGui::Text("Task Title *");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint("##AddTitle", "e.g. Implement OpenGL Framebuffer Renderer", addTitle_, sizeof(addTitle_));

        ImGui::Spacing();

        // Description Input
        ImGui::Text("Description (Optional)");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextMultiline("##AddDesc", addDesc_, sizeof(addDesc_), ImVec2(-1.0f, 65.0f));

        ImGui::Spacing();

        // Priority Selection
        ImGui::Text("Priority Level");
        ImGui::RadioButton("Low##AddP", &addPriority_, static_cast<int>(Priority::Low));
        ImGui::SameLine();
        ImGui::RadioButton("Medium##AddP", &addPriority_, static_cast<int>(Priority::Medium));
        ImGui::SameLine();
        ImGui::RadioButton("High##AddP", &addPriority_, static_cast<int>(Priority::High));

        ImGui::Spacing();

        // Deadline Input & Quick Selection Buttons
        ImGui::Text("Deadline (YYYY-MM-DD) *");
        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputText("##AddDeadline", addDeadline_, sizeof(addDeadline_));

        ImGui::SameLine();
        ImGui::TextDisabled("Quick:");
        ImGui::SameLine();
        if (ImGui::SmallButton("Today##AddD")) {
            std::strncpy(addDeadline_, todayDateStr.c_str(), sizeof(addDeadline_) - 1);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("+1d##AddD")) {
            std::string d = AddDaysToDate(todayDateStr, 1);
            std::strncpy(addDeadline_, d.c_str(), sizeof(addDeadline_) - 1);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("+3d##AddD")) {
            std::string d = AddDaysToDate(todayDateStr, 3);
            std::strncpy(addDeadline_, d.c_str(), sizeof(addDeadline_) - 1);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("+1w##AddD")) {
            std::string d = AddDaysToDate(todayDateStr, 7);
            std::strncpy(addDeadline_, d.c_str(), sizeof(addDeadline_) - 1);
        }

        if (!addErrorMsg_.empty()) {
            ImGui::Spacing();
            ImGui::TextColored(ThemeColors::OverdueRed, "%s", addErrorMsg_.c_str());
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Modal Action Buttons
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 210.0f);

        if (ImGui::Button("Cancel", ImVec2(90.0f, 30.0f))) {
            showAddModal_ = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ThemeColors::AccentPrimary);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ThemeColors::AccentHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ThemeColors::AccentActive);
        if (ImGui::Button("Create Task", ImVec2(100.0f, 30.0f))) {
            if (std::strlen(addTitle_) == 0) {
                addErrorMsg_ = "Title cannot be empty.";
            } else if (!IsValidDateString(addDeadline_)) {
                addErrorMsg_ = "Invalid date format. Please use YYYY-MM-DD.";
            } else {
                if (callbacks.onConfirmAdd) {
                    callbacks.onConfirmAdd(addTitle_, addDesc_, static_cast<Priority>(addPriority_), addDeadline_);
                }
                showAddModal_ = false;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::PopStyleColor(3);

        ImGui::EndPopup();
    }

    // ====================================================
    // 2. EDIT TASK MODAL
    // ====================================================
    if (showEditModal_) {
        ImGui::OpenPopup("Edit Task");
    }

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(520.0f, 430.0f), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Edit Task", &showEditModal_, ImGuiWindowFlags_NoResize)) {
        ImGui::TextColored(ThemeColors::AccentPrimary, "Edit Task #%llu", static_cast<unsigned long long>(editId_));
        ImGui::Separator();
        ImGui::Spacing();

        // Title
        ImGui::Text("Task Title *");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("##EditTitle", editTitle_, sizeof(editTitle_));

        ImGui::Spacing();

        // Description
        ImGui::Text("Description (Optional)");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextMultiline("##EditDesc", editDesc_, sizeof(editDesc_), ImVec2(-1.0f, 65.0f));

        ImGui::Spacing();

        // Priority
        ImGui::Text("Priority Level");
        ImGui::RadioButton("Low##EditP", &editPriority_, static_cast<int>(Priority::Low));
        ImGui::SameLine();
        ImGui::RadioButton("Medium##EditP", &editPriority_, static_cast<int>(Priority::Medium));
        ImGui::SameLine();
        ImGui::RadioButton("High##EditP", &editPriority_, static_cast<int>(Priority::High));

        ImGui::Spacing();

        // Deadline
        ImGui::Text("Deadline (YYYY-MM-DD) *");
        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputText("##EditDeadline", editDeadline_, sizeof(editDeadline_));

        ImGui::SameLine();
        ImGui::TextDisabled("Quick:");
        ImGui::SameLine();
        if (ImGui::SmallButton("Today##EditD")) {
            std::strncpy(editDeadline_, todayDateStr.c_str(), sizeof(editDeadline_) - 1);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("+1d##EditD")) {
            std::string d = AddDaysToDate(todayDateStr, 1);
            std::strncpy(editDeadline_, d.c_str(), sizeof(editDeadline_) - 1);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("+3d##EditD")) {
            std::string d = AddDaysToDate(todayDateStr, 3);
            std::strncpy(editDeadline_, d.c_str(), sizeof(editDeadline_) - 1);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("+1w##EditD")) {
            std::string d = AddDaysToDate(todayDateStr, 7);
            std::strncpy(editDeadline_, d.c_str(), sizeof(editDeadline_) - 1);
        }

        if (!editErrorMsg_.empty()) {
            ImGui::Spacing();
            ImGui::TextColored(ThemeColors::OverdueRed, "%s", editErrorMsg_.c_str());
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 210.0f);

        if (ImGui::Button("Cancel", ImVec2(90.0f, 30.0f))) {
            showEditModal_ = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ThemeColors::AccentPrimary);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ThemeColors::AccentHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ThemeColors::AccentActive);
        if (ImGui::Button("Save Changes", ImVec2(100.0f, 30.0f))) {
            if (std::strlen(editTitle_) == 0) {
                editErrorMsg_ = "Title cannot be empty.";
            } else if (!IsValidDateString(editDeadline_)) {
                editErrorMsg_ = "Invalid date format. Please use YYYY-MM-DD.";
            } else {
                if (callbacks.onConfirmEdit) {
                    callbacks.onConfirmEdit(editId_, editTitle_, editDesc_, static_cast<Priority>(editPriority_), editDeadline_);
                }
                showEditModal_ = false;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::PopStyleColor(3);

        ImGui::EndPopup();
    }

    // ====================================================
    // 3. DELETE CONFIRMATION MODAL
    // ====================================================
    if (showDeleteModal_) {
        ImGui::OpenPopup("Confirm Deletion");
    }

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(440.0f, 200.0f), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Confirm Deletion", &showDeleteModal_, ImGuiWindowFlags_NoResize)) {
        ImGui::TextColored(ThemeColors::OverdueRed, "Delete Task Confirmation");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextWrapped("Are you sure you want to delete this task?");
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.40f, 1.0f), "\"%s\"", deleteTitle_.c_str());
        ImGui::Spacing();
        ImGui::TextDisabled("This action cannot be undone.");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 220.0f);

        if (ImGui::Button("Cancel", ImVec2(90.0f, 30.0f))) {
            showDeleteModal_ = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.70f, 0.20f, 0.20f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.25f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.95f, 0.30f, 0.30f, 1.0f));
        if (ImGui::Button("Delete", ImVec2(100.0f, 30.0f))) {
            if (callbacks.onConfirmDelete) {
                callbacks.onConfirmDelete(deleteId_);
            }
            showDeleteModal_ = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(3);

        ImGui::EndPopup();
    }

    // ====================================================
    // 4. PRODUCTIVITY INSIGHTS & STATS MODAL
    // ====================================================
    if (showStatsModal_) {
        ImGui::OpenPopup("Productivity Insights");
    }

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(560.0f, 480.0f), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Productivity Insights", &showStatsModal_, ImGuiWindowFlags_NoResize)) {
        ImGui::TextColored(ThemeColors::AccentPrimary, "PRODUCTIVITY INSIGHTS & ANALYTICS");
        ImGui::Separator();
        ImGui::Spacing();

        // Completion Progress
        ImGui::Text("Overall Completion Progress:");
        char progressOverlay[64];
        std::snprintf(progressOverlay, sizeof(progressOverlay), "%zu / %zu Tasks Completed (%.0f%%)",
                      stats.completedTasks, stats.totalTasks, stats.completionRate * 100.0f);
        ImGui::ProgressBar(stats.completionRate, ImVec2(-1.0f, 24.0f), progressOverlay);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Metrics Table
        if (ImGui::BeginTable("StatsGrid", 2, ImGuiTableFlags_BordersInnerH)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Total Tasks:");
            ImGui::TableSetColumnIndex(1); ImGui::Text("%zu", stats.totalTasks);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Completed Tasks:");
            ImGui::TableSetColumnIndex(1); ImGui::TextColored(ThemeColors::PriorityLow, "%zu", stats.completedTasks);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Pending Tasks:");
            ImGui::TableSetColumnIndex(1); ImGui::TextColored(ImVec4(0.40f, 0.70f, 1.0f, 1.0f), "%zu", stats.pendingTasks);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Overdue Tasks:");
            ImGui::TableSetColumnIndex(1); ImGui::TextColored(ThemeColors::OverdueRed, "%zu", stats.overdueTasks);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("High Priority Pending:");
            ImGui::TableSetColumnIndex(1); ImGui::TextColored(ThemeColors::PriorityHigh, "%zu", stats.highPriorityPending);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Medium Priority Pending:");
            ImGui::TableSetColumnIndex(1); ImGui::TextColored(ThemeColors::PriorityMedium, "%zu", stats.mediumPriorityPending);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Low Priority Pending:");
            ImGui::TableSetColumnIndex(1); ImGui::TextColored(ThemeColors::PriorityLow, "%zu", stats.lowPriorityPending);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Average Urgency Score (Pending):");
            ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", stats.averageUrgency);

            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Urgency Engine Explanation
        ImGui::TextColored(ThemeColors::AccentPrimary, "Dynamic Urgency Formula Overview:");
        ImGui::BulletText("Overdue (d < 0): 100 + (weight * 15) + (days overdue * 2.5)");
        ImGui::BulletText("Due Today (d = 0): 80 + (weight * 12)");
        ImGui::BulletText("Upcoming (d > 0): (weight * 40) / (days + 1) + (weight * 3)");
        ImGui::BulletText("Completed: 0.0 (dimmed & sorted to bottom)");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 110.0f);
        if (ImGui::Button("Close", ImVec2(90.0f, 30.0f))) {
            showStatsModal_ = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

} // namespace ToDo::View
