#include "Model/Task.hpp"
#include "Model/TaskManager.hpp"
#include "Model/Persistence.hpp"

#include <iostream>
#include <cassert>
#include <filesystem>
#include <cmath>

void RunTests() {
    std::cout << "========================================" << std::endl;
    std::cout << "   RUNNING SMART TO-DO DOMAIN TESTS     " << std::endl;
    std::cout << "========================================" << std::endl;

    const std::string today = "2026-09-16";

    // 1. Date Calculations
    {
        std::cout << "[Test 1] Date Arithmetic & Formatting... ";
        assert(ToDo::IsValidDateString("2026-09-16"));
        assert(!ToDo::IsValidDateString("invalid-date"));
        assert(!ToDo::IsValidDateString("2026-02-31"));

        int diff = ToDo::CalculateDaysDifference("2026-09-16", "2026-09-19");
        assert(diff == 3);
        (void)diff;

        int overdueDiff = ToDo::CalculateDaysDifference("2026-09-16", "2026-09-14");
        assert(overdueDiff == -2);
        (void)overdueDiff;

        std::string tomorrow = ToDo::AddDaysToDate("2026-09-16", 1);
        assert(tomorrow == "2026-09-17");

        std::string yesterday = ToDo::AddDaysToDate("2026-09-16", -1);
        assert(yesterday == "2026-09-15");
        std::cout << "PASSED" << std::endl;
    }

    // 2. Dynamic Urgency Score
    {
        std::cout << "[Test 2] Dynamic Urgency Calculation... ";
        ToDo::Task overdueHigh;
        overdueHigh.priority = ToDo::Priority::High;
        overdueHigh.deadline = "2026-09-15"; // -1 day overdue
        overdueHigh.status = ToDo::TaskStatus::Pending;
        double scoreOverdueHigh = overdueHigh.GetUrgencyScore(today);
        assert(scoreOverdueHigh > 100.0);
        (void)scoreOverdueHigh;

        ToDo::Task overdueMed;
        overdueMed.priority = ToDo::Priority::Medium;
        overdueMed.deadline = "2026-09-15";
        overdueMed.status = ToDo::TaskStatus::Pending;
        double scoreOverdueMed = overdueMed.GetUrgencyScore(today);
        assert(scoreOverdueHigh > scoreOverdueMed); // High priority overdue ranks higher than Medium overdue
        (void)scoreOverdueMed;

        ToDo::Task todayHigh;
        todayHigh.priority = ToDo::Priority::High;
        todayHigh.deadline = today;
        todayHigh.status = ToDo::TaskStatus::Pending;
        double scoreTodayHigh = todayHigh.GetUrgencyScore(today);
        assert(scoreTodayHigh > 80.0);
        (void)scoreTodayHigh;

        ToDo::Task futureLow;
        futureLow.priority = ToDo::Priority::Low;
        futureLow.deadline = "2026-10-16"; // 30 days away
        futureLow.status = ToDo::TaskStatus::Pending;
        double scoreFutureLow = futureLow.GetUrgencyScore(today);
        assert(scoreFutureLow < scoreTodayHigh);
        (void)scoreFutureLow;

        ToDo::Task completedTask = todayHigh;
        completedTask.status = ToDo::TaskStatus::Completed;
        assert(completedTask.GetUrgencyScore(today) == 0.0);

        std::cout << "PASSED" << std::endl;
    }

    // 3. TaskManager CRUD
    {
        std::cout << "[Test 3] TaskManager CRUD Operations... ";
        ToDo::TaskManager manager;
        uint64_t id1 = manager.AddTask("Test Task 1", "Desc 1", ToDo::Priority::High, "2026-09-20");
        assert(id1 > 0);
        assert(manager.GetAllTasks().size() == 1);

        auto optTask = manager.GetTaskById(id1);
        assert(optTask.has_value());
        assert(optTask->title == "Test Task 1");

        bool updated = manager.UpdateTask(id1, "Updated Title", "Updated Desc", ToDo::Priority::Low, "2026-09-22");
        assert(updated);
        (void)updated;
        assert(manager.GetTaskById(id1)->title == "Updated Title");
        assert(manager.GetTaskById(id1)->priority == ToDo::Priority::Low);

        manager.ToggleTaskStatus(id1);
        assert(manager.GetTaskById(id1)->status == ToDo::TaskStatus::Completed);

        manager.ToggleTaskStatus(id1);
        assert(manager.GetTaskById(id1)->status == ToDo::TaskStatus::Pending);

        bool deleted = manager.DeleteTask(id1);
        assert(deleted);
        (void)deleted;
        assert(manager.GetAllTasks().empty());
        std::cout << "PASSED" << std::endl;
    }

    // 4. Multi-Criteria Sorting Engine
    {
        std::cout << "[Test 4] Dynamic Multi-Criteria Sorting... ";
        ToDo::TaskManager manager;
        // Seed 4 tasks:
        // A: Low due in 10 days
        uint64_t idA = manager.AddTask("A Task", "Desc", ToDo::Priority::Low, "2026-09-26");
        // B: High due tomorrow
        uint64_t idB = manager.AddTask("B Task", "Desc", ToDo::Priority::High, "2026-09-17");
        // C: High overdue by 2 days
        uint64_t idC = manager.AddTask("C Task", "Desc", ToDo::Priority::High, "2026-09-14");
        // D: Medium due today
        uint64_t idD = manager.AddTask("D Task", "Desc", ToDo::Priority::Medium, "2026-09-16");

        // Sort by Urgency Descending
        ToDo::FilterOptions optUrgency;
        optUrgency.sortMode = ToDo::SortMode::Urgency;
        optUrgency.sortDirection = ToDo::SortDirection::Descending;
        auto sortedByUrgency = manager.GetFilteredAndSortedTasks(optUrgency, today);
        assert(sortedByUrgency.size() == 4);
        assert(sortedByUrgency[0].id == idC); // Overdue High must be first!
        assert(sortedByUrgency[1].id == idB || sortedByUrgency[1].id == idD);
        assert(sortedByUrgency[3].id == idA); // Low far in future must be last

        // Sort by Alphabetical Ascending
        ToDo::FilterOptions optAlpha;
        optAlpha.sortMode = ToDo::SortMode::Alphabetical;
        optAlpha.sortDirection = ToDo::SortDirection::Ascending;
        auto sortedByAlpha = manager.GetFilteredAndSortedTasks(optAlpha, today);
        assert(sortedByAlpha[0].title == "A Task");
        assert(sortedByAlpha[1].title == "B Task");
        assert(sortedByAlpha[2].title == "C Task");
        assert(sortedByAlpha[3].title == "D Task");

        // Sort by Deadline Ascending
        ToDo::FilterOptions optDeadline;
        optDeadline.sortMode = ToDo::SortMode::Deadline;
        optDeadline.sortDirection = ToDo::SortDirection::Ascending;
        auto sortedByDeadline = manager.GetFilteredAndSortedTasks(optDeadline, today);
        assert(sortedByDeadline[0].id == idC); // 2026-09-14 earliest
        assert(sortedByDeadline[1].id == idD); // 2026-09-16 today
        assert(sortedByDeadline[2].id == idB); // 2026-09-17 tomorrow
        assert(sortedByDeadline[3].id == idA); // 2026-09-26 latest

        (void)idA;
        (void)idB;
        (void)idC;
        (void)idD;
        std::cout << "PASSED" << std::endl;
    }

    // 5. Filtering & Search
    {
        std::cout << "[Test 5] Live Search & Filtering... ";
        ToDo::TaskManager manager;
        manager.AddTask("Fix Compiler Warnings", "Modern C++20 standard", ToDo::Priority::High, "2026-09-18");
        manager.AddTask("Buy Coffee Beans", "Espresso roast", ToDo::Priority::Low, "2026-09-19");
        uint64_t doneId = manager.AddTask("Deploy Website", "Production release", ToDo::Priority::Medium, "2026-09-15");
        manager.SetTaskStatus(doneId, ToDo::TaskStatus::Completed);

        // Search substring
        ToDo::FilterOptions optSearch;
        optSearch.searchQuery = "compiler";
        auto searchResults = manager.GetFilteredAndSortedTasks(optSearch, today);
        assert(searchResults.size() == 1);
        assert(searchResults[0].title == "Fix Compiler Warnings");

        // Search in description
        optSearch.searchQuery = "espresso";
        searchResults = manager.GetFilteredAndSortedTasks(optSearch, today);
        assert(searchResults.size() == 1);
        assert(searchResults[0].title == "Buy Coffee Beans");

        // Filter Completed Only
        ToDo::FilterOptions optCompleted;
        optCompleted.statusFilter = ToDo::StatusFilter::CompletedOnly;
        auto completedResults = manager.GetFilteredAndSortedTasks(optCompleted, today);
        assert(completedResults.size() == 1);
        assert(completedResults[0].id == doneId);

        // Filter Pending Only
        ToDo::FilterOptions optPending;
        optPending.statusFilter = ToDo::StatusFilter::PendingOnly;
        auto pendingResults = manager.GetFilteredAndSortedTasks(optPending, today);
        assert(pendingResults.size() == 2);

        std::cout << "PASSED" << std::endl;
    }

    // 6. Persistence Save & Load
    {
        std::cout << "[Test 6] Persistence Save & Load... ";
        const std::string testFile = "test_tasks.json";
        const std::string testCsv = "test_tasks.csv";

        ToDo::TaskManager manager;
        manager.AddTask("Persistence Task 1", "Detailed Description with \"quotes\" and commas, test", ToDo::Priority::High, "2026-09-20");
        manager.AddTask("Persistence Task 2", "Another task", ToDo::Priority::Low, "2026-09-25");

        bool saved = ToDo::Persistence::SaveToFile(testFile, manager.GetAllTasks(), {});
        assert(saved);
        (void)saved;

        std::vector<ToDo::Task> loadedTasks;
        std::vector<ToDo::Task> loadedHistory;
        bool loaded = ToDo::Persistence::LoadFromFile(testFile, loadedTasks, loadedHistory);
        assert(loaded);
        (void)loaded;
        assert(loadedTasks.size() == 2);
        assert(loadedTasks[0].title == "Persistence Task 1");
        assert(loadedTasks[0].priority == ToDo::Priority::High);
        assert(loadedTasks[0].description == "Detailed Description with \"quotes\" and commas, test");

        bool csvExported = ToDo::Persistence::ExportToCSV(testCsv, manager.GetAllTasks(), {});
        assert(csvExported);
        (void)csvExported;

        std::filesystem::remove(testFile);
        std::filesystem::remove(testCsv);
        std::cout << "PASSED" << std::endl;
    }

    // 7. Statistics
    {
        std::cout << "[Test 7] Productivity Statistics... ";
        ToDo::TaskManager manager;
        manager.AddTask("Overdue Task", "desc", ToDo::Priority::High, "2026-09-10"); // overdue
        manager.AddTask("Pending Task", "desc", ToDo::Priority::Medium, "2026-09-20");
        uint64_t cId = manager.AddTask("Done Task", "desc", ToDo::Priority::Low, "2026-09-18");
        manager.SetTaskStatus(cId, ToDo::TaskStatus::Completed);

        auto stats = manager.GetStatistics(today);
        assert(stats.totalTasks == 3);
        assert(stats.completedTasks == 1);
        assert(stats.pendingTasks == 2);
        assert(stats.overdueTasks == 1);
        assert(std::abs(stats.completionRate - (1.0f / 3.0f)) < 0.01f);
        (void)stats;
        std::cout << "PASSED" << std::endl;
    }

    // 8. Task History Management (Archiving, Restoring, Deleting)
    {
        std::cout << "[Test 8] Task History & Archiving... ";
        ToDo::TaskManager manager;
        uint64_t id1 = manager.AddTask("Active Task", "Keep active", ToDo::Priority::High, "2026-09-20");
        uint64_t id2 = manager.AddTask("Done Task", "To be cleared", ToDo::Priority::Medium, "2026-09-15");
        manager.SetTaskStatus(id2, ToDo::TaskStatus::Completed);

        // Clear completed -> moves id2 to history!
        size_t cleared = manager.ClearCompletedTasks();
        assert(cleared == 1);
        (void)cleared;
        (void)id1;
        assert(manager.GetAllTasks().size() == 1);
        assert(manager.GetAllTasks()[0].id == id1);
        assert(manager.GetHistoryTasks().size() == 1);
        assert(manager.GetHistoryTasks()[0].id == id2);
        assert(!manager.GetHistoryTasks()[0].completed_at.empty());

        // Restore task back to active!
        bool restored = manager.RestoreTaskFromHistory(id2);
        assert(restored);
        (void)restored;
        assert(manager.GetAllTasks().size() == 2);
        assert(manager.GetHistoryTasks().empty());
        assert(manager.GetTaskById(id2)->status == ToDo::TaskStatus::Pending);

        // Re-complete and re-clear
        manager.SetTaskStatus(id2, ToDo::TaskStatus::Completed);
        manager.ClearCompletedTasks();
        assert(manager.GetHistoryTasks().size() == 1);

        // Save & reload history
        const std::string histFile = "test_history.json";
        ToDo::Persistence::SaveToFile(histFile, manager.GetAllTasks(), manager.GetHistoryTasks());
        std::vector<ToDo::Task> reloadedTasks;
        std::vector<ToDo::Task> reloadedHistory;
        ToDo::Persistence::LoadFromFile(histFile, reloadedTasks, reloadedHistory);
        assert(reloadedTasks.size() == 1);
        assert(reloadedHistory.size() == 1);
        assert(reloadedHistory[0].title == "Done Task");
        std::filesystem::remove(histFile);

        // Delete from history
        bool deletedFromHist = manager.DeleteTaskFromHistory(id2);
        assert(deletedFromHist);
        (void)deletedFromHist;
        assert(manager.GetHistoryTasks().empty());

        std::cout << "PASSED" << std::endl;
    }

    std::cout << "========================================" << std::endl;
    std::cout << "  ALL CORE DOMAIN TESTS PASSED (8/8)!   " << std::endl;
    std::cout << "========================================" << std::endl;
}

int main() {
    RunTests();
    return 0;
}
