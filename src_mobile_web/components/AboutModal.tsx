import React from 'react';
import { X, ShieldCheck, Cpu, Code2, Sparkles, Terminal, CheckCircle2, Lock, Zap } from 'lucide-react';

interface AboutModalProps {
  isOpen: boolean;
  onClose: () => void;
}

export const AboutModal: React.FC<AboutModalProps> = ({ isOpen, onClose }) => {
  if (!isOpen) return null;

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black/80 backdrop-blur-md animate-in fade-in duration-200">
      <div 
        className="relative w-full max-w-lg overflow-hidden rounded-2xl border border-indigo-500/30 bg-[#0d111a] shadow-2xl shadow-indigo-950/50 text-slate-100 flex flex-col max-h-[90vh]"
        onClick={(e) => e.stopPropagation()}
      >
        {/* Glow Accent Bar */}
        <div className="h-1.5 w-full bg-gradient-to-r from-blue-500 via-indigo-500 to-purple-500" />

        {/* Modal Header */}
        <div className="flex items-center justify-between px-6 py-4 border-b border-slate-800/80 bg-slate-900/60">
          <div className="flex items-center space-x-3">
            <div className="w-10 h-10 rounded-xl overflow-hidden border border-indigo-500/40 bg-slate-800 p-1 flex items-center justify-center shadow-lg shadow-indigo-500/20">
              <img src="/task_icon.png" alt="Task Manager Logo" className="w-full h-full object-contain" />
            </div>
            <div>
              <h2 className="text-lg font-bold tracking-wider text-transparent bg-clip-text bg-gradient-to-r from-indigo-300 via-indigo-100 to-white flex items-center gap-1.5">
                TASK MANAGER
                <span className="text-[10px] font-semibold uppercase px-1.5 py-0.5 rounded bg-indigo-500/20 text-indigo-300 border border-indigo-500/30">
                  v1.0 Local
                </span>
              </h2>
              <p className="text-xs text-slate-400">Architecture & Developer Manifest</p>
            </div>
          </div>
          <button
            onClick={onClose}
            className="p-1.5 rounded-lg text-slate-400 hover:text-white hover:bg-slate-800/80 transition-colors"
            aria-label="Close Modal"
          >
            <X className="w-5 h-5" />
          </button>
        </div>

        {/* Modal Scrollable Body */}
        <div className="p-6 space-y-5 overflow-y-auto custom-scrollbar text-sm">
          {/* Main Attribution Section */}
          <div className="p-4 rounded-xl bg-gradient-to-br from-indigo-950/40 via-slate-900 to-slate-900/80 border border-indigo-500/20 shadow-inner">
            <div className="flex items-center gap-2 mb-2 text-indigo-400 font-semibold text-xs tracking-wider uppercase">
              <Code2 className="w-4 h-4 text-indigo-400" />
              Lead Developers & Creators
            </div>
            <div className="text-base font-bold text-white tracking-wide flex flex-wrap items-center gap-2">
              <span className="px-2.5 py-1 rounded-md bg-indigo-600/30 border border-indigo-500/40 text-indigo-200">
                DEVELOPER ___CODEX
              </span>
              <span className="text-slate-400">&amp;</span>
              <span className="px-2.5 py-1 rounded-md bg-purple-600/30 border border-purple-500/40 text-purple-200">
                SHERALOCK
              </span>
            </div>
          </div>

          {/* AI & Development Assistant */}
          <div className="p-4 rounded-xl bg-gradient-to-br from-purple-950/40 via-slate-900 to-slate-900/80 border border-purple-500/20 shadow-inner">
            <div className="flex items-center gap-2 mb-2 text-purple-400 font-semibold text-xs tracking-wider uppercase">
              <Sparkles className="w-4 h-4 text-purple-400" />
              AI &amp; Development Assistant
            </div>
            <div className="text-sm font-mono font-bold text-purple-200 bg-purple-900/30 border border-purple-500/30 rounded-lg p-2.5 flex items-center gap-2">
              <Terminal className="w-4 h-4 text-purple-400 shrink-0" />
              AGI CODEX GEMINI GPT
            </div>
          </div>

          {/* Security & Zero Backend Guarantee */}
          <div className="p-4 rounded-xl bg-emerald-950/20 border border-emerald-500/30 space-y-2">
            <div className="flex items-center gap-2 text-emerald-400 font-semibold text-xs tracking-wider uppercase">
              <ShieldCheck className="w-4 h-4" />
              100% Local &amp; Zero-Server Guarantee
            </div>
            <p className="text-xs text-slate-300 leading-relaxed">
              This application operates <strong className="text-emerald-300">100% locally on your Android device</strong>. 
              There is <strong className="text-emerald-300">no external backend server</strong>, no remote database, no analytics, and zero telemetry.
            </p>
            <div className="grid grid-cols-2 gap-2 pt-1 text-[11px] text-slate-300 font-mono">
              <div className="flex items-center gap-1.5 bg-slate-900/80 p-2 rounded border border-emerald-500/20">
                <Lock className="w-3.5 h-3.5 text-emerald-400" />
                <span>Isolated Storage</span>
              </div>
              <div className="flex items-center gap-1.5 bg-slate-900/80 p-2 rounded border border-emerald-500/20">
                <CheckCircle2 className="w-3.5 h-3.5 text-emerald-400" />
                <span>100% Offline Active</span>
              </div>
            </div>
          </div>

          {/* Core Engine Features */}
          <div className="p-4 rounded-xl bg-slate-900/80 border border-slate-800 space-y-2.5">
            <div className="flex items-center gap-2 text-indigo-400 font-semibold text-xs tracking-wider uppercase">
              <Zap className="w-4 h-4" />
              Dynamic Prioritization Engine
            </div>
            <ul className="text-xs text-slate-300 space-y-1.5 list-disc list-inside">
              <li>
                <strong className="text-slate-200">Mathematical Decay Formula</strong> calculates real-time urgency weights for imminent deadlines.
              </li>
              <li>
                <strong className="text-slate-200">Hierarchical Tie-Breaking</strong>: Urgency &rarr; Priority &rarr; Deadline &rarr; Lexicographical.
              </li>
              <li>
                <strong className="text-slate-200">Completed Task Vault</strong> with one-click restore and permanent purge capabilities.
              </li>
              <li>
                <strong className="text-slate-200">One-Click Backup &amp; CSV Export</strong> directly to local device storage.
              </li>
              <li>
                <strong className="text-slate-200">100% Local Task Reminders</strong>: Powered by native Android alarms. Runs offline without battery drain, zero push servers.
              </li>
            </ul>
          </div>
        </div>

        {/* Modal Footer */}
        <div className="flex items-center justify-between px-6 py-3.5 border-t border-slate-800/80 bg-slate-900/80">
          <div className="flex items-center gap-1.5 text-[11px] text-slate-400">
            <Cpu className="w-3.5 h-3.5 text-indigo-400" />
            <span>Standalone Android APK &amp; Web</span>
          </div>
          <button
            onClick={onClose}
            className="px-4 py-2 rounded-xl bg-indigo-600 hover:bg-indigo-500 text-white font-medium text-xs shadow-lg shadow-indigo-600/30 transition-all cursor-pointer active:scale-95"
          >
            Close &amp; Return
          </button>
        </div>
      </div>
    </div>
  );
};
