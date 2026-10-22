import React from 'react';
import { ShieldCheck, HardDrive, CheckCircle2 } from 'lucide-react';
import { TaskStatistics } from '../types/task';

interface StatusBarProps {
  stats: TaskStatistics;
  lastSavedTime: string;
}

export const StatusBar: React.FC<StatusBarProps> = ({ stats, lastSavedTime }) => {
  const pct = Math.round(stats.completionRate * 100);

  return (
    <footer className="sticky bottom-0 z-30 bg-[#090d14]/95 backdrop-blur-md border-t border-slate-800/80 px-4 py-2.5 sm:px-6 text-xs text-slate-400">
      <div className="max-w-7xl mx-auto flex flex-col sm:flex-row items-center justify-between gap-2">
        {/* Left: Summary Metrics */}
        <div className="flex items-center gap-3 sm:gap-4 flex-wrap text-[11px] sm:text-xs">
          <span>
            Total: <strong className="text-white">{stats.totalTasks}</strong>
          </span>
          <span className="text-slate-600">&bull;</span>
          <span className="text-emerald-400">
            Done: <strong className="font-semibold">{stats.completedTasks}</strong> ({pct}%)
          </span>
          <span className="text-slate-600">&bull;</span>
          <span className="text-indigo-400">
            Pending: <strong className="font-semibold">{stats.pendingTasks}</strong>
          </span>
          {stats.overdueTasks > 0 && (
            <>
              <span className="text-slate-600">&bull;</span>
              <span className="text-red-400 animate-pulse">
                Overdue: <strong className="font-semibold">{stats.overdueTasks}</strong>
              </span>
            </>
          )}
        </div>

        {/* Right: Security & Save Status */}
        <div className="flex items-center gap-3 text-[11px]">
          <div className="flex items-center gap-1.5 text-emerald-400 bg-emerald-950/40 px-2 py-0.5 rounded-full border border-emerald-500/30">
            <ShieldCheck className="w-3.5 h-3.5" />
            <span>100% Local Storage</span>
          </div>

          <div className="flex items-center gap-1 text-slate-400 font-mono">
            <CheckCircle2 className="w-3 h-3 text-emerald-500" />
            <span>{lastSavedTime}</span>
          </div>
        </div>
      </div>
    </footer>
  );
};
