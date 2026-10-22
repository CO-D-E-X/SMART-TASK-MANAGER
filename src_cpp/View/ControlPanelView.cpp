#include "View/ControlPanelView.hpp"
#include "View/Theme.hpp"

#include <imgui.h>
#include <cstring>
#include <cstdio>

namespace ToDo::View {

ControlPanelView::ControlPanelView() {
    searchBuffer_[0] = '\0';
}

void ControlPanelView::Render(FilterOptions& options,
                             const std::string& todayDateStr,
                             size_t completedCount,
                             const std::string& lastSavedStatus,
                             const ControlPanelCallbacks& callbacks) {
    // ----------------------------------------------------
    // Header Bar: Title, Date, and Primary Actions
    // ----------------------------------------------------
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 10.0f));

    ImGui::BeginGroup();
    ImGui::TextColored(ThemeColors::AccentPrimary, "SMART TO-DO LIST");
    ImGui::SameLine();
    ImGui::TextDisabled("| Dynamic Prioritization (C++20)");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.60f, 0.65f, 0.75f, 1.0f), "|  Today: %s", todayDateStr.c_str());
    ImGui::EndGroup();

    ImGui::SameLine();

    // Right-aligned header action buttons
    const float buttonGroupWidth = 410.0f;
    const float availWidth = ImGui::GetContentRegionAvail().x;
    if (availWidth > buttonGroupWidth) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - buttonGroupWidth));
    }

    // Button: + Add Task
    ImGui::PushStyleColor(ImGuiCol_Button, ThemeColors::AccentPrimary);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ThemeColors::AccentHover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ThemeColors::AccentActive);
    if (ImGui::Button("+ Add Task", ImVec2(92.0f, 28.0f))) {
        if (callbacks.onOpenAddTask) {
            callbacks.onOpenAddTask();
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    // Button: Save Now
    if (ImGui::Button("Save", ImVec2(65.0f, 28.0f))) {
        if (callbacks.onSaveNow) {
            callbacks.onSaveNow();
        }
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Save all tasks to disk now.\nStatus: %s", lastSavedStatus.c_str());
    }

    ImGui::SameLine();

    // Button: Clear Completed
    char clearBtnLabel[64];
    std::snprintf(clearBtnLabel, sizeof(clearBtnLabel), "Clear Done (%zu)", completedCount);
    const bool hasCompleted = completedCount > 0;
    if (!hasCompleted) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button(clearBtnLabel, ImVec2(125.0f, 28.0f))) {
        if (callbacks.onClearCompleted) {
            callbacks.onClearCompleted();
        }
    }
    if (!hasCompleted) {
        ImGui::EndDisabled();
    }

    ImGui::SameLine();

    // Button: Stats
    if (ImGui::Button("Stats", ImVec2(65.0f, 28.0f))) {
        if (callbacks.onOpenStats) {
            callbacks.onOpenStats();
        }
    }

    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ----------------------------------------------------
    // Control Bar: Search Input, Filters, and Sorting
    // ----------------------------------------------------
    bool filtersChanged = false;

    // Search bar
    ImGui::SetNextItemWidth(260.0f);
    if (ImGui::InputTextWithHint("##SearchInput", "Search by title or details...", searchBuffer_, sizeof(searchBuffer_))) {
        options.searchQuery = searchBuffer_;
        filtersChanged = true;
    }

    if (searchBuffer_[0] != '\0') {
        ImGui::SameLine();
        if (ImGui::SmallButton("X##ClearSearch")) {
            searchBuffer_[0] = '\0';
            options.searchQuery.clear();
            filtersChanged = true;
        }
    }

    ImGui::SameLine();

    // Filter by Status Dropdown
    ImGui::SetNextItemWidth(140.0f);
    const char* statusItems[] = {"Show: All", "Show: Pending", "Show: Completed", "Show: Overdue"};
    int currentStatusIdx = static_cast<int>(options.statusFilter);
    if (ImGui::Combo("##StatusFilter", &currentStatusIdx, statusItems, IM_ARRAYSIZE(statusItems))) {
        options.statusFilter = static_cast<StatusFilter>(currentStatusIdx);
        filtersChanged = true;
    }

    ImGui::SameLine();

    // Filter by Priority Dropdown
    ImGui::SetNextItemWidth(150.0f);
    const char* priorityItems[] = {"Priority: All", "Priority: High", "Priority: Med & High", "Priority: Low"};
    int currentPriorityIdx = static_cast<int>(options.priorityFilter);
    if (ImGui::Combo("##PriorityFilter", &currentPriorityIdx, priorityItems, IM_ARRAYSIZE(priorityItems))) {
        options.priorityFilter = static_cast<PriorityFilter>(currentPriorityIdx);
        filtersChanged = true;
    }

    ImGui::SameLine();

    // Sort By Dropdown
    ImGui::SetNextItemWidth(170.0f);
    const char* sortItems[] = {
        "Sort: Urgency",
        "Sort: Deadline",
        "Sort: Priority",
        "Sort: Alphabetical",
        "Sort: Manual"
    };
    int currentSortIdx = static_cast<int>(options.sortMode);
    if (ImGui::Combo("##SortMode", &currentSortIdx, sortItems, IM_ARRAYSIZE(sortItems))) {
        options.sortMode = static_cast<SortMode>(currentSortIdx);
        filtersChanged = true;
    }

    ImGui::SameLine();

    // Sort Direction Toggle Button
    const char* dirLabel = (options.sortDirection == SortDirection::Descending) ? "Desc" : "Asc";
    if (ImGui::Button(dirLabel, ImVec2(50.0f, 0.0f))) {
        options.sortDirection = (options.sortDirection == SortDirection::Descending)
            ? SortDirection::Ascending
            : SortDirection::Descending;
        filtersChanged = true;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Toggle sort direction (Ascending / Descending)");
    }

    if (filtersChanged && callbacks.onFilterChanged) {
        callbacks.onFilterChanged(options);
    }

    ImGui::Spacing();
}

} // namespace ToDo::View
