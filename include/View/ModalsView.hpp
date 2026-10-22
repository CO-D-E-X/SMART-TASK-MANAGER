#pragma once

#include "Model/Task.hpp"
#include "Model/TaskManager.hpp"

#include <functional>
#include <string>

namespace ToDo::View {

struct ModalCallbacks {
    std::function<void(const std::string& title, const std::string& desc, Priority priority, const std::string& deadline)> onConfirmAdd;
    std::function<void(uint64_t id, const std::string& title, const std::string& desc, Priority priority, const std::string& deadline)> onConfirmEdit;
    std::function<void(uint64_t id)> onConfirmDelete;
};

class ModalsView {
public:
    ModalsView();

    void OpenAddTaskModal(const std::string& defaultDateStr);
    void OpenEditTaskModal(const Task& task);
    void OpenDeleteModal(uint64_t id, const std::string& title);
    void OpenStatsModal();

    void Render(const std::string& todayDateStr,
                const TaskStatistics& stats,
                const ModalCallbacks& callbacks);

private:
    bool showAddModal_{false};
    bool showEditModal_{false};
    bool showDeleteModal_{false};
    bool showStatsModal_{false};

    // Add Task state
    char addTitle_[128]{0};
    char addDesc_[512]{0};
    int addPriority_{2}; // Medium default
    char addDeadline_[32]{0};
    std::string addErrorMsg_;

    // Edit Task state
    uint64_t editId_{0};
    char editTitle_[128]{0};
    char editDesc_[512]{0};
    int editPriority_{2};
    char editDeadline_[32]{0};
    std::string editErrorMsg_;

    // Delete confirmation state
    uint64_t deleteId_{0};
    std::string deleteTitle_;
};

} // namespace ToDo::View
