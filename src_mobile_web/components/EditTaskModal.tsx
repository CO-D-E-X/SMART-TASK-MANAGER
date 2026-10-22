import React, { useState, useEffect } from 'react';
import { X, Edit3, AlertCircle, Bell } from 'lucide-react';
import { Task, Priority, TaskReminder } from '../types/task';
import { getCurrentDateString, addDaysToDate } from '../utils/urgency';

interface EditTaskModalProps {
  isOpen: boolean;
  task: Task | null;
  onClose: () => void;
  onSave: (id: number, title: string, description: string, priority: Priority, deadline: string, reminder?: TaskReminder) => void;
}

export const EditTaskModal: React.FC<EditTaskModalProps> = ({ isOpen, task, onClose, onSave }) => {
  const today = getCurrentDateString();
  const [title, setTitle] = useState('');
  const [description, setDescription] = useState('');
  const [priority, setPriority] = useState<Priority>('Medium');
  const [deadline, setDeadline] = useState(today);

  // Reminder State
  const [reminderEnabled, setReminderEnabled] = useState(false);
  const [reminderTime, setReminderTime] = useState(`${today}T09:00`);
  const [error, setError] = useState('');

  useEffect(() => {
    if (task) {
      setTitle(task.title);
      setDescription(task.description);
      setPriority(task.priority);
      setDeadline(task.deadline);
      setReminderEnabled(task.reminder?.enabled ?? false);
      setReminderTime(task.reminder?.time ?? `${task.deadline}T09:00`);
      setError('');
    }
  }, [task]);

  if (!isOpen || !task) return null;

  const handleSetQuickReminder = (type: 'deadline' | '15m' | '1h' | 'tomorrow') => {
    const now = new Date();
    if (type === 'deadline') {
      setReminderTime(`${deadline}T09:00`);
    } else if (type === '15m') {
      const target = new Date(now.getTime() + 15 * 60 * 1000);
      setReminderTime(target.toISOString().slice(0, 16));
    } else if (type === '1h') {
      const target = new Date(now.getTime() + 60 * 60 * 1000);
      setReminderTime(target.toISOString().slice(0, 16));
    } else if (type === 'tomorrow') {
      const tomorrow = addDaysToDate(today, 1);
      setReminderTime(`${tomorrow}T09:00`);
    }
    setReminderEnabled(true);
  };

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (!title.trim()) {
      setError('Please enter a task title.');
      return;
    }
    if (!deadline) {
      setError('Please specify a valid deadline date.');
      return;
    }

    const reminder: TaskReminder | undefined = reminderEnabled && reminderTime ? {
      enabled: true,
      time: reminderTime,
    } : undefined;

    onSave(task.id, title.trim(), description.trim(), priority, deadline, reminder);
    onClose();
  };

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black/80 backdrop-blur-sm animate-in fade-in duration-200">
      <div 
        className="w-full max-w-md overflow-hidden rounded-2xl border border-slate-800 bg-[#0d111a] shadow-2xl text-slate-100 flex flex-col max-h-[92vh]"
        onClick={(e) => e.stopPropagation()}
      >
        {/* Header */}
        <div className="flex items-center justify-between px-6 py-4 border-b border-slate-800 bg-slate-900/60">
          <div className="flex items-center gap-2">
            <Edit3 className="w-5 h-5 text-indigo-400" />
            <h2 className="text-base font-bold text-white">Edit Task #{task.id}</h2>
          </div>
          <button
            onClick={onClose}
            className="p-1 rounded-lg text-slate-400 hover:text-white hover:bg-slate-800"
          >
            <X className="w-5 h-5" />
          </button>
        </div>

        {/* Form Body */}
        <form onSubmit={handleSubmit} className="p-6 space-y-4 text-xs sm:text-sm overflow-y-auto custom-scrollbar">
          {error && (
            <div className="flex items-center gap-2 p-2.5 rounded-xl bg-red-950/50 border border-red-500/40 text-red-300 text-xs">
              <AlertCircle className="w-4 h-4 shrink-0" />
              <span>{error}</span>
            </div>
          )}

          {/* Title */}
          <div>
            <label className="block text-xs font-semibold text-slate-300 mb-1.5 uppercase tracking-wider">
              Task Title <span className="text-red-400">*</span>
            </label>
            <input
              type="text"
              value={title}
              onChange={(e) => {
                setTitle(e.target.value);
                if (error) setError('');
              }}
              className="w-full px-3.5 py-2.5 rounded-xl bg-slate-950 border border-slate-700 text-slate-100 focus:outline-none focus:border-indigo-500 focus:ring-1 focus:ring-indigo-500"
            />
          </div>

          {/* Description */}
          <div>
            <label className="block text-xs font-semibold text-slate-300 mb-1.5 uppercase tracking-wider">
              Description / Notes
            </label>
            <textarea
              value={description}
              onChange={(e) => setDescription(e.target.value)}
              rows={2}
              className="w-full px-3.5 py-2 rounded-xl bg-slate-950 border border-slate-700 text-slate-100 focus:outline-none focus:border-indigo-500 focus:ring-1 focus:ring-indigo-500 resize-none"
            />
          </div>

          {/* Priority */}
          <div>
            <label className="block text-xs font-semibold text-slate-300 mb-1.5 uppercase tracking-wider">
              Priority Level
            </label>
            <div className="grid grid-cols-3 gap-2">
              {(['Low', 'Medium', 'High'] as Priority[]).map((p) => (
                <button
                  type="button"
                  key={p}
                  onClick={() => setPriority(p)}
                  className={`py-2 px-3 rounded-xl border text-xs font-bold transition-all cursor-pointer ${
                    priority === p
                      ? p === 'High'
                        ? 'bg-red-500/20 text-red-300 border-red-500 shadow-sm'
                        : p === 'Medium'
                        ? 'bg-amber-500/20 text-amber-300 border-amber-500 shadow-sm'
                        : 'bg-emerald-500/20 text-emerald-300 border-emerald-500 shadow-sm'
                      : 'bg-slate-950 border-slate-800 text-slate-400 hover:border-slate-700'
                  }`}
                >
                  {p}
                </button>
              ))}
            </div>
          </div>

          {/* Deadline */}
          <div>
            <label className="block text-xs font-semibold text-slate-300 mb-1.5 uppercase tracking-wider">
              Target Deadline
            </label>
            <div className="flex items-center gap-2 mb-2">
              <input
                type="date"
                value={deadline}
                onChange={(e) => setDeadline(e.target.value)}
                className="w-full px-3.5 py-2 rounded-xl bg-slate-950 border border-slate-700 text-slate-100 focus:outline-none focus:border-indigo-500 [color-scheme:dark]"
              />
            </div>

            {/* Quick date buttons */}
            <div className="flex items-center gap-1.5 text-xs text-slate-400">
              <span className="text-[11px] text-slate-500">Quick:</span>
              <button
                type="button"
                onClick={() => setDeadline(today)}
                className="px-2 py-1 rounded-lg bg-slate-800 hover:bg-slate-700 text-slate-300 text-[11px] cursor-pointer"
              >
                Today
              </button>
              <button
                type="button"
                onClick={() => setDeadline(addDaysToDate(today, 1))}
                className="px-2 py-1 rounded-lg bg-slate-800 hover:bg-slate-700 text-slate-300 text-[11px] cursor-pointer"
              >
                +1d
              </button>
              <button
                type="button"
                onClick={() => setDeadline(addDaysToDate(today, 3))}
                className="px-2 py-1 rounded-lg bg-slate-800 hover:bg-slate-700 text-slate-300 text-[11px] cursor-pointer"
              >
                +3d
              </button>
              <button
                type="button"
                onClick={() => setDeadline(addDaysToDate(today, 7))}
                className="px-2 py-1 rounded-lg bg-slate-800 hover:bg-slate-700 text-slate-300 text-[11px] cursor-pointer"
              >
                +1w
              </button>
            </div>
          </div>

          {/* Reminder Alert Section */}
          <div className="p-3.5 rounded-xl bg-slate-950 border border-indigo-500/30 space-y-2.5">
            <div className="flex items-center justify-between">
              <label className="flex items-center gap-2 cursor-pointer select-none">
                <input
                  type="checkbox"
                  checked={reminderEnabled}
                  onChange={(e) => setReminderEnabled(e.target.checked)}
                  className="w-4 h-4 rounded text-indigo-600 bg-slate-900 border-slate-700 focus:ring-indigo-500"
                />
                <span className="text-xs font-bold text-indigo-300 flex items-center gap-1.5">
                  <Bell className="w-3.5 h-3.5 text-indigo-400" />
                  Schedule Mobile Reminder Alert
                </span>
              </label>
              <span className="text-[10px] text-emerald-400 font-medium">100% Local Alarm</span>
            </div>

            {reminderEnabled && (
              <div className="space-y-2 pt-1 animate-in fade-in duration-150">
                <input
                  type="datetime-local"
                  value={reminderTime}
                  onChange={(e) => setReminderTime(e.target.value)}
                  className="w-full px-3 py-2 rounded-xl bg-slate-900 border border-slate-700 text-xs text-slate-100 focus:outline-none focus:border-indigo-500 [color-scheme:dark]"
                />

                {/* Quick Reminder Shortcuts */}
                <div className="flex flex-wrap gap-1.5 text-[10px]">
                  <span className="text-slate-500 self-center">Presets:</span>
                  <button
                    type="button"
                    onClick={() => handleSetQuickReminder('15m')}
                    className="px-2 py-0.5 rounded bg-indigo-950/80 hover:bg-indigo-900 text-indigo-300 border border-indigo-500/30 cursor-pointer"
                  >
                    +15m (Test)
                  </button>
                  <button
                    type="button"
                    onClick={() => handleSetQuickReminder('1h')}
                    className="px-2 py-0.5 rounded bg-slate-800 hover:bg-slate-700 text-slate-300 cursor-pointer"
                  >
                    +1 hour
                  </button>
                  <button
                    type="button"
                    onClick={() => handleSetQuickReminder('deadline')}
                    className="px-2 py-0.5 rounded bg-slate-800 hover:bg-slate-700 text-slate-300 cursor-pointer"
                  >
                    Deadline 9:00 AM
                  </button>
                  <button
                    type="button"
                    onClick={() => handleSetQuickReminder('tomorrow')}
                    className="px-2 py-0.5 rounded bg-slate-800 hover:bg-slate-700 text-slate-300 cursor-pointer"
                  >
                    Tomorrow 9:00 AM
                  </button>
                </div>
              </div>
            )}
          </div>

          {/* Actions */}
          <div className="flex items-center justify-end gap-2.5 pt-3 border-t border-slate-800">
            <button
              type="button"
              onClick={onClose}
              className="px-4 py-2 rounded-xl bg-slate-800 hover:bg-slate-700 text-slate-300 font-medium text-xs cursor-pointer"
            >
              Cancel
            </button>
            <button
              type="submit"
              className="px-5 py-2 rounded-xl bg-indigo-600 hover:bg-indigo-500 text-white font-semibold text-xs shadow-lg shadow-indigo-600/30 transition-all cursor-pointer active:scale-95"
            >
              Save Changes
            </button>
          </div>
        </form>
      </div>
    </div>
  );
};
