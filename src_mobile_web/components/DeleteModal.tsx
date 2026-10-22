import React from 'react';
import { X, Trash2, AlertTriangle } from 'lucide-react';

interface DeleteModalProps {
  isOpen: boolean;
  taskId: number | null;
  taskTitle: string;
  onClose: () => void;
  onConfirm: (id: number) => void;
}

export const DeleteModal: React.FC<DeleteModalProps> = ({
  isOpen,
  taskId,
  taskTitle,
  onClose,
  onConfirm,
}) => {
  if (!isOpen || taskId === null) return null;

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black/80 backdrop-blur-sm animate-in fade-in duration-200">
      <div 
        className="w-full max-w-sm overflow-hidden rounded-2xl border border-red-500/30 bg-[#0d111a] shadow-2xl text-slate-100 flex flex-col"
        onClick={(e) => e.stopPropagation()}
      >
        {/* Header */}
        <div className="flex items-center justify-between px-6 py-4 border-b border-slate-800 bg-red-950/30">
          <div className="flex items-center gap-2 text-red-400 font-bold text-sm">
            <AlertTriangle className="w-5 h-5 text-red-400" />
            <span>Confirm Deletion</span>
          </div>
          <button
            onClick={onClose}
            className="p-1 rounded-lg text-slate-400 hover:text-white hover:bg-slate-800"
          >
            <X className="w-5 h-5" />
          </button>
        </div>

        {/* Content */}
        <div className="p-6 space-y-3 text-xs sm:text-sm">
          <p className="text-slate-300">
            Are you sure you want to delete this task?
          </p>
          <div className="p-3 rounded-xl bg-slate-950 border border-slate-800 text-amber-300 font-medium break-words">
            "{taskTitle}"
          </div>
          <p className="text-slate-500 text-[11px]">
            This action cannot be undone. To archive instead, complete the task and clear it.
          </p>
        </div>

        {/* Actions */}
        <div className="flex items-center justify-end gap-2.5 px-6 py-3.5 border-t border-slate-800 bg-slate-900/60">
          <button
            onClick={onClose}
            className="px-4 py-2 rounded-xl bg-slate-800 hover:bg-slate-700 text-slate-300 font-medium text-xs cursor-pointer"
          >
            Cancel
          </button>
          <button
            onClick={() => onConfirm(taskId)}
            className="flex items-center gap-1.5 px-4 py-2 rounded-xl bg-red-600 hover:bg-red-500 text-white font-semibold text-xs shadow-lg shadow-red-600/30 transition-all cursor-pointer active:scale-95"
          >
            <Trash2 className="w-3.5 h-3.5" />
            <span>Delete</span>
          </button>
        </div>
      </div>
    </div>
  );
};
