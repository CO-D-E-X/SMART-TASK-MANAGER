#pragma once

#include <string>
#include <cstdint>
#include <chrono>
#include <optional>

namespace ToDo {

enum class Priority {
    Low = 1,
    Medium = 2,
    High = 3
};

enum class TaskStatus {
    Pending = 0,
    Completed = 1
};

struct Task {
    uint64_t id{0};
    std::string title;
    std::string description;
    Priority priority{Priority::Medium};
    std::string deadline; // "YYYY-MM-DD"
    TaskStatus status{TaskStatus::Pending};
    uint64_t manual_order{0};
    int64_t created_at{0}; // Unix timestamp in seconds
    std::string completed_at; // Timestamp or date when completed/cleared

    // Dynamic calculations
    [[nodiscard]] double GetUrgencyScore(const std::string& todayDateStr) const;
    [[nodiscard]] int GetDaysRemaining(const std::string& todayDateStr) const;
    [[nodiscard]] bool IsOverdue(const std::string& todayDateStr) const;
    [[nodiscard]] bool IsCompleted() const { return status == TaskStatus::Completed; }
};

// Date utility functions
[[nodiscard]] std::string GetCurrentDateString();
[[nodiscard]] std::string GetCurrentTimestampString();
[[nodiscard]] bool IsValidDateString(const std::string& dateStr);
[[nodiscard]] int CalculateDaysDifference(const std::string& fromDateStr, const std::string& toDateStr);
[[nodiscard]] std::string AddDaysToDate(const std::string& baseDateStr, int daysToAdd);

// Priority helpers
[[nodiscard]] std::string PriorityToString(Priority priority);
[[nodiscard]] Priority StringToPriority(const std::string& str);
[[nodiscard]] double GetPriorityWeight(Priority priority);

} // namespace ToDo
