#include "View/HistoryTableView.hpp"
#include "View/Theme.hpp"

#include <imgui.h>
#include <algorithm>
#include <cctype>
#include <cstdio>

namespace ToDo::View {

namespace {

std::string ToLower(std::string_view str) {
    std::string result(str);
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

} // anonymous namespace

HistoryTableView::HistoryTableView() {
    historySearchBuffer_[0] = '\0';
}

void HistoryTableView::Render(const std::vector<Task>& historyTasks,
                             const HistoryTableCallbacks& callbacks) {
    // ----------------------------------------------------
    // Top Bar: Search History and Clear All History
    // ----------------------------------------------------
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 8.0f));

    ImGui::SetNextItemWidth(280.0f);
    ImGui::InputTextWithHint("##HistorySearchInput", "Search archived history...", historySearchBuffer_, sizeof(historySearchBuffer_));

    if (historySearchBuffer_[0] != '\0') {
        ImGui::SameLine();
        if (ImGui::SmallButton("X##ClearHistSearch")) {
            historySearchBuffer_[0] = '\0';
        }
    }

    ImGui::SameLine();

    const bool hasHistory = !historyTasks.empty();
    const float clearBtnWidth = 140.0f;
    const float avail = ImGui::GetContentRegionAvail().x;
    if (avail > clearBtnWidth) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail - clearBtnWidth));
    }

    if (!hasHistory) {
        ImGui::BeginDisabled();
    }
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.16f, 0.16f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.55f, 0.22f, 0.22f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.70f, 0.25f, 0.25f, 1.0f));
    if (ImGui::Button("Clear All History", ImVec2(clearBtnWidth, 26.0f))) {
        if (callbacks.onClearAllHistory) {
            callbacks.onClearAllHistory();
        }
    }
    ImGui::PopStyleColor(3);
    if (!hasHistory) {
        ImGui::EndDisabled();
    }

    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ----------------------------------------------------
    // Filter history by search buffer
    // ----------------------------------------------------
    std::vector<const Task*> displayTasks;
    displayTasks.reserve(historyTasks.size());

    const std::string lowerQuery = ToLower(historySearchBuffer_);
    for (const auto& task : historyTasks) {
        if (!lowerQuery.empty()) {
            const std::string lowerTitle = ToLower(task.title);
            const std::string lowerDesc = ToLower(task.description);
            if (lowerTitle.find(lowerQuery) == std::string::npos &&
                lowerDesc.find(lowerQuery) == std::string::npos) {
                continue;
            }
        }
        displayTasks.push_back(&task);
    }

    if (displayTasks.empty()) {
        ImGui::BeginChild("EmptyHistoryChild", ImVec2(0, 180), true);
        ImGui::Spacing();
        ImGui::Spacing();
        const char* emptyMsg = hasHistory
            ? "No archived tasks match your search query."
            : "No archived tasks in history yet.";
        const char* hintMsg = hasHistory
            ? "Clear the search box above to see all history."
            : "When you mark tasks as completed and click 'Clear Done', they will be preserved here.";

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

    // ----------------------------------------------------
    // History Table
    // ----------------------------------------------------
    const ImGuiTableFlags tableFlags =
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_BordersInnerH |
        ImGuiTableFlags_BordersOuter |
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_Reorderable |
        ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_SizingStretchProp;

    const float tableHeight = ImGui::GetContentRegionAvail().y;

    if (ImGui::BeginTable("HistoryTasksTable", 6, tableFlags, ImVec2(0, tableHeight))) {
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 55.0f);
        ImGui::TableSetupColumn("Title & Description", ImGuiTableColumnFlags_WidthStretch, 2.5f);
        ImGui::TableSetupColumn("Priority", ImGuiTableColumnFlags_WidthFixed, 105.0f);
        ImGui::TableSetupColumn("Deadline", ImGuiTableColumnFlags_WidthFixed, 130.0f);
        ImGui::TableSetupColumn("Completed On", ImGuiTableColumnFlags_WidthFixed, 165.0f);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 150.0f);

        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        for (const auto* taskPtr : displayTasks) {
            const auto& task = *taskPtr;
            ImGui::PushID(static_cast<int>(task.id));

            ImGui::TableNextRow(ImGuiTableRowFlags_None, 34.0f);

            // Column 0: Status Checkmark
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ThemeColors::PriorityLow, " [Done]");

            // Column 1: Title & Description
            ImGui::TableSetColumnIndex(1);
            ImGui::PushStyleColor(ImGuiCol_Text, ThemeColors::CompletedMuted);
            ImGui::TextUnformatted(task.title.c_str());

            // Strikethrough line
            const ImVec2 textMin = ImGui::GetItemRectMin();
            const ImVec2 textMax = ImGui::GetItemRectMax();
            const float midY = (textMin.y + textMax.y) * 0.5f;
            ImGui::GetWindowDrawList()->AddLine(
                ImVec2(textMin.x, midY),
                ImVec2(textMax.x, midY),
                ImGui::GetColorU32(ThemeColors::CompletedMuted),
                1.2f
            );

            if (!task.description.empty()) {
                ImGui::TextDisabled("%.60s%s", task.description.c_str(),
                                    task.description.size() > 60 ? "..." : "");
            }
            ImGui::PopStyleColor();

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
            ImGui::TextDisabled("%s", task.deadline.c_str());

            // Column 4: Completed On
            ImGui::TableSetColumnIndex(4);
            if (!task.completed_at.empty()) {
                ImGui::TextColored(ImVec4(0.40f, 0.75f, 0.55f, 1.0f), "%s", task.completed_at.c_str());
            } else {
                ImGui::TextDisabled("Completed");
            }

            // Column 5: Action Buttons [Restore] [Delete]
            ImGui::TableSetColumnIndex(5);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.32f, 0.50f, 0.90f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.28f, 0.44f, 0.70f, 1.0f));
            if (ImGui::SmallButton("Restore")) {
                if (callbacks.onRestore) {
                    callbacks.onRestore(task.id);
                }
            }
            ImGui::PopStyleColor(2);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Restore this task back to the Active tasks list as pending");
            }

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.38f, 0.16f, 0.16f, 0.8f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.60f, 0.22f, 0.22f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.25f, 0.25f, 1.0f));
            if (ImGui::SmallButton("Delete")) {
                if (callbacks.onDelete) {
                    callbacks.onDelete(task.id);
                }
            }
            ImGui::PopStyleColor(3);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Permanently delete this archived task");
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}

} // namespace ToDo::View
