#pragma once

#include "Model/TaskManager.hpp"
#include <functional>
#include <string>

namespace ToDo::View {

struct ControlPanelCallbacks {
    std::function<void()> onOpenAddTask;
    std::function<void()> onSaveNow;
    std::function<void()> onClearCompleted;
    std::function<void()> onOpenStats;
    std::function<void(const FilterOptions&)> onFilterChanged;
};

class ControlPanelView {
public:
    ControlPanelView();

    void Render(FilterOptions& options,
                const std::string& todayDateStr,
                size_t completedCount,
                const std::string& lastSavedStatus,
                const ControlPanelCallbacks& callbacks);

private:
    char searchBuffer_[256]{0};
};

} // namespace ToDo::View
