import React from 'react';
import { Plus, BarChart2, Download, Trash2, Shield, Info } from 'lucide-react';

interface HeaderProps {
  todayDateStr: string;
  completedCount: number;
  onOpenAbout: () => void;
  onOpenAddTask: () => void;
  onOpenStats: () => void;
  onOpenExport: () => void;
  onClearCompleted: () => void;
}

export const Header: React.FC<HeaderProps> = ({
  todayDateStr,
  completedCount,
  onOpenAbout,
  onOpenAddTask,
  onOpenStats,
  onOpenExport,
  onClearCompleted,
}) => {
  return (
    <header className="sticky top-0 z-30 bg-[#0c1017]/95 backdrop-blur-md border-b border-slate-800/80 px-4 pt-safe sm:px-6 transition-all">
      <div className="max-w-7xl mx-auto pt-2 pb-3 flex flex-col md:flex-row md:items-center md:justify-between gap-3">
        {/* App Logo & Title (Clickable to open About modal) */}
        <div 
          onClick={onOpenAbout}
          className="group flex items-center space-x-3 cursor-pointer select-none py-1 px-2 -mx-2 rounded-xl transition-all hover:bg-slate-800/60 active:scale-[0.98]"
          title="Click to view About Developer: ___CODEX & SHERALOCK & Assistant"
        >
          <div className="relative">
            <div className="w-10 h-10 rounded-xl overflow-hidden border border-indigo-500/40 bg-slate-900 p-1 flex items-center justify-center shadow-lg shadow-indigo-600/20 group-hover:border-indigo-400 group-hover:shadow-indigo-500/40 transition-all">
              <img src="/task_icon.png" alt="TASK MANAGER Logo" className="w-full h-full object-contain" />
            </div>
            <span className="absolute -top-1 -right-1 flex h-3 w-3">
              <span className="animate-ping absolute inline-flex h-full w-full rounded-full bg-emerald-400 opacity-75"></span>
              <span className="relative inline-flex rounded-full h-3 w-3 bg-emerald-500 border border-slate-900"></span>
            </span>
          </div>

          <div>
            <div className="flex items-center gap-2">
              <h1 className="text-lg sm:text-xl font-extrabold tracking-wider text-transparent bg-clip-text bg-gradient-to-r from-indigo-300 via-indigo-100 to-white group-hover:from-indigo-200 group-hover:to-white">
                TASK MANAGER
              </h1>
              <span className="flex items-center text-[10px] font-semibold uppercase px-2 py-0.5 rounded-full bg-emerald-950/80 text-emerald-400 border border-emerald-500/40">
                <Shield className="w-2.5 h-2.5 mr-1" />
                100% Local
              </span>
            </div>
            <div className="flex items-center gap-2 text-xs text-slate-400">
              <span className="text-slate-400">Dynamic Task Prioritization</span>
              <span>•</span>
              <span className="text-indigo-300 font-mono text-[11px]">Today: {todayDateStr}</span>
              <Info className="w-3 h-3 text-indigo-400 group-hover:text-indigo-300 inline opacity-70" />
            </div>
          </div>
        </div>

        {/* Action Buttons Toolbar */}
        <div className="flex items-center flex-wrap gap-2">
          {/* Add Task Button */}
          <button
            onClick={onOpenAddTask}
            className="flex items-center gap-1.5 px-3.5 py-2 rounded-xl bg-indigo-600 hover:bg-indigo-500 active:bg-indigo-700 text-white font-semibold text-xs sm:text-sm shadow-lg shadow-indigo-600/30 transition-all active:scale-95 cursor-pointer"
          >
            <Plus className="w-4 h-4" />
            <span>Add Task</span>
          </button>

          {/* Stats Button */}
          <button
            onClick={onOpenStats}
            className="flex items-center gap-1.5 px-3 py-2 rounded-xl bg-slate-800/90 hover:bg-slate-700/90 active:bg-slate-800 text-slate-200 font-medium text-xs sm:text-sm border border-slate-700/80 shadow transition-all active:scale-95 cursor-pointer"
            title="Productivity Insights & Urgency Analytics"
          >
            <BarChart2 className="w-4 h-4 text-indigo-400" />
            <span className="hidden sm:inline">Stats</span>
          </button>

          {/* Backup / Export Button */}
          <button
            onClick={onOpenExport}
            className="flex items-center gap-1.5 px-3 py-2 rounded-xl bg-slate-800/90 hover:bg-slate-700/90 active:bg-slate-800 text-slate-200 font-medium text-xs sm:text-sm border border-slate-700/80 shadow transition-all active:scale-95 cursor-pointer"
            title="Local Backup & CSV Export"
          >
            <Download className="w-4 h-4 text-emerald-400" />
            <span className="hidden sm:inline">Backup</span>
          </button>

          {/* Clear Completed Button */}
          <button
            onClick={onClearCompleted}
            disabled={completedCount === 0}
            className={`flex items-center gap-1.5 px-3 py-2 rounded-xl text-xs sm:text-sm font-medium border transition-all active:scale-95 ${
              completedCount > 0
                ? 'bg-amber-950/40 hover:bg-amber-900/50 text-amber-200 border-amber-500/40 cursor-pointer'
                : 'bg-slate-900/40 text-slate-500 border-slate-800 cursor-not-allowed opacity-50'
            }`}
            title="Archive completed tasks into History Vault"
          >
            <Trash2 className="w-4 h-4 text-amber-400" />
            <span>Clear Done ({completedCount})</span>
          </button>
        </div>
      </div>
    </header>
  );
};
