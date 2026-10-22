import React from 'react';
import { Search, X, ArrowUpDown, ArrowDown, ArrowUp } from 'lucide-react';
import { FilterOptions, StatusFilter, PriorityFilter, SortMode } from '../types/task';

interface ControlPanelProps {
  options: FilterOptions;
  onChange: (options: FilterOptions) => void;
}

export const ControlPanel: React.FC<ControlPanelProps> = ({ options, onChange }) => {
  const handleSearchChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    onChange({ ...options, searchQuery: e.target.value });
  };

  const handleClearSearch = () => {
    onChange({ ...options, searchQuery: '' });
  };

  const handleStatusChange = (e: React.ChangeEvent<HTMLSelectElement>) => {
    onChange({ ...options, statusFilter: e.target.value as StatusFilter });
  };

  const handlePriorityChange = (e: React.ChangeEvent<HTMLSelectElement>) => {
    onChange({ ...options, priorityFilter: e.target.value as PriorityFilter });
  };

  const handleSortModeChange = (e: React.ChangeEvent<HTMLSelectElement>) => {
    onChange({ ...options, sortMode: e.target.value as SortMode });
  };

  const toggleSortDirection = () => {
    onChange({
      ...options,
      sortDirection: options.sortDirection === 'Desc' ? 'Asc' : 'Desc',
    });
  };

  return (
    <div className="bg-slate-900/60 border border-slate-800/80 rounded-2xl p-3.5 mb-4 shadow-md backdrop-blur-sm">
      <div className="flex flex-col lg:flex-row gap-3 items-stretch lg:items-center justify-between">
        {/* Search Bar */}
        <div className="relative flex-1 min-w-[200px]">
          <div className="absolute inset-y-0 left-0 pl-3 flex items-center pointer-events-none text-slate-400">
            <Search className="w-4 h-4" />
          </div>
          <input
            type="text"
            value={options.searchQuery}
            onChange={handleSearchChange}
            placeholder="Search by title or details..."
            className="w-full pl-9 pr-9 py-2 rounded-xl bg-slate-950/80 border border-slate-700/80 text-sm text-slate-100 placeholder-slate-500 focus:outline-none focus:border-indigo-500 focus:ring-1 focus:ring-indigo-500 transition-all"
          />
          {options.searchQuery && (
            <button
              onClick={handleClearSearch}
              className="absolute inset-y-0 right-0 pr-3 flex items-center text-slate-400 hover:text-slate-200"
            >
              <X className="w-4 h-4" />
            </button>
          )}
        </div>

        {/* Filters and Sorting Controls */}
        <div className="flex flex-wrap items-center gap-2">
          {/* Status Filter */}
          <div className="flex-1 sm:flex-initial">
            <select
              value={options.statusFilter}
              onChange={handleStatusChange}
              className="w-full sm:w-auto px-3 py-2 rounded-xl bg-slate-950/80 border border-slate-700/80 text-xs sm:text-sm text-slate-200 focus:outline-none focus:border-indigo-500 cursor-pointer"
            >
              <option value="All">Show: All</option>
              <option value="PendingOnly">Show: Pending</option>
              <option value="CompletedOnly">Show: Completed</option>
              <option value="OverdueOnly">Show: Overdue</option>
            </select>
          </div>

          {/* Priority Filter */}
          <div className="flex-1 sm:flex-initial">
            <select
              value={options.priorityFilter}
              onChange={handlePriorityChange}
              className="w-full sm:w-auto px-3 py-2 rounded-xl bg-slate-950/80 border border-slate-700/80 text-xs sm:text-sm text-slate-200 focus:outline-none focus:border-indigo-500 cursor-pointer"
            >
              <option value="All">Priority: All</option>
              <option value="HighOnly">Priority: High</option>
              <option value="Med & High">Priority: Med &amp; High</option>
              <option value="LowOnly">Priority: Low</option>
            </select>
          </div>

          {/* Sort Mode */}
          <div className="flex-1 sm:flex-initial">
            <select
              value={options.sortMode}
              onChange={handleSortModeChange}
              className="w-full sm:w-auto px-3 py-2 rounded-xl bg-slate-950/80 border border-slate-700/80 text-xs sm:text-sm text-slate-200 focus:outline-none focus:border-indigo-500 cursor-pointer"
            >
              <option value="Urgency">Sort: Urgency</option>
              <option value="Deadline">Sort: Deadline</option>
              <option value="Priority">Sort: Priority</option>
              <option value="Alphabetical">Sort: Alphabetical</option>
              <option value="Manual">Sort: Manual</option>
            </select>
          </div>

          {/* Sort Direction Toggle */}
          <button
            onClick={toggleSortDirection}
            className="flex items-center gap-1 px-3 py-2 rounded-xl bg-slate-950/80 hover:bg-slate-800 border border-slate-700/80 text-xs sm:text-sm text-slate-300 font-mono transition-colors cursor-pointer select-none active:scale-95"
            title={`Toggle sort direction: currently ${options.sortDirection === 'Desc' ? 'Descending' : 'Ascending'}`}
          >
            {options.sortDirection === 'Desc' ? (
              <>
                <ArrowDown className="w-3.5 h-3.5 text-indigo-400" />
                <span>Desc</span>
              </>
            ) : (
              <>
                <ArrowUp className="w-3.5 h-3.5 text-indigo-400" />
                <span>Asc</span>
              </>
            )}
          </button>
        </div>
      </div>
    </div>
  );
};
