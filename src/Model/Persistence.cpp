#include "Model/Persistence.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

namespace ToDo {

using json = nlohmann::json;

namespace {

json TaskToJson(const Task& task) {
    json item;
    item["id"] = task.id;
    item["title"] = task.title;
    item["description"] = task.description;
    item["priority"] = PriorityToString(task.priority);
    item["deadline"] = task.deadline;
    item["completed"] = (task.status == TaskStatus::Completed);
    item["manual_order"] = task.manual_order;
    item["created_at"] = task.created_at;
    item["completed_at"] = task.completed_at;
    return item;
}

Task JsonToTask(const json& item) {
    Task task;
    task.id = item.value("id", static_cast<uint64_t>(0));
    task.title = item.value("title", "");
    task.description = item.value("description", "");
    task.priority = StringToPriority(item.value("priority", "Medium"));
    task.deadline = item.value("deadline", GetCurrentDateString());
    const bool completed = item.value("completed", false);
    task.status = completed ? TaskStatus::Completed : TaskStatus::Pending;
    task.manual_order = item.value("manual_order", static_cast<uint64_t>(0));
    task.created_at = item.value("created_at", static_cast<int64_t>(0));
    task.completed_at = item.value("completed_at", "");
    return task;
}

} // anonymous namespace

bool Persistence::SaveToFile(const std::string& filepath,
                             const std::vector<Task>& tasks,
                             const std::vector<Task>& history) {
    try {
        json jTasks = json::array();
        for (const auto& task : tasks) {
            jTasks.push_back(TaskToJson(task));
        }

        json jHistory = json::array();
        for (const auto& task : history) {
            jHistory.push_back(TaskToJson(task));
        }

        json root;
        root["version"] = 1;
        root["tasks"] = std::move(jTasks);
        root["history"] = std::move(jHistory);

        std::ofstream file(filepath, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            std::cerr << "[Persistence] Failed to open file for writing: " << filepath << std::endl;
            return false;
        }

        file << root.dump(4);
        file.close();
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "[Persistence] Exception saving tasks: " << ex.what() << std::endl;
        return false;
    }
}

bool Persistence::LoadFromFile(const std::string& filepath,
                               std::vector<Task>& outTasks,
                               std::vector<Task>& outHistory) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }

        json root;
        file >> root;
        file.close();

        std::vector<Task> loadedTasks;
        if (root.contains("tasks") && root["tasks"].is_array()) {
            for (const auto& item : root["tasks"]) {
                loadedTasks.push_back(JsonToTask(item));
            }
        }

        std::vector<Task> loadedHistory;
        if (root.contains("history") && root["history"].is_array()) {
            for (const auto& item : root["history"]) {
                loadedHistory.push_back(JsonToTask(item));
            }
        }

        outTasks = std::move(loadedTasks);
        outHistory = std::move(loadedHistory);
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "[Persistence] Exception loading tasks: " << ex.what() << std::endl;
        return false;
    }
}

bool Persistence::ExportToCSV(const std::string& filepath,
                              const std::vector<Task>& tasks,
                              const std::vector<Task>& history) {
    try {
        std::ofstream file(filepath, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            return false;
        }

        file << "Type,ID,Title,Description,Priority,Deadline,Status,CreatedAt,CompletedAt\n";

        auto escapeCSV = [](const std::string& field) -> std::string {
            std::string escaped = "\"";
            for (char c : field) {
                if (c == '"') escaped += "\"\"";
                else escaped += c;
            }
            escaped += "\"";
            return escaped;
        };

        for (const auto& t : tasks) {
            file << "Active," << t.id << ","
                 << escapeCSV(t.title) << ","
                 << escapeCSV(t.description) << ","
                 << PriorityToString(t.priority) << ","
                 << t.deadline << ","
                 << (t.status == TaskStatus::Completed ? "Completed" : "Pending") << ","
                 << t.created_at << ","
                 << escapeCSV(t.completed_at) << "\n";
        }

        for (const auto& t : history) {
            file << "History," << t.id << ","
                 << escapeCSV(t.title) << ","
                 << escapeCSV(t.description) << ","
                 << PriorityToString(t.priority) << ","
                 << t.deadline << ","
                 << "Cleared/Completed,"
                 << t.created_at << ","
                 << escapeCSV(t.completed_at) << "\n";
        }

        file.close();
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "[Persistence] Exception exporting CSV: " << ex.what() << std::endl;
        return false;
    }
}

} // namespace ToDo
