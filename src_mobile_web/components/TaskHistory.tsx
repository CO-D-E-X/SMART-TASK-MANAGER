import React, { useState } from 'react';
import { RotateCcw, Trash2, Search, X, Archive, AlertCircle } from 'lucide-react';
import { Task } from '../types/task';

interface TaskHistoryProps {
  historyTasks: Task[];
  onRestore: (id: number) => void;
  onDelete: (id: number) => void;
  onClearAll: () => void;
}

export const TaskHistory: React.FC<TaskHistoryProps> = ({
  historyTasks,
  onRestore,
  onDelete,
  onClearAll,
}) => {
  const [searchQuery, setSearchQuery] = useState('');

  const filteredHistory = historyTasks.filter((t) => {
    if (!searchQuery.trim()) return true;
    const q = searchQuery.toLowerCase();
    return t.title.toLowerCase().includes(q) || t.description.toLowerCase().includes(q);
  });

  return (
    <div className="space-y-4">
      {/* History Header & Search */}
      <div className="flex flex-col sm:flex-row items-stretch sm:items-center justify-between gap-3 bg-slate-900/60 p-4 rounded-2xl border border-slate-800">
        <div className="flex items-center gap-2.5">
          <Archive className="w-5 h-5 text-indigo-400" />
          <div>
            <h2 className="text-base font-bold text-slate-100">Archived Task Vault</h2>
            <p className="text-xs text-slate-400">Completed and cleared productivity records</p>
          </div>
        </div>

        <div className="flex items-center gap-2">
          {/* Search */}
          <div className="relative flex-1 sm:w-64">
            <Search className="w-3.5 h-3.5 absolute left-3 top-1/2 -translate-y-1/2 text-slate-400 pointer-events-none" />
            <input
              type="text"
              value={searchQuery}
              onChange={(e) => setSearchQuery(e.target.value)}
              placeholder="Search history..."
              className="w-full pl-8 pr-7 py-1.5 rounded-xl bg-slate-950/80 border border-slate-700/80 text-xs text-slate-100 placeholder-slate-500 focus:outline-none focus:border-indigo-500"
            />
            {searchQuery && (
              <button
                onClick={() => setSearchQuery('')}
                className="absolute right-2.5 top-1/2 -translate-y-1/2 text-slate-400 hover:text-white"
              >
                <X className="w-3 h-3" />
              </button>
            )}
          </div>

          {/* Clear All History */}
          {historyTasks.length > 0 && (
            <button
              onClick={onClearAll}
              className="flex items-center gap-1.5 px-3 py-1.5 rounded-xl bg-red-950/40 hover:bg-red-900/50 text-red-300 border border-red-500/40 text-xs font-medium transition-all active:scale-95 cursor-pointer"
              title="Purge all archived tasks permanently"
            >
              <Trash2 className="w-3.5 h-3.5" />
              <span>Purge All</span>
            </button>
          )}
        </div>
      </div>

      {/* History Items List */}
      {filteredHistory.length === 0 ? (
        <div className="text-center py-12 px-4 rounded-2xl bg-slate-900/40 border border-slate-800/60">
          <Archive className="w-12 h-12 text-slate-600 mx-auto mb-3" />
          <h3 className="text-sm font-semibold text-slate-300">
            {searchQuery ? 'No matching archived tasks found' : 'No tasks in history yet'}
          </h3>
          <p className="text-xs text-slate-500 mt-1 max-w-sm mx-auto">
            {searchQuery
              ? 'Try adjusting your search query.'
              : 'Tasks marked as completed and cleared will be securely archived here.'}
          </p>
        </div>
      ) : (
        <div className="space-y-2.5">
          {filteredHistory.map((task) => (
            <div
              key={task.id}
              className="flex items-center justify-between p-3.5 rounded-xl bg-slate-900/50 border border-slate-800 hover:border-slate-700 transition-all text-xs"
            >
              <div className="min-w-0 flex-1 pr-3">
                <div className="flex items-center gap-2 mb-1">
                  <h4 className="font-semibold text-slate-300 line-through truncate">{task.title}</h4>
                  <span className="text-[10px] font-medium px-2 py-0.5 rounded-full bg-slate-800 text-slate-400 border border-slate-700 shrink-0">
                    {task.priority}
                  </span>
                </div>
                {task.description && (
                  <p className="text-slate-500 truncate text-[11px] mb-1">{task.description}</p>
                )}
                <div className="text-slate-500 text-[10px]">
                  <span>Deadline: {task.deadline}</span>
                  {task.completed_at && <span className="ml-2">• Completed: {task.completed_at}</span>}
                </div>
              </div>

              <div className="flex items-center gap-1.5 shrink-0">
                <button
                  onClick={() => onRestore(task.id)}
                  className="flex items-center gap-1 px-2.5 py-1.5 rounded-lg bg-indigo-950/60 hover:bg-indigo-900/80 text-indigo-300 border border-indigo-500/40 text-xs font-medium transition-all active:scale-95 cursor-pointer"
                  title="Restore task to active list"
                >
                  <RotateCcw className="w-3.5 h-3.5" />
                  <span>Restore</span>
                </button>
                <button
                  onClick={() => onDelete(task.id)}
                  className="p-1.5 rounded-lg text-slate-400 hover:text-red-400 hover:bg-slate-800 transition-colors cursor-pointer"
                  title="Delete permanently"
                >
                  <Trash2 className="w-3.5 h-3.5" />
                </button>
              </div>
            </div>
          ))}
        </div>
      )}
    </div>
  );
};
