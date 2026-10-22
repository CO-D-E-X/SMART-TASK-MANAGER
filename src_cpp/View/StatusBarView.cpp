#include "View/StatusBarView.hpp"
#include "View/Theme.hpp"

#include <imgui.h>
#include <cstdio>

namespace ToDo::View {

void StatusBarView::Render(const TaskStatistics& stats, const std::string& lastSaveStatus) {
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::BeginGroup();

    // Metric Badges
    ImGui::TextDisabled("Total: %zu", stats.totalTasks);
    ImGui::SameLine();
    ImGui::Text(" | ");
    ImGui::SameLine();

    ImGui::TextColored(ThemeColors::PriorityLow, "Completed: %zu", stats.completedTasks);
    ImGui::SameLine();
    ImGui::Text(" | ");
    ImGui::SameLine();

    ImGui::TextColored(ImVec4(0.45f, 0.70f, 1.00f, 1.0f), "Pending: %zu", stats.pendingTasks);
    ImGui::SameLine();
    ImGui::Text(" | ");
    ImGui::SameLine();

    if (stats.overdueTasks > 0) {
        ImGui::TextColored(ThemeColors::OverdueRed, "Overdue: %zu", stats.overdueTasks);
    } else {
        ImGui::TextDisabled("Overdue: 0");
    }

    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();

    // Progress Bar
    char progressText[48];
    std::snprintf(progressText, sizeof(progressText), "%.0f%% Done", stats.completionRate * 100.0f);
    ImGui::SetNextItemWidth(180.0f);
    ImGui::ProgressBar(stats.completionRate, ImVec2(180.0f, 16.0f), progressText);

    ImGui::EndGroup();

    // Right-aligned Auto-Save status
    const ImVec2 statusTextSize = ImGui::CalcTextSize(lastSaveStatus.c_str());
    const float availWidth = ImGui::GetContentRegionAvail().x;
    if (availWidth > statusTextSize.x + 10.0f) {
        ImGui::SameLine(ImGui::GetWindowWidth() - statusTextSize.x - 24.0f);
        ImGui::TextDisabled("%s", lastSaveStatus.c_str());
    }
}

} // namespace ToDo::View
