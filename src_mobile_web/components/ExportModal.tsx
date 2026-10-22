import React, { useRef, useState } from 'react';
import { X, Download, Upload, FileSpreadsheet, FileText, CheckCircle2, AlertCircle, Trash2 } from 'lucide-react';
import { Task } from '../types/task';
import { exportToCSV, exportToJSON, importFromJSON } from '../utils/storage';

interface ExportModalProps {
  isOpen: boolean;
  tasks: Task[];
  history: Task[];
  onClose: () => void;
  onImport: (tasks: Task[], history: Task[]) => void;
}

export const ExportModal: React.FC<ExportModalProps> = ({
  isOpen,
  tasks,
  history,
  onClose,
  onImport,
}) => {
  const fileInputRef = useRef<HTMLInputElement>(null);
  const [statusMessage, setStatusMessage] = useState<{ type: 'success' | 'error'; text: string } | null>(null);

  if (!isOpen) return null;

  const handleExportCSV = () => {
    exportToCSV(tasks, history);
    setStatusMessage({ type: 'success', text: 'CSV export downloaded successfully.' });
  };

  const handleExportJSON = () => {
    exportToJSON(tasks, history);
    setStatusMessage({ type: 'success', text: 'Complete JSON backup downloaded.' });
  };

  const handleFileChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = (event) => {
      const content = event.target?.result as string;
      const parsed = importFromJSON(content);
      if (parsed) {
        onImport(parsed.tasks, parsed.history);
        setStatusMessage({
          type: 'success',
          text: `Successfully imported ${parsed.tasks.length} active tasks and ${parsed.history.length} archived tasks!`,
        });
      } else {
        setStatusMessage({
          type: 'error',
          text: 'Invalid JSON backup format. Please select a valid Task Manager backup file.',
        });
      }
    };
    reader.onerror = () => {
      setStatusMessage({ type: 'error', text: 'Error reading file.' });
    };
    reader.readAsText(file);
  };

  const handleClearAll = () => {
    if (window.confirm('Are you sure you want to clear all active tasks and history?')) {
      onImport([], []);
      setStatusMessage({ type: 'success', text: 'All tasks and history cleared. Clean slate!' });
    }
  };

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black/80 backdrop-blur-sm animate-in fade-in duration-200">
      <div 
        className="w-full max-w-md overflow-hidden rounded-2xl border border-slate-800 bg-[#0d111a] shadow-2xl text-slate-100 flex flex-col"
        onClick={(e) => e.stopPropagation()}
      >
        {/* Header */}
        <div className="flex items-center justify-between px-6 py-4 border-b border-slate-800 bg-slate-900/60">
          <div className="flex items-center gap-2">
            <Download className="w-5 h-5 text-emerald-400" />
            <h2 className="text-base font-bold text-white">Backup, Export &amp; Restore</h2>
          </div>
          <button
            onClick={onClose}
            className="p-1 rounded-lg text-slate-400 hover:text-white hover:bg-slate-800"
          >
            <X className="w-5 h-5" />
          </button>
        </div>

        {/* Body */}
        <div className="p-6 space-y-4 text-xs sm:text-sm">
          {statusMessage && (
            <div
              className={`flex items-center gap-2 p-3 rounded-xl border text-xs ${
                statusMessage.type === 'success'
                  ? 'bg-emerald-950/40 border-emerald-500/40 text-emerald-300'
                  : 'bg-red-950/40 border-red-500/40 text-red-300'
              }`}
            >
              {statusMessage.type === 'success' ? (
                <CheckCircle2 className="w-4 h-4 shrink-0 text-emerald-400" />
              ) : (
                <AlertCircle className="w-4 h-4 shrink-0 text-red-400" />
              )}
              <span>{statusMessage.text}</span>
            </div>
          )}

          {/* Export Options */}
          <div className="space-y-2">
            <span className="text-xs font-semibold text-slate-300 uppercase tracking-wider block">
              Export Data (Download to device)
            </span>
            <div className="grid grid-cols-2 gap-2.5">
              <button
                onClick={handleExportCSV}
                className="flex flex-col items-center justify-center p-3.5 rounded-xl bg-slate-950 border border-slate-700/80 hover:border-emerald-500/60 hover:bg-slate-900 transition-all cursor-pointer group"
              >
                <FileSpreadsheet className="w-6 h-6 text-emerald-400 group-hover:scale-110 transition-transform mb-1.5" />
                <span className="font-semibold text-xs text-white">Export to CSV</span>
                <span className="text-[10px] text-slate-400">Excel / Spreadsheet</span>
              </button>

              <button
                onClick={handleExportJSON}
                className="flex flex-col items-center justify-center p-3.5 rounded-xl bg-slate-950 border border-slate-700/80 hover:border-indigo-500/60 hover:bg-slate-900 transition-all cursor-pointer group"
              >
                <FileText className="w-6 h-6 text-indigo-400 group-hover:scale-110 transition-transform mb-1.5" />
                <span className="font-semibold text-xs text-white">JSON Backup</span>
                <span className="text-[10px] text-slate-400">Full device restore</span>
              </button>
            </div>
          </div>

          {/* Import Option */}
          <div className="pt-2 border-t border-slate-800 space-y-2">
            <span className="text-xs font-semibold text-slate-300 uppercase tracking-wider block">
              Restore from Backup
            </span>
            <input
              type="file"
              ref={fileInputRef}
              onChange={handleFileChange}
              accept=".json"
              className="hidden"
            />
            <button
              onClick={() => fileInputRef.current?.click()}
              className="w-full flex items-center justify-center gap-2 p-3 rounded-xl bg-slate-900 border border-dashed border-slate-700 hover:border-indigo-400 hover:bg-slate-850 text-xs font-medium text-slate-300 transition-all cursor-pointer"
            >
              <Upload className="w-4 h-4 text-indigo-400" />
              <span>Select JSON Backup File to Restore</span>
            </button>
          </div>

          {/* Clear All Option */}
          {(tasks.length > 0 || history.length > 0) && (
            <div className="pt-2 border-t border-slate-800">
              <button
                onClick={handleClearAll}
                className="w-full flex items-center justify-center gap-1.5 py-2 px-3 rounded-xl bg-red-950/30 hover:bg-red-950/60 text-red-400 border border-red-500/30 text-[11px] transition-colors cursor-pointer"
              >
                <Trash2 className="w-3.5 h-3.5" />
                <span>Clear All Tasks &amp; History (Fresh Reset)</span>
              </button>
            </div>
          )}
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
