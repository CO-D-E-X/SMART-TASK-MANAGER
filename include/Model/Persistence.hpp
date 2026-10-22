#pragma once

#include "Model/Task.hpp"

#include <vector>
#include <string>

namespace ToDo {

class Persistence {
public:
    static bool SaveToFile(const std::string& filepath,
                           const std::vector<Task>& tasks,
                           const std::vector<Task>& history = {});
    static bool LoadFromFile(const std::string& filepath,
                             std::vector<Task>& outTasks,
                             std::vector<Task>& outHistory);
    static bool ExportToCSV(const std::string& filepath,
                            const std::vector<Task>& tasks,
                            const std::vector<Task>& history = {});
};

} // namespace ToDo
