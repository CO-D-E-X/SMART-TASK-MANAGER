#pragma once

#include "Model/TaskManager.hpp"
#include "Model/Persistence.hpp"
#include "View/ControlPanelView.hpp"
#include "View/TaskTableView.hpp"
#include "View/HistoryTableView.hpp"
#include "View/ModalsView.hpp"
#include "View/StatusBarView.hpp"

#include <memory>
#include <string>

namespace ToDo {

class AppController {
public:
    explicit AppController(std::string dataFilePath = "tasks.json");
    ~AppController();

    void Initialize();
    void RenderUI();
    void Shutdown();

    void SaveTasks();
    void LoadTasks();

private:
    std::string GetFormattedTimestamp();

    std::string dataFilePath_;
    std::string todayDateStr_;
    std::string lastSaveStatus_{"Ready"};

    TaskManager model_;
    FilterOptions currentFilters_;

    // Views
    View::ControlPanelView controlPanelView_;
    View::TaskTableView taskTableView_;
    View::HistoryTableView historyTableView_;
    View::ModalsView modalsView_;
    View::StatusBarView statusBarView_;
};

} // namespace ToDo
