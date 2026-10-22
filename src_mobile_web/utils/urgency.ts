import { Task, Priority, FilterOptions, TaskStatistics } from '../types/task';

export function getPriorityWeight(priority: Priority): number {
  switch (priority) {
    case 'Low':
      return 1.0;
    case 'Medium':
      return 2.5;
    case 'High':
      return 5.0;
    default:
      return 2.5;
  }
}

export function getCurrentDateString(): string {
  const d = new Date();
  const year = d.getFullYear();
  const month = String(d.getMonth() + 1).padStart(2, '0');
  const day = String(d.getDate()).padStart(2, '0');
  return `${year}-${month}-${day}`;
}

export function getCurrentTimestampString(): string {
  const d = new Date();
  const year = d.getFullYear();
  const month = String(d.getMonth() + 1).padStart(2, '0');
  const day = String(d.getDate()).padStart(2, '0');
  const hours = String(d.getHours()).padStart(2, '0');
  const mins = String(d.getMinutes()).padStart(2, '0');
  const secs = String(d.getSeconds()).padStart(2, '0');
  return `${year}-${month}-${day} ${hours}:${mins}:${secs}`;
}

export function addDaysToDate(baseDateStr: string, daysToAdd: number): string {
  const parts = baseDateStr.split('-').map(Number);
  if (parts.length !== 3 || isNaN(parts[0]) || isNaN(parts[1]) || isNaN(parts[2])) {
    return baseDateStr;
  }
  const date = new Date(parts[0], parts[1] - 1, parts[2]);
  date.setDate(date.getDate() + daysToAdd);
  const year = date.getFullYear();
  const month = String(date.getMonth() + 1).padStart(2, '0');
  const day = String(date.getDate()).padStart(2, '0');
  return `${year}-${month}-${day}`;
}

export function calculateDaysDifference(fromDateStr: string, toDateStr: string): number {
  const fromParts = fromDateStr.split('-').map(Number);
  const toParts = toDateStr.split('-').map(Number);
  if (fromParts.length !== 3 || toParts.length !== 3) return 0;
  
  const fromDate = new Date(fromParts[0], fromParts[1] - 1, fromParts[2]);
  const toDate = new Date(toParts[0], toParts[1] - 1, toParts[2]);
  
  const diffTime = toDate.getTime() - fromDate.getTime();
  return Math.round(diffTime / (1000 * 60 * 60 * 24));
}

export function getDaysRemaining(task: Task, todayDateStr: string): number {
  return calculateDaysDifference(todayDateStr, task.deadline);
}

export function isTaskOverdue(task: Task, todayDateStr: string): boolean {
  if (task.status === 'Completed') return false;
  return getDaysRemaining(task, todayDateStr) < 0;
}

/**
 * Exact Dynamic Urgency Scoring formula from C++20 engine:
 * 1. Completed: 0.0
 * 2. Overdue (d < 0): 100.0 + (W * 15.0) + min(|d|, 30) * 2.5
 * 3. Due today (d == 0): 80.0 + (W * 12.0)
 * 4. Upcoming (d > 0): (W * 40.0) / (d + 1.0) + (W * 3.0)
 */
export function getUrgencyScore(task: Task, todayDateStr: string): number {
  if (task.status === 'Completed') {
    return 0.0;
  }

  const weight = getPriorityWeight(task.priority);
  const daysLeft = getDaysRemaining(task, todayDateStr);

  if (daysLeft < 0) {
    const overdueDays = Math.abs(daysLeft);
    return 100.0 + (weight * 15.0) + (Math.min(overdueDays, 30.0) * 2.5);
  } else if (daysLeft === 0) {
    return 80.0 + (weight * 12.0);
  } else {
    return ((weight * 40.0) / (daysLeft + 1.0)) + (weight * 3.0);
  }
}

