#pragma once

#include "Model/Task.hpp"

#include <vector>
#include <string>
#include <optional>
#include <functional>
#include <cstdint>

namespace ToDo {

enum class SortMode {
    Urgency = 0,
    Deadline,
    Priority,
    Alphabetical,
    Manual
};

enum class SortDirection {
    Descending = 0,
    Ascending
};

enum class StatusFilter {
    All = 0,
    PendingOnly,
    CompletedOnly,
    OverdueOnly
};

enum class PriorityFilter {
    All = 0,
    HighOnly,
    MediumAndHigh,
    LowOnly
};

struct FilterOptions {
    std::string searchQuery;
    StatusFilter statusFilter{StatusFilter::All};
    PriorityFilter priorityFilter{PriorityFilter::All};
    SortMode sortMode{SortMode::Urgency};
    SortDirection sortDirection{SortDirection::Descending};
};

struct TaskStatistics {
    size_t totalTasks{0};
    size_t completedTasks{0};
    size_t pendingTasks{0};
    size_t overdueTasks{0};
    size_t highPriorityPending{0};
    size_t mediumPriorityPending{0};
    size_t lowPriorityPending{0};
    float completionRate{0.0f}; // 0.0f to 1.0f
    double averageUrgency{0.0};
};

class TaskManager {
public:
    using ChangeListener = std::function<void()>;

    TaskManager();

    // CRUD Operations
    uint64_t AddTask(std::string title, std::string description, Priority priority, std::string deadline);
    bool UpdateTask(uint64_t id, const std::string& title, const std::string& description, Priority priority, const std::string& deadline);
    bool DeleteTask(uint64_t id);
    bool ToggleTaskStatus(uint64_t id);
    bool SetTaskStatus(uint64_t id, TaskStatus status);
    size_t ClearCompletedTasks();

    // Query & Search
    [[nodiscard]] std::optional<Task> GetTaskById(uint64_t id) const;
    [[nodiscard]] const std::vector<Task>& GetAllTasks() const { return tasks_; }
    [[nodiscard]] std::vector<Task> GetFilteredAndSortedTasks(const FilterOptions& options, const std::string& todayDateStr) const;
    [[nodiscard]] TaskStatistics GetStatistics(const std::string& todayDateStr) const;

    // Direct data management (used by persistence)
    void LoadTasks(std::vector<Task> tasks, std::vector<Task> history = {});
    void SeedSampleTasksIfEmpty();

    // Task History Management
    [[nodiscard]] const std::vector<Task>& GetHistoryTasks() const { return historyTasks_; }
    [[nodiscard]] std::vector<Task> GetFilteredHistoryTasks(const std::string& searchQuery) const;
    bool RestoreTaskFromHistory(uint64_t id);
    bool DeleteTaskFromHistory(uint64_t id);
    void ClearAllHistory();

    // Change Notification
    void SetChangeListener(ChangeListener listener) { changeListener_ = std::move(listener); }

private:
    void NotifyChanged();
    [[nodiscard]] uint64_t GenerateNextId();

    std::vector<Task> tasks_;
    std::vector<Task> historyTasks_;
    uint64_t nextId_{1};
    uint64_t nextManualOrder_{1};
    ChangeListener changeListener_;
};

} // namespace ToDo
