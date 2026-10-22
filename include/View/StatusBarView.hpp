#pragma once

#include "Model/TaskManager.hpp"
#include <string>

namespace ToDo::View {

class StatusBarView {
public:
    StatusBarView() = default;

    void Render(const TaskStatistics& stats, const std::string& lastSaveStatus);
};

} // namespace ToDo::View