export function filterAndSortTasks(
  tasks: Task[],
  options: FilterOptions,
  todayDateStr: string
): Task[] {
  let result = tasks.slice();

  // 1. Search Query Filter
  if (options.searchQuery.trim().length > 0) {
    const q = options.searchQuery.toLowerCase();
    result = result.filter(
      (t) =>
        t.title.toLowerCase().includes(q) ||
        t.description.toLowerCase().includes(q)
    );
  }

  // 2. Status Filter
  if (options.statusFilter === 'PendingOnly') {
    result = result.filter((t) => t.status === 'Pending');
  } else if (options.statusFilter === 'CompletedOnly') {
    result = result.filter((t) => t.status === 'Completed');
  } else if (options.statusFilter === 'OverdueOnly') {
    result = result.filter((t) => isTaskOverdue(t, todayDateStr));
  }

  // 3. Priority Filter
  if (options.priorityFilter === 'HighOnly') {
    result = result.filter((t) => t.priority === 'High');
  } else if (options.priorityFilter === 'Med & High') {
    result = result.filter((t) => t.priority === 'High' || t.priority === 'Medium');
  } else if (options.priorityFilter === 'LowOnly') {
    result = result.filter((t) => t.priority === 'Low');
  }

  // 4. Multi-Criteria Sorting Engine
  result.sort((a, b) => {
    const scoreA = getUrgencyScore(a, todayDateStr);
    const scoreB = getUrgencyScore(b, todayDateStr);
    const priorityValA = a.priority === 'High' ? 3 : a.priority === 'Medium' ? 2 : 1;
    const priorityValB = b.priority === 'High' ? 3 : b.priority === 'Medium' ? 2 : 1;
    const titleA = a.title.toLowerCase();
    const titleB = b.title.toLowerCase();

    if (options.sortMode === 'Urgency') {
      if (options.sortDirection === 'Desc') {
        if (scoreB !== scoreA) return scoreB - scoreA;
        if (priorityValB !== priorityValA) return priorityValB - priorityValA;
        if (a.deadline !== b.deadline) return a.deadline.localeCompare(b.deadline);
        return titleA.localeCompare(titleB);
      } else {
        if (scoreA !== scoreB) return scoreA - scoreB;
        if (priorityValA !== priorityValB) return priorityValA - priorityValB;
        if (a.deadline !== b.deadline) return a.deadline.localeCompare(b.deadline);
        return titleA.localeCompare(titleB);
      }
    } else if (options.sortMode === 'Deadline') {
      if (options.sortDirection === 'Asc') {
        if (a.deadline !== b.deadline) return a.deadline.localeCompare(b.deadline);
        if (priorityValB !== priorityValA) return priorityValB - priorityValA;
        if (scoreB !== scoreA) return scoreB - scoreA;
        return titleA.localeCompare(titleB);
      } else {
        if (b.deadline !== a.deadline) return b.deadline.localeCompare(a.deadline);
        if (priorityValA !== priorityValB) return priorityValA - priorityValB;
        if (scoreA !== scoreB) return scoreA - scoreB;
        return titleA.localeCompare(titleB);
      }
    } else if (options.sortMode === 'Priority') {
      if (options.sortDirection === 'Desc') {
        if (priorityValB !== priorityValA) return priorityValB - priorityValA;
        if (scoreB !== scoreA) return scoreB - scoreA;
        if (a.deadline !== b.deadline) return a.deadline.localeCompare(b.deadline);
        return titleA.localeCompare(titleB);
      } else {
        if (priorityValA !== priorityValB) return priorityValA - priorityValB;
        if (scoreA !== scoreB) return scoreA - scoreB;
        if (a.deadline !== b.deadline) return a.deadline.localeCompare(b.deadline);
        return titleA.localeCompare(titleB);
      }
    } else if (options.sortMode === 'Alphabetical') {
      if (options.sortDirection === 'Asc') {
        return titleA.localeCompare(titleB);
      } else {
        return titleB.localeCompare(titleA);
      }
    } else if (options.sortMode === 'Manual') {
      if (options.sortDirection === 'Asc') {
        return a.manual_order - b.manual_order;
      } else {
        return b.manual_order - a.manual_order;
      }
    }

    return a.id - b.id;
  });

  return result;
}

export function computeTaskStatistics(tasks: Task[], todayDateStr: string): TaskStatistics {
  const stats: TaskStatistics = {
    totalTasks: tasks.length,
    completedTasks: 0,
    pendingTasks: 0,
    overdueTasks: 0,
    highPriorityPending: 0,
    mediumPriorityPending: 0,
    lowPriorityPending: 0,
    completionRate: 0,
    averageUrgency: 0,
  };

  let totalUrgency = 0;

  for (const t of tasks) {
    if (t.status === 'Completed') {
      stats.completedTasks++;
    } else {
      stats.pendingTasks++;
      const score = getUrgencyScore(t, todayDateStr);
      totalUrgency += score;

      if (isTaskOverdue(t, todayDateStr)) {
        stats.overdueTasks++;
      }

      if (t.priority === 'High') stats.highPriorityPending++;
      else if (t.priority === 'Medium') stats.mediumPriorityPending++;
      else stats.lowPriorityPending++;
    }
  }

  if (stats.totalTasks > 0) {
    stats.completionRate = stats.completedTasks / stats.totalTasks;
  }

  if (stats.pendingTasks > 0) {
    stats.averageUrgency = totalUrgency / stats.pendingTasks;
  }

  return stats;
}
