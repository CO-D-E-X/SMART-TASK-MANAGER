import React, { useState } from 'react';
import { Check, Edit3, Trash2, Calendar, AlertTriangle, HelpCircle, Bell } from 'lucide-react';
import { Task } from '../types/task';
import { getUrgencyScore, getDaysRemaining, isTaskOverdue, getPriorityWeight } from '../utils/urgency';
import { formatReminderDisplay } from '../utils/notificationService';

interface TaskCardProps {
  task: Task;
  todayDateStr: string;
  onToggle: (id: number) => void;
  onEdit: (task: Task) => void;
  onDelete: (id: number, title: string) => void;
}

export const TaskCard: React.FC<TaskCardProps> = ({
  task,
  todayDateStr,
  onToggle,
  onEdit,
  onDelete,
}) => {
  const [showFormula, setShowFormula] = useState(false);
  const isCompleted = task.status === 'Completed';
  const overdue = isTaskOverdue(task, todayDateStr);
  const daysRemaining = getDaysRemaining(task, todayDateStr);
  const urgencyScore = getUrgencyScore(task, todayDateStr);
  const weight = getPriorityWeight(task.priority);

  // Priority Badge Styling
  const getPriorityStyle = () => {
    switch (task.priority) {
      case 'High':
        return 'bg-red-950/60 text-red-400 border-red-500/40';
      case 'Medium':
        return 'bg-amber-950/60 text-amber-400 border-amber-500/40';
      case 'Low':
        return 'bg-emerald-950/60 text-emerald-400 border-emerald-500/40';
    }
  };

  // Urgency Score Badge Styling
  const getUrgencyBadgeStyle = () => {
    if (isCompleted) {
      return 'bg-slate-800 text-slate-400 border-slate-700';
    }
    if (overdue) {
      return 'bg-red-500/20 text-red-300 border-red-500/50 animate-pulse';
    }
    if (daysRemaining === 0) {
      return 'bg-amber-500/20 text-amber-300 border-amber-500/50';
    }
    if (urgencyScore > 20) {
      return 'bg-indigo-500/20 text-indigo-300 border-indigo-500/50';
    }
    return 'bg-slate-800 text-slate-300 border-slate-700';
  };

  // Deadline Text
  const getDeadlineText = () => {
    if (isCompleted) return `Completed on ${task.completed_at || 'earlier'}`;
    if (daysRemaining < 0) return `Overdue by ${Math.abs(daysRemaining)} day${Math.abs(daysRemaining) === 1 ? '' : 's'}`;
    if (daysRemaining === 0) return 'Due Today!';
    if (daysRemaining === 1) return 'Due Tomorrow';
    return `Due in ${daysRemaining} days`;
  };

  return (
    <div
      className={`group relative rounded-2xl border transition-all duration-200 p-4 ${
        isCompleted
          ? 'bg-slate-900/40 border-slate-800/60 opacity-60'
          : overdue
          ? 'bg-gradient-to-r from-red-950/20 via-slate-900/90 to-slate-900/90 border-red-900/40 hover:border-red-500/50 shadow-lg shadow-red-950/20'
          : 'bg-slate-900/80 border-slate-800/90 hover:border-indigo-500/40 hover:shadow-lg hover:shadow-indigo-950/20'
      }`}
    >
      <div className="flex items-start gap-3.5">
        {/* Completion Checkbox */}
        <button
          onClick={() => onToggle(task.id)}
          className={`mt-1 w-6 h-6 rounded-lg flex items-center justify-center transition-all cursor-pointer border ${
            isCompleted
              ? 'bg-emerald-600 border-emerald-500 text-white'
              : 'bg-slate-950 border-slate-700 hover:border-indigo-400 text-transparent'
          }`}
          aria-label={isCompleted ? 'Mark as incomplete' : 'Mark as complete'}
        >
          <Check className={`w-4 h-4 stroke-[3] ${isCompleted ? 'block' : 'hidden'}`} />
        </button>

        {/* Task Details */}
        <div className="flex-1 min-w-0">
          <div className="flex flex-wrap items-center gap-2 mb-1.5">
            {/* Title */}
            <h3
              className={`text-base font-semibold tracking-wide break-words transition-all ${
                isCompleted
                  ? 'line-through text-slate-500'
                  : 'text-slate-100 group-hover:text-indigo-200'
              }`}
            >
              {task.title}
            </h3>

            {/* Badges Container */}
            <div className="flex items-center gap-1.5 flex-wrap ml-auto">
              {/* Priority Pill */}
              <span className={`text-[11px] font-semibold px-2 py-0.5 rounded-full border ${getPriorityStyle()}`}>
                {task.priority}
              </span>

              {/* Reminder Alert Badge */}
              {task.reminder?.enabled && task.reminder.time && !isCompleted && (
                <span
                  className="text-[11px] font-medium px-2 py-0.5 rounded-full border bg-indigo-950/70 border-indigo-500/40 text-indigo-300 flex items-center gap-1 shadow-sm"
                  title={`Mobile reminder set for: ${formatReminderDisplay(task.reminder.time)}`}
                >
                  <Bell className="w-3 h-3 text-indigo-400" />
                  <span>{formatReminderDisplay(task.reminder.time)}</span>
                </span>
              )}

              {/* Urgency Badge */}
              <button
                onClick={() => setShowFormula(!showFormula)}
                className={`text-[11px] font-mono font-bold px-2 py-0.5 rounded-full border flex items-center gap-1 transition-all cursor-pointer ${getUrgencyBadgeStyle()}`}
                title="Click to view urgency calculation breakdown"
              >
                <span>Urgency: {urgencyScore.toFixed(1)}</span>
                <HelpCircle className="w-3 h-3 opacity-70" />
              </button>
            </div>
          </div>

          {/* Description */}
          {task.description && (
            <p
              className={`text-xs sm:text-sm mb-3 leading-relaxed break-words ${
                isCompleted ? 'text-slate-600 line-through' : 'text-slate-400'
              }`}
            >
              {task.description}
            </p>
          )}

          {/* Metadata & Actions Bar */}
          <div className="flex flex-wrap items-center justify-between gap-2 pt-2 border-t border-slate-800/60 text-xs">
            {/* Deadline Status */}
            <div
              className={`flex items-center gap-1.5 font-medium ${
                isCompleted
                  ? 'text-slate-500'
                  : overdue
                  ? 'text-red-400'
                  : daysRemaining === 0
                  ? 'text-amber-400'
                  : 'text-slate-400'
              }`}
            >
              {overdue ? (
                <AlertTriangle className="w-3.5 h-3.5 shrink-0" />
              ) : (
                <Calendar className="w-3.5 h-3.5 shrink-0 text-slate-500" />
              )}
              <span>{task.deadline}</span>
              <span>•</span>
              <span className="font-semibold">{getDeadlineText()}</span>
            </div>

            {/* Actions: Edit & Delete */}
            <div className="flex items-center gap-1 ml-auto">
              <button
                onClick={() => onEdit(task)}
                className="p-1.5 rounded-lg text-slate-400 hover:text-indigo-300 hover:bg-slate-800 transition-colors cursor-pointer"
                title="Edit Task"
              >
                <Edit3 className="w-4 h-4" />
              </button>
              <button
                onClick={() => onDelete(task.id, task.title)}
                className="p-1.5 rounded-lg text-slate-400 hover:text-red-400 hover:bg-slate-800 transition-colors cursor-pointer"
                title="Delete Task"
              >
                <Trash2 className="w-4 h-4" />
              </button>
            </div>
          </div>

          {/* Urgency Formula Explanation Accordion */}
          {showFormula && (
            <div className="mt-3 p-3 rounded-xl bg-slate-950/90 border border-indigo-500/30 text-[11px] text-slate-300 font-mono animate-in fade-in duration-150">
              <div className="font-bold text-indigo-400 mb-1 flex items-center justify-between">
                <span>DYNAMIC URGENCY BREAKDOWN:</span>
                <span className="text-[10px] text-slate-500">Task #{task.id}</span>
              </div>
              <div className="space-y-0.5">
                <div>Priority: <strong className="text-white">{task.priority}</strong> (Weight W = {weight.toFixed(1)})</div>
                <div>Deadline: <strong className="text-white">{task.deadline}</strong> ({daysRemaining} days remaining)</div>
                {isCompleted ? (
                  <div className="text-slate-400">Score = 0.0 (Completed tasks sink to bottom)</div>
                ) : overdue ? (
                  <div className="text-red-300">
                    Formula: 100 + (W &times; 15) + min(|d|, 30) &times; 2.5<br />
                    = 100 + ({weight} &times; 15) + {Math.min(Math.abs(daysRemaining), 30)} &times; 2.5 = <strong className="text-white">{urgencyScore.toFixed(2)}</strong>
                  </div>
                ) : daysRemaining === 0 ? (
                  <div className="text-amber-300">
                    Formula: 80 + (W &times; 12)<br />
                    = 80 + ({weight} &times; 12) = <strong className="text-white">{urgencyScore.toFixed(2)}</strong>
                  </div>
                ) : (
                  <div className="text-indigo-300">
                    Formula: (W &times; 40) / (d + 1) + (W &times; 3)<br />
                    = ({weight} &times; 40) / ({daysRemaining} + 1) + ({weight} &times; 3) = <strong className="text-white">{urgencyScore.toFixed(2)}</strong>
                  </div>
                )}
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
};
