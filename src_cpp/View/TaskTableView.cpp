#include "View/TaskTableView.hpp"
#include "View/Theme.hpp"

#include <imgui.h>
#include <cstdio>
#include <string>

namespace ToDo::View {

void TaskTableView::Render(const std::vector<Task>& tasks,
                           const std::string& todayDateStr,
                           const TaskTableCallbacks& callbacks) {
    if (tasks.empty()) {
        ImGui::BeginChild("EmptyStateChild", ImVec2(0, 200), true);
        ImGui::Spacing();
        ImGui::Spacing();
        const char* emptyMsg = "No tasks found matching your filter or search criteria.";
        const char* hintMsg = "Create a new task using the '+ Add Task' button or clear your active filters.";
        
        const float windowWidth = ImGui::GetWindowSize().x;
        const float textWidth = ImGui::CalcTextSize(emptyMsg).x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::TextColored(ImVec4(0.70f, 0.75f, 0.82f, 1.0f), "%s", emptyMsg);

        const float hintWidth = ImGui::CalcTextSize(hintMsg).x;
        ImGui::SetCursorPosX((windowWidth - hintWidth) * 0.5f);
        ImGui::TextDisabled("%s", hintMsg);
        ImGui::EndChild();
        return;
    }

    const ImGuiTableFlags tableFlags =
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_BordersInnerH |
        ImGuiTableFlags_BordersOuter |
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_Reorderable |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_SizingStretchProp;

    const float tableHeight = ImGui::GetContentRegionAvail().y;

    if (ImGui::BeginTable("TasksTable", 6, tableFlags, ImVec2(0, tableHeight))) {
        ImGui::TableSetupColumn("Done", ImGuiTableColumnFlags_WidthFixed, 42.0f);
        ImGui::TableSetupColumn("Title & Description", ImGuiTableColumnFlags_WidthStretch, 2.5f);
        ImGui::TableSetupColumn("Priority", ImGuiTableColumnFlags_WidthFixed, 105.0f);
        ImGui::TableSetupColumn("Deadline", ImGuiTableColumnFlags_WidthFixed, 155.0f);
        ImGui::TableSetupColumn("Urgency Score", ImGuiTableColumnFlags_WidthFixed, 160.0f);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 140.0f);

        ImGui::TableSetupScrollFreeze(0, 1); // Freeze header row
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < tasks.size(); ++i) {
            const auto& task = tasks[i];
            ImGui::PushID(static_cast<int>(task.id));

            ImGui::TableNextRow(ImGuiTableRowFlags_None, 36.0f);

            const bool isCompleted = task.IsCompleted();
            const bool isOverdue = task.IsOverdue(todayDateStr);
            const int daysRemaining = task.GetDaysRemaining(todayDateStr);
            const double urgency = task.GetUrgencyScore(todayDateStr);

            // Column 0: Checkbox (Complete)
            ImGui::TableSetColumnIndex(0);
            bool checked = isCompleted;
            if (ImGui::Checkbox("##check", &checked)) {
                if (callbacks.onToggleComplete) {
                    callbacks.onToggleComplete(task.id);
                }
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip(isCompleted ? "Mark as pending" : "Mark as completed");
            }

            // Column 1: Title & Description
            ImGui::TableSetColumnIndex(1);
            if (isCompleted) {
                ImGui::PushStyleColor(ImGuiCol_Text, ThemeColors::CompletedMuted);
            }

            ImGui::TextUnformatted(task.title.c_str());

            // Strikethrough effect on title if completed
            if (isCompleted) {
                const ImVec2 textMin = ImGui::GetItemRectMin();
                const ImVec2 textMax = ImGui::GetItemRectMax();
                const float midY = (textMin.y + textMax.y) * 0.5f;
                ImGui::GetWindowDrawList()->AddLine(
                    ImVec2(textMin.x, midY),
                    ImVec2(textMax.x, midY),
                    ImGui::GetColorU32(ThemeColors::CompletedMuted),
                    1.5f
                );
            }

            if (!task.description.empty()) {
                ImGui::TextDisabled("%.60s%s", task.description.c_str(),
                                    task.description.size() > 60 ? "..." : "");
            }

            if (isCompleted) {
                ImGui::PopStyleColor();
            }

            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Task ID: #%llu", static_cast<unsigned long long>(task.id));
                ImGui::Text("Title: %s", task.title.c_str());
                if (!task.description.empty()) {
                    ImGui::Separator();
                    ImGui::TextWrapped("Description: %s", task.description.c_str());
                }
                ImGui::EndTooltip();
            }

            // Column 2: Priority Badge
            ImGui::TableSetColumnIndex(2);
            RenderPriorityBadge(task.priority);

            // Column 3: Deadline
            ImGui::TableSetColumnIndex(3);
            if (isCompleted) {
                ImGui::TextColored(ThemeColors::CompletedMuted, "%s", task.deadline.c_str());
            } else if (isOverdue) {
                ImGui::TextColored(ThemeColors::OverdueRed, "%s", task.deadline.c_str());
                ImGui::SameLine();
                ImGui::TextColored(ThemeColors::OverdueRed, "(%dd late)", -daysRemaining);
            } else if (daysRemaining == 0) {
                ImGui::TextColored(ImVec4(1.0f, 0.70f, 0.20f, 1.0f), "%s (Today)", task.deadline.c_str());
            } else if (daysRemaining == 1) {
                ImGui::TextColored(ImVec4(0.85f, 0.85f, 0.40f, 1.0f), "%s (Tmrw)", task.deadline.c_str());
            } else {
                ImGui::Text("%s (+%dd)", task.deadline.c_str(), daysRemaining);
            }

            // Column 4: Urgency Score & Breakdown
            ImGui::TableSetColumnIndex(4);
            RenderUrgencyBadge(urgency, daysRemaining, isCompleted);
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Dynamic Urgency Breakdown:");
                ImGui::BulletText("Base Priority Weight: %.1fx (%s)",
                                  GetPriorityWeight(task.priority),
                                  PriorityToString(task.priority).c_str());
                if (isCompleted) {
                    ImGui::BulletText("Status: Completed (0.0 score)");
                } else if (isOverdue) {
                    ImGui::BulletText("Days Overdue: %d days (critical boost +100)", -daysRemaining);
                } else if (daysRemaining == 0) {
                    ImGui::BulletText("Due: Today (high focus +80)");
                } else {
                    ImGui::BulletText("Days Remaining: %d days", daysRemaining);
                    ImGui::BulletText("Decay Model: (weight * 40) / (days + 1) + bonus");
                }
                ImGui::Separator();
                ImGui::Text("Final Score: %.2f", urgency);
                ImGui::EndTooltip();
            }

            // Column 5: Action Buttons [Edit] [Delete]
            ImGui::TableSetColumnIndex(5);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.22f, 0.26f, 0.35f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.32f, 0.38f, 0.52f, 1.0f));
            if (ImGui::SmallButton("Edit")) {
                if (callbacks.onEdit) {
                    callbacks.onEdit(task);
                }
            }
            ImGui::PopStyleColor(2);

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.38f, 0.16f, 0.16f, 0.8f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.60f, 0.22f, 0.22f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.25f, 0.25f, 1.0f));
            if (ImGui::SmallButton("Delete")) {
                if (callbacks.onDelete) {
                    callbacks.onDelete(task.id, task.title);
                }
            }
            ImGui::PopStyleColor(3);

            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}

} // namespace ToDo::View
