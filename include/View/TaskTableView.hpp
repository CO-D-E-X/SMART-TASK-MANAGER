#pragma once

#include "Model/Task.hpp"
#include <vector>
#include <functional>
#include <string>

namespace ToDo::View {

struct TaskTableCallbacks {
    std::function<void(uint64_t id)> onToggleComplete;
    std::function<void(const Task& task)> onEdit;
    std::function<void(uint64_t id, const std::string& title)> onDelete;
};

class TaskTableView {
public:
    TaskTableView() = default;

    void Render(const std::vector<Task>& tasks,
                const std::string& todayDateStr,
                const TaskTableCallbacks& callbacks);
};

} // namespace ToDo::View
