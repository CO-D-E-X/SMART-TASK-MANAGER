#include "View/Theme.hpp"
#include <cstdio>
#include <cmath>

namespace ToDo::View {

void ApplyModernDarkTheme() {
    ImGuiStyle& style = ImGui::GetStyle();

    // Geometry & Ergonomics
    style.WindowRounding    = 10.0f;
    style.ChildRounding     = 8.0f;
    style.FrameRounding     = 6.0f;
    style.PopupRounding     = 8.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabRounding      = 6.0f;
    style.TabRounding       = 6.0f;

    style.WindowPadding     = ImVec2(16.0f, 16.0f);
    style.FramePadding      = ImVec2(12.0f, 7.0f);
    style.ItemSpacing       = ImVec2(10.0f, 8.0f);
    style.ItemInnerSpacing  = ImVec2(8.0f, 6.0f);
    style.CellPadding       = ImVec2(10.0f, 8.0f);
    style.ScrollbarSize     = 13.0f;

    style.WindowBorderSize  = 1.0f;
    style.ChildBorderSize   = 1.0f;
    style.PopupBorderSize   = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.TabBorderSize     = 0.0f;

    // Sleek Modern Dark Palette
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                  = ImVec4(0.92f, 0.94f, 0.97f, 1.00f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.55f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.10f, 0.11f, 0.14f, 1.00f);
    colors[ImGuiCol_ChildBg]               = ImVec4(0.12f, 0.14f, 0.17f, 0.85f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.12f, 0.13f, 0.17f, 0.98f);
    colors[ImGuiCol_Border]                = ImVec4(0.20f, 0.23f, 0.28f, 0.70f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]               = ImVec4(0.16f, 0.18f, 0.23f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.21f, 0.24f, 0.31f, 1.00f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.25f, 0.29f, 0.38f, 1.00f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.09f, 0.10f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.13f, 0.15f, 0.19f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.09f, 0.10f, 0.13f, 0.75f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.12f, 0.13f, 0.17f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.10f, 0.11f, 0.14f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.24f, 0.27f, 0.35f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.32f, 0.36f, 0.46f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.40f, 0.45f, 0.58f, 1.00f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.36f, 0.55f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.36f, 0.55f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.45f, 0.65f, 1.00f, 1.00f);
    colors[ImGuiCol_Button]                = ImVec4(0.20f, 0.23f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.28f, 0.33f, 0.43f, 1.00f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.35f, 0.42f, 0.55f, 1.00f);
    colors[ImGuiCol_Header]                = ImVec4(0.19f, 0.22f, 0.29f, 1.00f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.31f, 0.40f, 1.00f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.33f, 0.39f, 0.52f, 1.00f);
    colors[ImGuiCol_Separator]             = ImVec4(0.20f, 0.23f, 0.28f, 0.70f);
    colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.30f, 0.35f, 0.45f, 1.00f);
    colors[ImGuiCol_SeparatorActive]       = ImVec4(0.36f, 0.55f, 1.00f, 1.00f);
    colors[ImGuiCol_ResizeGrip]            = ImVec4(0.24f, 0.27f, 0.35f, 0.50f);
    colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.36f, 0.55f, 1.00f, 0.75f);
    colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.36f, 0.55f, 1.00f, 1.00f);
    colors[ImGuiCol_Tab]                   = ImVec4(0.14f, 0.16f, 0.21f, 1.00f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.26f, 0.31f, 0.42f, 1.00f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.21f, 0.25f, 0.34f, 1.00f);
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.12f, 0.13f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.16f, 0.18f, 0.24f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.15f, 0.17f, 0.22f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.22f, 0.26f, 0.33f, 0.90f);
    colors[ImGuiCol_TableBorderLight]      = ImVec4(0.18f, 0.21f, 0.26f, 0.50f);
    colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.02f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.36f, 0.42f, 0.95f, 0.35f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 0.82f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]          = ImVec4(0.36f, 0.55f, 1.00f, 1.00f);
}

void RenderPriorityBadge(Priority priority) {
    const char* label = PriorityToString(priority).c_str();
    ImVec4 color;
    switch (priority) {
        case Priority::High:   color = ThemeColors::PriorityHigh;   break;
        case Priority::Medium: color = ThemeColors::PriorityMedium; break;
        case Priority::Low:    color = ThemeColors::PriorityLow;    break;
    }

    const ImVec4 bgCol = ImVec4(color.x, color.y, color.z, 0.18f);
    const ImVec4 borderCol = ImVec4(color.x, color.y, color.z, 0.85f);

    const ImVec2 textSize = ImGui::CalcTextSize(label);
    const ImVec2 pad(9.0f, 3.0f);
    const ImVec2 badgeSize(textSize.x + pad.x * 2.0f, textSize.y + pad.y * 2.0f);
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    const ImVec2 pMax(pos.x + badgeSize.x, pos.y + badgeSize.y);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(pos, pMax, ImGui::GetColorU32(bgCol), 4.0f);
    drawList->AddRect(pos, pMax, ImGui::GetColorU32(borderCol), 4.0f, 0, 1.0f);
    drawList->AddText(ImVec2(pos.x + pad.x, pos.y + pad.y), ImGui::GetColorU32(color), label);

    ImGui::Dummy(badgeSize);
}

void RenderUrgencyBadge(double score, int daysRemaining, bool isCompleted) {
    if (isCompleted) {
        ImGui::TextColored(ThemeColors::CompletedMuted, "Done");
        return;
    }

    ImVec4 color;
    const char* category = "Low";
    if (daysRemaining < 0) {
        color = ThemeColors::OverdueRed;
        category = "OVERDUE";
    } else if (score >= 90.0) {
        color = ThemeColors::PriorityHigh;
        category = "Critical";
    } else if (score >= 50.0) {
        color = ThemeColors::PriorityMedium;
        category = "High";
    } else if (score >= 25.0) {
        color = ImVec4(0.95f, 0.85f, 0.20f, 1.0f); // Warm yellow
        category = "Moderate";
    } else {
        color = ThemeColors::PriorityLow;
        category = "Low";
    }

    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.1f (%s)", score, category);

    const ImVec2 textSize = ImGui::CalcTextSize(buf);
    const ImVec2 pad(8.0f, 3.0f);
    const ImVec2 badgeSize(textSize.x + pad.x * 2.0f, textSize.y + pad.y * 2.0f);
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    const ImVec2 pMax(pos.x + badgeSize.x, pos.y + badgeSize.y);

    const ImVec4 bgCol = ImVec4(color.x, color.y, color.z, 0.16f);
    const ImVec4 borderCol = ImVec4(color.x, color.y, color.z, 0.75f);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(pos, pMax, ImGui::GetColorU32(bgCol), 4.0f);
    drawList->AddRect(pos, pMax, ImGui::GetColorU32(borderCol), 4.0f, 0, 1.0f);
    drawList->AddText(ImVec2(pos.x + pad.x, pos.y + pad.y), ImGui::GetColorU32(color), buf);

    ImGui::Dummy(badgeSize);
}

void RenderStatusBadge(TaskStatus status, bool isOverdue) {
    if (status == TaskStatus::Completed) {
        ImGui::TextColored(ThemeColors::CompletedMuted, "[DONE]");
    } else if (isOverdue) {
        ImGui::TextColored(ThemeColors::OverdueRed, "[OVERDUE]");
    } else {
        ImGui::TextColored(ImVec4(0.36f, 0.65f, 1.00f, 1.00f), "[PENDING]");
    }
}

} // namespace ToDo::View
