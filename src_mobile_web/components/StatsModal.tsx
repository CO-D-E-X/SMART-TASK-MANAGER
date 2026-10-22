import React from 'react';
import { X, BarChart2, CheckCircle, Clock, AlertTriangle, Zap, Percent, Activity } from 'lucide-react';
import { TaskStatistics } from '../types/task';

interface StatsModalProps {
  isOpen: boolean;
  stats: TaskStatistics;
  onClose: () => void;
}

export const StatsModal: React.FC<StatsModalProps> = ({ isOpen, stats, onClose }) => {
  if (!isOpen) return null;

  const pct = Math.round(stats.completionRate * 100);

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black/80 backdrop-blur-sm animate-in fade-in duration-200">
      <div 
        className="w-full max-w-lg overflow-hidden rounded-2xl border border-slate-800 bg-[#0d111a] shadow-2xl text-slate-100 flex flex-col max-h-[90vh]"
        onClick={(e) => e.stopPropagation()}
      >
        {/* Header */}
        <div className="flex items-center justify-between px-6 py-4 border-b border-slate-800 bg-slate-900/60">
          <div className="flex items-center gap-2">
            <BarChart2 className="w-5 h-5 text-indigo-400" />
            <h2 className="text-base font-bold text-white">Productivity Insights &amp; Analytics</h2>
          </div>
          <button
            onClick={onClose}
            className="p-1 rounded-lg text-slate-400 hover:text-white hover:bg-slate-800"
          >
            <X className="w-5 h-5" />
          </button>
        </div>

        {/* Body */}
        <div className="p-6 space-y-5 overflow-y-auto custom-scrollbar text-xs sm:text-sm">
          {/* Progress Bar Section */}
          <div className="p-4 rounded-xl bg-slate-950 border border-slate-800 space-y-2">
            <div className="flex items-center justify-between font-semibold">
              <span className="text-slate-300">Overall Completion Progress</span>
              <span className="text-indigo-400 font-mono font-bold">{pct}%</span>
            </div>
            <div className="h-3 w-full bg-slate-800 rounded-full overflow-hidden">
              <div
                className="h-full bg-gradient-to-r from-indigo-500 to-emerald-500 transition-all duration-500 rounded-full"
                style={{ width: `${pct}%` }}
              />
            </div>
            <div className="flex justify-between text-[11px] text-slate-400">
              <span>{stats.completedTasks} completed</span>
              <span>{stats.totalTasks} total tasks</span>
            </div>
          </div>

          {/* Metrics Grid */}
          <div className="grid grid-cols-2 sm:grid-cols-4 gap-2.5">
            <div className="p-3 rounded-xl bg-slate-900 border border-slate-800 text-center">
              <span className="text-[11px] text-slate-400">Total</span>
              <div className="text-lg font-bold text-white mt-0.5">{stats.totalTasks}</div>
            </div>
            <div className="p-3 rounded-xl bg-emerald-950/30 border border-emerald-500/30 text-center">
              <span className="text-[11px] text-emerald-400">Completed</span>
              <div className="text-lg font-bold text-emerald-300 mt-0.5">{stats.completedTasks}</div>
            </div>
            <div className="p-3 rounded-xl bg-indigo-950/30 border border-indigo-500/30 text-center">
              <span className="text-[11px] text-indigo-400">Pending</span>
              <div className="text-lg font-bold text-indigo-300 mt-0.5">{stats.pendingTasks}</div>
            </div>
            <div className="p-3 rounded-xl bg-red-950/30 border border-red-500/30 text-center">
              <span className="text-[11px] text-red-400">Overdue</span>
              <div className="text-lg font-bold text-red-300 mt-0.5">{stats.overdueTasks}</div>
            </div>
          </div>

          {/* Pending Priorities Breakdown */}
          <div className="p-4 rounded-xl bg-slate-950 border border-slate-800 space-y-2.5">
            <div className="text-xs font-semibold text-slate-300 uppercase tracking-wider">
              Pending Priorities Breakdown
            </div>
            <div className="space-y-2 text-xs">
              <div className="flex items-center justify-between">
                <span className="flex items-center gap-2 text-red-400 font-medium">
                  <span className="w-2 h-2 rounded-full bg-red-500" />
                  High Priority
                </span>
                <span className="font-mono font-bold text-red-300">{stats.highPriorityPending}</span>
              </div>
              <div className="flex items-center justify-between">
                <span className="flex items-center gap-2 text-amber-400 font-medium">
                  <span className="w-2 h-2 rounded-full bg-amber-500" />
                  Medium Priority
                </span>
                <span className="font-mono font-bold text-amber-300">{stats.mediumPriorityPending}</span>
              </div>
              <div className="flex items-center justify-between">
                <span className="flex items-center gap-2 text-emerald-400 font-medium">
                  <span className="w-2 h-2 rounded-full bg-emerald-500" />
                  Low Priority
                </span>
                <span className="font-mono font-bold text-emerald-300">{stats.lowPriorityPending}</span>
              </div>
              <div className="pt-2 border-t border-slate-800 flex items-center justify-between">
                <span className="text-slate-400 flex items-center gap-1.5">
                  <Activity className="w-3.5 h-3.5 text-indigo-400" />
                  Average Urgency (Pending):
                </span>
                <span className="font-mono font-bold text-indigo-300">{stats.averageUrgency.toFixed(2)}</span>
              </div>
            </div>
          </div>

          {/* Mathematical Urgency Engine Reference */}
          <div className="p-4 rounded-xl bg-indigo-950/20 border border-indigo-500/30 space-y-2 text-xs">
            <div className="flex items-center gap-1.5 text-indigo-400 font-semibold tracking-wider uppercase text-[11px]">
              <Zap className="w-3.5 h-3.5" />
              Dynamic Urgency Formula Engine
            </div>
            <ul className="text-slate-300 space-y-1 font-mono text-[11px]">
              <li>&bull; Overdue (d &lt; 0): 100 + (W &times; 15) + min(|d|, 30) &times; 2.5</li>
              <li>&bull; Due Today (d = 0): 80 + (W &times; 12)</li>
              <li>&bull; Upcoming (d &gt; 0): (W &times; 40) / (d + 1) + (W &times; 3)</li>
              <li>&bull; Weights: High = 5.0, Med = 2.5, Low = 1.0</li>
            </ul>
          </div>
        </div>

        {/* Footer */}
        <div className="flex justify-end px-6 py-3.5 border-t border-slate-800 bg-slate-900/60">
          <button
            onClick={onClose}
            className="px-4 py-2 rounded-xl bg-slate-800 hover:bg-slate-700 text-slate-200 font-medium text-xs cursor-pointer"
          >
            Close
          </button>
        </div>
      </div>
    </div>
  );
};
