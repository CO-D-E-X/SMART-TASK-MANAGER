#pragma once

#include "Model/Task.hpp"
#include <vector>
#include <functional>
#include <string>

namespace ToDo::View {

struct HistoryTableCallbacks {
    std::function<void(uint64_t id)> onRestore;
    std::function<void(uint64_t id)> onDelete;
    std::function<void()> onClearAllHistory;
};

class HistoryTableView {
public:
    HistoryTableView();

    void Render(const std::vector<Task>& historyTasks,
                const HistoryTableCallbacks& callbacks);

private:
    char historySearchBuffer_[256]{0};
};

} // namespace ToDo::View
