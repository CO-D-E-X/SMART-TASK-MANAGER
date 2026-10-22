#include "Model/TaskManager.hpp"

#include <algorithm>
#include <tuple>
#include <chrono>
#include <cctype>

namespace ToDo {

namespace {

std::string ToLower(std::string_view str) {
    std::string result(str);
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

} // anonymous namespace

TaskManager::TaskManager() = default;

uint64_t TaskManager::GenerateNextId() {
    return nextId_++;
}

uint64_t TaskManager::AddTask(std::string title, std::string description, Priority priority, std::string deadline) {
    Task task;
    task.id = GenerateNextId();
    task.title = std::move(title);
    task.description = std::move(description);
    task.priority = priority;
    task.deadline = std::move(deadline);
    task.status = TaskStatus::Pending;
    task.manual_order = nextManualOrder_++;
    task.created_at = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    tasks_.push_back(std::move(task));
    NotifyChanged();
    return tasks_.back().id;
}

bool TaskManager::UpdateTask(uint64_t id, const std::string& title, const std::string& description, Priority priority, const std::string& deadline) {
    for (auto& task : tasks_) {
        if (task.id == id) {
            task.title = title;
            task.description = description;
            task.priority = priority;
            task.deadline = deadline;
            NotifyChanged();
            return true;
        }
    }
    return false;
}

bool TaskManager::DeleteTask(uint64_t id) {
    const auto it = std::remove_if(tasks_.begin(), tasks_.end(),
                                  [id](const Task& t) { return t.id == id; });
    if (it != tasks_.end()) {
        tasks_.erase(it, tasks_.end());
        NotifyChanged();
        return true;
    }
    return false;
}

bool TaskManager::ToggleTaskStatus(uint64_t id) {
    for (auto& task : tasks_) {
        if (task.id == id) {
            task.status = (task.status == TaskStatus::Pending) ? TaskStatus::Completed : TaskStatus::Pending;
            NotifyChanged();
            return true;
        }
    }
    return false;
}

bool TaskManager::SetTaskStatus(uint64_t id, TaskStatus status) {
    for (auto& task : tasks_) {
        if (task.id == id) {
            if (task.status != status) {
                task.status = status;
                NotifyChanged();
            }
            return true;
        }
    }
    return false;
}

size_t TaskManager::ClearCompletedTasks() {
    size_t movedCount = 0;
    const std::string nowStr = GetCurrentTimestampString();

    std::vector<Task> remaining;
    remaining.reserve(tasks_.size());

    for (auto& task : tasks_) {
        if (task.status == TaskStatus::Completed) {
            if (task.completed_at.empty()) {
                task.completed_at = nowStr;
            }
            historyTasks_.insert(historyTasks_.begin(), std::move(task));
            movedCount++;
        } else {
            remaining.push_back(std::move(task));
        }
    }

    if (movedCount > 0) {
        tasks_ = std::move(remaining);
        NotifyChanged();
    }

    return movedCount;
}

std::optional<Task> TaskManager::GetTaskById(uint64_t id) const {
    for (const auto& task : tasks_) {
        if (task.id == id) {
            return task;
        }
    }
    return std::nullopt;
}

std::vector<Task> TaskManager::GetFilteredAndSortedTasks(const FilterOptions& options, const std::string& todayDateStr) const {
    std::vector<Task> result;
    result.reserve(tasks_.size());

    const std::string lowerQuery = ToLower(options.searchQuery);

    for (const auto& task : tasks_) {
        // 1. Status Filter
        switch (options.statusFilter) {
            case StatusFilter::PendingOnly:
                if (task.status != TaskStatus::Pending) continue;
                break;
            case StatusFilter::CompletedOnly:
                if (task.status != TaskStatus::Completed) continue;
                break;
            case StatusFilter::OverdueOnly:
                if (!task.IsOverdue(todayDateStr)) continue;
                break;
            case StatusFilter::All:
            default:
                break;
        }

        // 2. Priority Filter
        switch (options.priorityFilter) {
            case PriorityFilter::HighOnly:
                if (task.priority != Priority::High) continue;
                break;
            case PriorityFilter::MediumAndHigh:
                if (task.priority != Priority::High && task.priority != Priority::Medium) continue;
                break;
            case PriorityFilter::LowOnly:
                if (task.priority != Priority::Low) continue;
                break;
            case PriorityFilter::All:
            default:
                break;
        }

        // 3. Search Query Filter (Substring on title and description)
        if (!lowerQuery.empty()) {
            const std::string lowerTitle = ToLower(task.title);
            const std::string lowerDesc = ToLower(task.description);
            if (lowerTitle.find(lowerQuery) == std::string::npos &&
                lowerDesc.find(lowerQuery) == std::string::npos) {
                continue;
            }
        }

        result.push_back(task);
    }

    // 4. Multi-Criteria Sorting Engine
    std::sort(result.begin(), result.end(), [&](const Task& a, const Task& b) {
        const double scoreA = a.GetUrgencyScore(todayDateStr);
        const double scoreB = b.GetUrgencyScore(todayDateStr);
        const int priorityA = static_cast<int>(a.priority);
        const int priorityB = static_cast<int>(b.priority);
        const std::string titleA = ToLower(a.title);
        const std::string titleB = ToLower(b.title);

        if (options.sortMode == SortMode::Urgency) {
            if (options.sortDirection == SortDirection::Descending) {
                // Highest urgency score first, tie-break with priority, deadline, title
                return std::tie(scoreB, priorityB, a.deadline, titleA) <
                       std::tie(scoreA, priorityA, b.deadline, titleB);
            } else {
                return std::tie(scoreA, priorityA, a.deadline, titleA) <
                       std::tie(scoreB, priorityB, b.deadline, titleB);
            }
        } else if (options.sortMode == SortMode::Deadline) {
            if (options.sortDirection == SortDirection::Ascending) {
                // Earliest deadline first, tie-break with priority and urgency
                return std::tie(a.deadline, priorityB, scoreB, titleA) <
                       std::tie(b.deadline, priorityA, scoreA, titleB);
            } else {
                return std::tie(b.deadline, priorityA, scoreA, titleB) <
                       std::tie(a.deadline, priorityB, scoreB, titleA);
            }
        } else if (options.sortMode == SortMode::Priority) {
            if (options.sortDirection == SortDirection::Descending) {
                // High -> Medium -> Low
                return std::tie(priorityB, scoreB, a.deadline, titleA) <
                       std::tie(priorityA, scoreA, b.deadline, titleB);
            } else {
                return std::tie(priorityA, scoreA, a.deadline, titleA) <
                       std::tie(priorityB, scoreB, b.deadline, titleB);
            }
        } else if (options.sortMode == SortMode::Alphabetical) {
            if (options.sortDirection == SortDirection::Ascending) {
                return std::tie(titleA, priorityB) < std::tie(titleB, priorityA);
            } else {
                return std::tie(titleB, priorityA) < std::tie(titleA, priorityB);
            }
        } else if (options.sortMode == SortMode::Manual) {
            if (options.sortDirection == SortDirection::Ascending) {
                return a.manual_order < b.manual_order;
            } else {
                return a.manual_order > b.manual_order;
            }
        }

        return a.id < b.id;
    });

    return result;
}

TaskStatistics TaskManager::GetStatistics(const std::string& todayDateStr) const {
    TaskStatistics stats;
    stats.totalTasks = tasks_.size();

    double totalUrgency = 0.0;
    size_t pendingCount = 0;

    for (const auto& task : tasks_) {
        if (task.status == TaskStatus::Completed) {
            stats.completedTasks++;
        } else {
            stats.pendingTasks++;
            pendingCount++;
            totalUrgency += task.GetUrgencyScore(todayDateStr);

            if (task.IsOverdue(todayDateStr)) {
                stats.overdueTasks++;
            }

            switch (task.priority) {
                case Priority::High:   stats.highPriorityPending++; break;
                case Priority::Medium: stats.mediumPriorityPending++; break;
                case Priority::Low:    stats.lowPriorityPending++; break;
            }
        }
    }

    if (stats.totalTasks > 0) {
        stats.completionRate = static_cast<float>(stats.completedTasks) / static_cast<float>(stats.totalTasks);
    }

    if (pendingCount > 0) {
        stats.averageUrgency = totalUrgency / static_cast<double>(pendingCount);
    }

    return stats;
}

void TaskManager::LoadTasks(std::vector<Task> tasks, std::vector<Task> history) {
    tasks_ = std::move(tasks);
    historyTasks_ = std::move(history);
    uint64_t maxId = 0;
    uint64_t maxOrder = 0;
    for (const auto& task : tasks_) {
        if (task.id > maxId) maxId = task.id;
        if (task.manual_order > maxOrder) maxOrder = task.manual_order;
    }
    for (const auto& task : historyTasks_) {
        if (task.id > maxId) maxId = task.id;
        if (task.manual_order > maxOrder) maxOrder = task.manual_order;
    }
    nextId_ = maxId + 1;
    nextManualOrder_ = maxOrder + 1;
}

std::vector<Task> TaskManager::GetFilteredHistoryTasks(const std::string& searchQuery) const {
    if (searchQuery.empty()) {
        return historyTasks_;
    }

    const std::string lowerQuery = ToLower(searchQuery);
    std::vector<Task> result;
    for (const auto& task : historyTasks_) {
        const std::string lowerTitle = ToLower(task.title);
        const std::string lowerDesc = ToLower(task.description);
        if (lowerTitle.find(lowerQuery) != std::string::npos ||
            lowerDesc.find(lowerQuery) != std::string::npos) {
            result.push_back(task);
        }
    }
    return result;
}

bool TaskManager::RestoreTaskFromHistory(uint64_t id) {
    for (auto it = historyTasks_.begin(); it != historyTasks_.end(); ++it) {
        if (it->id == id) {
            Task task = std::move(*it);
            historyTasks_.erase(it);

            task.status = TaskStatus::Pending;
            task.manual_order = nextManualOrder_++;
            task.completed_at.clear();
            tasks_.push_back(std::move(task));

            NotifyChanged();
            return true;
        }
    }
    return false;
}

bool TaskManager::DeleteTaskFromHistory(uint64_t id) {
    const auto it = std::remove_if(historyTasks_.begin(), historyTasks_.end(),
                                  [id](const Task& t) { return t.id == id; });
    if (it != historyTasks_.end()) {
        historyTasks_.erase(it, historyTasks_.end());
        NotifyChanged();
        return true;
    }
    return false;
}

void TaskManager::ClearAllHistory() {
    if (!historyTasks_.empty()) {
        historyTasks_.clear();
        NotifyChanged();
    }
}

void TaskManager::SeedSampleTasksIfEmpty() {
    if (!tasks_.empty()) {
        return;
    }

    const std::string today = GetCurrentDateString();

    // 1. High priority overdue task
    AddTask("Deploy v2.4 Security Patch",
            "Apply zero-day mitigations and restart production ingress gateway nodes.",
            Priority::High,
            AddDaysToDate(today, -1));

    // 2. High priority due today
    AddTask("Submit C++ Project Milestone Report",
            "Prepare dynamic task prioritization metrics and architectural documentation.",
            Priority::High,
            today);

    // 3. Medium priority due tomorrow
    AddTask("Review Pull Requests & Merge Tests",
            "Inspect CMake FetchContent dependencies and ensure zero raw memory leaks.",
            Priority::Medium,
            AddDaysToDate(today, 1));

    // 4. Medium priority due in 3 days
    AddTask("Refactor GUI Layout with Dear ImGui",
            "Enhance table responsiveness, badge aesthetics, and custom dark mode colors.",
            Priority::Medium,
            AddDaysToDate(today, 3));

    // 5. Low priority due next week
    AddTask("Archive Q3 Project Assets",
            "Upload finalized sprint artifacts to cloud storage and compress local cache.",
            Priority::Low,
            AddDaysToDate(today, 7));

    // 6. Completed task
    const uint64_t doneId = AddTask("Setup Modern C++20 Toolchain",
                                   "Installed GCC 16, CMake, OpenGL, and GLFW libraries.",
                                   Priority::Medium,
                                   AddDaysToDate(today, -3));
    SetTaskStatus(doneId, TaskStatus::Completed);
}

void TaskManager::NotifyChanged() {
    if (changeListener_) {
        changeListener_();
    }
}

} // namespace ToDo
