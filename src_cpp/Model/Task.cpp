#include "Model/Task.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace ToDo {

namespace {

bool ParseDate(const std::string& dateStr, std::chrono::year_month_day& outYmd) {
    if (dateStr.size() != 10 || dateStr[4] != '-' || dateStr[7] != '-') {
        return false;
    }

    int y = 0;
    int m = 0;
    int d = 0;

    try {
        y = std::stoi(dateStr.substr(0, 4));
        m = std::stoi(dateStr.substr(5, 2));
        d = std::stoi(dateStr.substr(8, 2));
    } catch (...) {
        return false;
    }

    auto ymd = std::chrono::year_month_day{
        std::chrono::year{y},
        std::chrono::month{static_cast<unsigned>(m)},
        std::chrono::day{static_cast<unsigned>(d)}
    };

    if (!ymd.ok()) {
        return false;
    }

    outYmd = ymd;
    return true;
}

std::string FormatYmd(const std::chrono::year_month_day& ymd) {
    int y = static_cast<int>(ymd.year());
    unsigned m = static_cast<unsigned>(ymd.month());
    unsigned d = static_cast<unsigned>(ymd.day());
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%04d-%02u-%02u", y, m, d);
    return std::string(buf);
}

} // anonymous namespace

std::string GetCurrentDateString() {
    const auto now = std::chrono::system_clock::now();
    const auto dp = std::chrono::floor<std::chrono::days>(now);
    const std::chrono::year_month_day ymd{dp};
    return FormatYmd(ymd);
}

std::string GetCurrentTimestampString() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buf);
}

bool IsValidDateString(const std::string& dateStr) {
    std::chrono::year_month_day dummy;
    return ParseDate(dateStr, dummy);
}

int CalculateDaysDifference(const std::string& fromDateStr, const std::string& toDateStr) {
    std::chrono::year_month_day fromYmd;
    std::chrono::year_month_day toYmd;

    if (!ParseDate(fromDateStr, fromYmd) || !ParseDate(toDateStr, toYmd)) {
        return 0;
    }

    const std::chrono::sys_days fromDays{fromYmd};
    const std::chrono::sys_days toDays{toYmd};

    return static_cast<int>((toDays - fromDays).count());
}

std::string AddDaysToDate(const std::string& baseDateStr, int daysToAdd) {
    std::chrono::year_month_day ymd;
    if (!ParseDate(baseDateStr, ymd)) {
        return baseDateStr;
    }

    const std::chrono::sys_days sysDays{ymd};
    const std::chrono::sys_days targetDays = sysDays + std::chrono::days{daysToAdd};
    const std::chrono::year_month_day targetYmd{targetDays};

    return FormatYmd(targetYmd);
}

std::string PriorityToString(Priority priority) {
    switch (priority) {
        case Priority::Low:    return "Low";
        case Priority::Medium: return "Medium";
        case Priority::High:   return "High";
        default:               return "Medium";
    }
}

Priority StringToPriority(const std::string& str) {
    if (str == "Low" || str == "low" || str == "1") return Priority::Low;
    if (str == "High" || str == "high" || str == "3") return Priority::High;
    return Priority::Medium;
}

double GetPriorityWeight(Priority priority) {
    switch (priority) {
        case Priority::Low:    return 1.0;
        case Priority::Medium: return 2.5;
        case Priority::High:   return 5.0;
        default:               return 2.5;
    }
}

int Task::GetDaysRemaining(const std::string& todayDateStr) const {
    return CalculateDaysDifference(todayDateStr, deadline);
}

bool Task::IsOverdue(const std::string& todayDateStr) const {
    if (status == TaskStatus::Completed) {
        return false;
    }
    return GetDaysRemaining(todayDateStr) < 0;
}

double Task::GetUrgencyScore(const std::string& todayDateStr) const {
    if (status == TaskStatus::Completed) {
        return 0.0;
    }

    const double weight = GetPriorityWeight(priority);
    const int daysLeft = GetDaysRemaining(todayDateStr);

    if (daysLeft < 0) {
        // Overdue: critical priority (> 100 points)
        const double overdueDays = static_cast<double>(-daysLeft);
        return 100.0 + (weight * 15.0) + (std::min(overdueDays, 30.0) * 2.5);
    } else if (daysLeft == 0) {
        // Due today: immediate attention needed
        return 80.0 + (weight * 12.0);
    } else {
        // Upcoming: dynamic decay curve inversely proportional to days remaining
        return ((weight * 40.0) / (static_cast<double>(daysLeft) + 1.0)) + (weight * 3.0);
    }
}

} // namespace ToDo
