#pragma once

#include "Model/Task.hpp"
#include <imgui.h>

namespace ToDo::View {

struct ThemeColors {
    static constexpr ImVec4 PriorityLow{0.18f, 0.80f, 0.44f, 1.0f};       // Emerald Green
    static constexpr ImVec4 PriorityMedium{0.95f, 0.61f, 0.07f, 1.0f};    // Amber Warm Yellow
    static constexpr ImVec4 PriorityHigh{0.91f, 0.30f, 0.24f, 1.0f};      // Coral Red
    static constexpr ImVec4 OverdueRed{1.00f, 0.22f, 0.22f, 1.0f};        // Crimson
    static constexpr ImVec4 CompletedMuted{0.50f, 0.55f, 0.58f, 0.60f};   // Dimmed Slate
    static constexpr ImVec4 AccentPrimary{0.36f, 0.42f, 0.95f, 1.0f};     // Electric Indigo
    static constexpr ImVec4 AccentHover{0.45f, 0.52f, 1.00f, 1.0f};
    static constexpr ImVec4 AccentActive{0.28f, 0.34f, 0.80f, 1.0f};
    static constexpr ImVec4 CardBg{0.14f, 0.16f, 0.20f, 1.0f};
    static constexpr ImVec4 Background{0.09f, 0.10f, 0.13f, 1.0f};
};

void ApplyModernDarkTheme();

// Custom UI Badges & Visual Widgets
void RenderPriorityBadge(Priority priority);
void RenderUrgencyBadge(double score, int daysRemaining, bool isCompleted);
void RenderStatusBadge(TaskStatus status, bool isOverdue);

} // namespace ToDo::View
