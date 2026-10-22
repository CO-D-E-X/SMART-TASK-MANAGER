import React, { useState, useEffect, useMemo } from 'react';
import { Header } from './components/Header';
import { ControlPanel } from './components/ControlPanel';
import { TaskCard } from './components/TaskCard';
import { TaskHistory } from './components/TaskHistory';
import { AddTaskModal } from './components/AddTaskModal';
import { EditTaskModal } from './components/EditTaskModal';
import { DeleteModal } from './components/DeleteModal';
import { StatsModal } from './components/StatsModal';
import { ExportModal } from './components/ExportModal';
import { AboutModal } from './components/AboutModal';
import { StatusBar } from './components/StatusBar';

import { Task, FilterOptions, Priority, TaskReminder } from './types/task';
import {
  getCurrentDateString,
  getCurrentTimestampString,
  filterAndSortTasks,
  computeTaskStatistics,
} from './utils/urgency';
import { loadLocalData, saveLocalData } from './utils/storage';
import {
  initNotificationService,
  scheduleTaskReminder,
  cancelTaskReminder,
  syncAllReminders,
} from './utils/notificationService';
import { ListTodo, Archive, Plus } from 'lucide-react';

export const App: React.FC = () => {
  const [tasks, setTasks] = useState<Task[]>([]);
  const [history, setHistory] = useState<Task[]>([]);
  const [activeTab, setActiveTab] = useState<'active' | 'history'>('active');
  const [lastSavedTime, setLastSavedTime] = useState<string>('Ready');

  // Filter & Sort State
  const [filterOptions, setFilterOptions] = useState<FilterOptions>({
    searchQuery: '',
    statusFilter: 'All',
    priorityFilter: 'All',
    sortMode: 'Urgency',
    sortDirection: 'Desc',
  });

  // Modals state
  const [isAboutOpen, setIsAboutOpen] = useState(false);
  const [isAddTaskOpen, setIsAddTaskOpen] = useState(false);
  const [isEditTaskOpen, setIsEditTaskOpen] = useState(false);
  const [editingTask, setEditingTask] = useState<Task | null>(null);
  const [isDeleteOpen, setIsDeleteOpen] = useState(false);
  const [deletingTaskId, setDeletingTaskId] = useState<number | null>(null);
  const [deletingTaskTitle, setDeletingTaskTitle] = useState('');
  const [isStatsOpen, setIsStatsOpen] = useState(false);
  const [isExportOpen, setIsExportOpen] = useState(false);

  const todayDateStr = useMemo(() => getCurrentDateString(), []);

  // Initial Data Load & Notification Sync
  useEffect(() => {
    const data = loadLocalData();
    setTasks(data.tasks);
    setHistory(data.history);
    setLastSavedTime(`Saved (${new Date().toLocaleTimeString()})`);

    // Initialize local notification channels and alarms
    initNotificationService().then(() => {
      syncAllReminders(data.tasks);
    });
  }, []);

  // Sync to local storage on changes
  const persistChanges = (newTasks: Task[], newHistory: Task[]) => {
    setTasks(newTasks);
    setHistory(newHistory);
    saveLocalData(newTasks, newHistory);
    setLastSavedTime(`Saved (${new Date().toLocaleTimeString()})`);
  };

  // CRUD Operations
  const handleAddTask = (
    title: string,
    description: string,
    priority: Priority,
    deadline: string,
    reminder?: TaskReminder
  ) => {
    const maxId = tasks.reduce((max, t) => Math.max(max, t.id), history.reduce((max, t) => Math.max(max, t.id), 0));
    const maxOrder = tasks.reduce((max, t) => Math.max(max, t.manual_order), 0);

    const newTask: Task = {
      id: maxId + 1,
      title,
      description,
      priority,
      deadline,
      status: 'Pending',
      manual_order: maxOrder + 1,
      created_at: Math.floor(Date.now() / 1000),
      reminder,
    };

    const updated = [newTask, ...tasks];
    persistChanges(updated, history);

    // Schedule notification if reminder enabled
    if (reminder?.enabled) {
      scheduleTaskReminder(newTask);
    }
  };

  const handleEditTask = (
    id: number,
    title: string,
    description: string,
    priority: Priority,
    deadline: string,
    reminder?: TaskReminder
  ) => {
    let updatedTaskObj: Task | null = null;
    const updated = tasks.map((t) => {
      if (t.id === id) {
        const item: Task = { ...t, title, description, priority, deadline, reminder };
        updatedTaskObj = item;
        return item;
      }
      return t;
    });

    persistChanges(updated, history);

    if (updatedTaskObj) {
      if (reminder?.enabled) {
        scheduleTaskReminder(updatedTaskObj);
      } else {
        cancelTaskReminder(id);
      }
    }
  };

  const handleToggleTask = (id: number) => {
    const updated = tasks.map((t) => {
      if (t.id === id) {
        const nextStatus: import('./types/task').TaskStatus = t.status === 'Completed' ? 'Pending' : 'Completed';
        const updatedTask = {
          ...t,
          status: nextStatus,
          completed_at: nextStatus === 'Completed' ? getCurrentTimestampString() : undefined,
        };

        if (nextStatus === 'Completed') {
          cancelTaskReminder(id);
        } else if (t.reminder?.enabled) {
          scheduleTaskReminder(updatedTask);
        }

        return updatedTask;
      }
      return t;
    });

    persistChanges(updated, history);
  };

  const handleDeleteTask = (id: number) => {
    cancelTaskReminder(id);
    const updated = tasks.filter((t) => t.id !== id);
    persistChanges(updated, history);
    setIsDeleteOpen(false);
  };

  const handleClearCompleted = () => {
    const completed = tasks.filter((t) => t.status === 'Completed');
    if (completed.length === 0) return;

    // Cancel any notifications
    completed.forEach((t) => cancelTaskReminder(t.id));

    const remaining = tasks.filter((t) => t.status !== 'Completed');
    const newHistory = [...completed, ...history];
    persistChanges(remaining, newHistory);
  };

  // History Actions
  const handleRestoreFromHistory = (id: number) => {
    const target = history.find((t) => t.id === id);
    if (!target) return;

    const newHistory = history.filter((t) => t.id !== id);
    const restored: Task = {
      ...target,
      status: 'Pending',
      completed_at: undefined,
      manual_order: tasks.length + 1,
    };
    persistChanges([restored, ...tasks], newHistory);

    if (restored.reminder?.enabled) {
      scheduleTaskReminder(restored);
    }
  };

  const handleDeleteFromHistory = (id: number) => {
    const newHistory = history.filter((t) => t.id !== id);
    persistChanges(tasks, newHistory);
  };

  const handleClearAllHistory = () => {
    if (window.confirm('Are you sure you want to permanently clear all archived history?')) {
      persistChanges(tasks, []);
    }
  };

  const handleImportData = (importedTasks: Task[], importedHistory: Task[]) => {
    persistChanges(importedTasks, importedHistory);
    syncAllReminders(importedTasks);
  };

  // Filtered & Sorted Active Tasks
  const filteredTasks = useMemo(() => {
    return filterAndSortTasks(tasks, filterOptions, todayDateStr);
  }, [tasks, filterOptions, todayDateStr]);

  // Statistics
  const statistics = useMemo(() => {
    return computeTaskStatistics(tasks, todayDateStr);
  }, [tasks, todayDateStr]);

  return (
    <div className="min-h-screen bg-[#090a0d] text-slate-100 flex flex-col font-sans selection:bg-indigo-600 selection:text-white pb-safe">
      {/* Top Header */}
      <Header
        todayDateStr={todayDateStr}
        completedCount={statistics.completedTasks}
        onOpenAbout={() => setIsAboutOpen(true)}
        onOpenAddTask={() => setIsAddTaskOpen(true)}
        onOpenStats={() => setIsStatsOpen(true)}
        onOpenExport={() => setIsExportOpen(true)}
        onClearCompleted={handleClearCompleted}
      />

      {/* Main Container */}
      <main className="flex-1 max-w-7xl w-full mx-auto p-4 sm:p-6 flex flex-col">
        {/* Navigation Tabs */}
        <div className="flex items-center space-x-2 border-b border-slate-800 pb-3 mb-4">
          <button
            onClick={() => setActiveTab('active')}
            className={`flex items-center gap-2 px-4 py-2 rounded-xl text-xs sm:text-sm font-semibold transition-all cursor-pointer ${
              activeTab === 'active'
                ? 'bg-indigo-600 text-white shadow-lg shadow-indigo-600/30'
                : 'text-slate-400 hover:text-white hover:bg-slate-850'
            }`}
          >
            <ListTodo className="w-4 h-4" />
            <span>Active Tasks</span>
            <span
              className={`text-[11px] px-2 py-0.5 rounded-full font-mono ${
                activeTab === 'active' ? 'bg-indigo-800/80 text-white' : 'bg-slate-800 text-slate-400'
              }`}
            >
              {tasks.length}
            </span>
          </button>

          <button
            onClick={() => setActiveTab('history')}
            className={`flex items-center gap-2 px-4 py-2 rounded-xl text-xs sm:text-sm font-semibold transition-all cursor-pointer ${
              activeTab === 'history'
                ? 'bg-indigo-600 text-white shadow-lg shadow-indigo-600/30'
                : 'text-slate-400 hover:text-white hover:bg-slate-850'
            }`}
          >
            <Archive className="w-4 h-4" />
            <span>Task History Vault</span>
            <span
              className={`text-[11px] px-2 py-0.5 rounded-full font-mono ${
                activeTab === 'history' ? 'bg-indigo-800/80 text-white' : 'bg-slate-800 text-slate-400'
              }`}
            >
              {history.length}
            </span>
          </button>
        </div>

        {/* Tab Content */}
        {activeTab === 'active' ? (
          <div>
            {/* Search & Filters */}
            <ControlPanel options={filterOptions} onChange={setFilterOptions} />

            {/* Tasks List */}
            {filteredTasks.length === 0 ? (
              <div className="text-center py-16 px-4 rounded-2xl bg-slate-900/40 border border-slate-800/80 my-4">
                <ListTodo className="w-12 h-12 text-slate-600 mx-auto mb-3" />
                <h3 className="text-base font-semibold text-slate-300">No matching tasks found</h3>
                <p className="text-xs text-slate-500 mt-1 max-w-sm mx-auto mb-4">
                  {filterOptions.searchQuery
                    ? 'No tasks matched your current search filters.'
                    : 'Get started by creating your first prioritized task!'}
                </p>
                <button
                  onClick={() => setIsAddTaskOpen(true)}
                  className="inline-flex items-center gap-1.5 px-4 py-2 rounded-xl bg-indigo-600 hover:bg-indigo-500 text-white text-xs font-semibold shadow-lg shadow-indigo-600/30 transition-all cursor-pointer active:scale-95"
                >
                  <Plus className="w-4 h-4" />
                  <span>Add New Task</span>
                </button>
              </div>
            ) : (
              <div className="space-y-3">
                {filteredTasks.map((task) => (
                  <TaskCard
                    key={task.id}
                    task={task}
                    todayDateStr={todayDateStr}
                    onToggle={handleToggleTask}
                    onEdit={(t) => {
                      setEditingTask(t);
                      setIsEditTaskOpen(true);
                    }}
                    onDelete={(id, title) => {
                      setDeletingTaskId(id);
                      setDeletingTaskTitle(title);
                      setIsDeleteOpen(true);
                    }}
                  />
                ))}
              </div>
            )}
          </div>
        ) : (
          <TaskHistory
            historyTasks={history}
            onRestore={handleRestoreFromHistory}
            onDelete={handleDeleteFromHistory}
            onClearAll={handleClearAllHistory}
          />
        )}
      </main>

      {/* Footer / Status Bar */}
      <StatusBar stats={statistics} lastSavedTime={lastSavedTime} />

      {/* Modals */}
      <AboutModal isOpen={isAboutOpen} onClose={() => setIsAboutOpen(false)} />

      <AddTaskModal
        isOpen={isAddTaskOpen}
        onClose={() => setIsAddTaskOpen(false)}
        onAdd={handleAddTask}
      />

      <EditTaskModal
        isOpen={isEditTaskOpen}
        task={editingTask}
        onClose={() => {
          setIsEditTaskOpen(false);
          setEditingTask(null);
        }}
        onSave={handleEditTask}
      />

      <DeleteModal
        isOpen={isDeleteOpen}
        taskId={deletingTaskId}
        taskTitle={deletingTaskTitle}
        onClose={() => {
          setIsDeleteOpen(false);
          setDeletingTaskId(null);
          setDeletingTaskTitle('');
        }}
        onConfirm={handleDeleteTask}
      />

      <StatsModal
        isOpen={isStatsOpen}
        stats={statistics}
        onClose={() => setIsStatsOpen(false)}
      />

      <ExportModal
        isOpen={isExportOpen}
        tasks={tasks}
        history={history}
        onClose={() => setIsExportOpen(false)}
        onImport={handleImportData}
      />
    </div>
  );
};
