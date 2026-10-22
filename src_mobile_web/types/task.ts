export type Priority = 'Low' | 'Medium' | 'High';

export type TaskStatus = 'Pending' | 'Completed';

export interface TaskReminder {
  enabled: boolean;
  time: string; // "YYYY-MM-DDTHH:mm" format
}

export interface Task {
  id: number;
  title: string;
  description: string;
  priority: Priority;
  deadline: string; // "YYYY-MM-DD"
  status: TaskStatus;
  manual_order: number;
  created_at: number; // Unix timestamp in seconds
  completed_at?: string; // Timestamp string when completed
  reminder?: TaskReminder;
}

export type SortMode = 'Urgency' | 'Deadline' | 'Priority' | 'Alphabetical' | 'Manual';
export type SortDirection = 'Asc' | 'Desc';
export type StatusFilter = 'All' | 'PendingOnly' | 'CompletedOnly' | 'OverdueOnly';
export type PriorityFilter = 'All' | 'HighOnly' | 'Med & High' | 'LowOnly';

export interface FilterOptions {
  searchQuery: string;
  statusFilter: StatusFilter;
  priorityFilter: PriorityFilter;
  sortMode: SortMode;
  sortDirection: SortDirection;
}

export interface TaskStatistics {
  totalTasks: number;
  completedTasks: number;
  pendingTasks: number;
  overdueTasks: number;
  highPriorityPending: number;
  mediumPriorityPending: number;
  lowPriorityPending: number;
  completionRate: number; // 0.0 to 1.0
  averageUrgency: number;
}
