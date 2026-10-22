import { Task } from '../types/task';
import { getCurrentDateString } from './urgency';

const STORAGE_KEY = 'task_manager_local_vault_v1';

export interface StorageData {
  version: number;
  tasks: Task[];
  history: Task[];
  lastSaved?: string;
}

export function loadLocalData(): { tasks: Task[]; history: Task[] } {
  try {
    const raw = localStorage.getItem(STORAGE_KEY);
    if (!raw) {
      saveLocalData([], []);
      return { tasks: [], history: [] };
    }
    const parsed: StorageData = JSON.parse(raw);
    return {
      tasks: Array.isArray(parsed.tasks) ? parsed.tasks : [],
      history: Array.isArray(parsed.history) ? parsed.history : [],
    };
  } catch (e) {
    console.error('Failed to load local storage data:', e);
    return { tasks: [], history: [] };
  }
}

export function saveLocalData(tasks: Task[], history: Task[]): boolean {
  try {
    const data: StorageData = {
      version: 1,
      tasks,
      history,
      lastSaved: new Date().toISOString(),
    };
    localStorage.setItem(STORAGE_KEY, JSON.stringify(data));
    return true;
  } catch (e) {
    console.error('Failed to save to local storage:', e);
    return false;
  }
}

export function exportToCSV(tasks: Task[], history: Task[]): void {
  const headers = ['Type', 'ID', 'Title', 'Description', 'Priority', 'Deadline', 'Status', 'CreatedAt', 'CompletedAt'];
  const allRows = [
    ...tasks.map((t) => ['Active', t.id, `"${t.title.replace(/"/g, '""')}"`, `"${t.description.replace(/"/g, '""')}"`, t.priority, t.deadline, t.status, t.created_at, t.completed_at || '']),
    ...history.map((t) => ['History', t.id, `"${t.title.replace(/"/g, '""')}"`, `"${t.description.replace(/"/g, '""')}"`, t.priority, t.deadline, t.status, t.created_at, t.completed_at || '']),
  ];

  const csvContent = 'data:text/csv;charset=utf-8,' + [headers.join(','), ...allRows.map((e) => e.join(','))].join('\n');
  const encodedUri = encodeURI(csvContent);
  const link = document.createElement('a');
  link.setAttribute('href', encodedUri);
  link.setAttribute('download', `task_manager_export_${getCurrentDateString()}.csv`);
  document.body.appendChild(link);
  link.click();
  document.body.removeChild(link);
}

export function exportToJSON(tasks: Task[], history: Task[]): void {
  const data: StorageData = {
    version: 1,
    tasks,
    history,
    lastSaved: new Date().toISOString(),
  };
  const jsonStr = 'data:text/json;charset=utf-8,' + encodeURIComponent(JSON.stringify(data, null, 2));
  const link = document.createElement('a');
  link.setAttribute('href', jsonStr);
  link.setAttribute('download', `task_manager_backup_${getCurrentDateString()}.json`);
  document.body.appendChild(link);
  link.click();
  document.body.removeChild(link);
}

export function importFromJSON(jsonString: string): { tasks: Task[]; history: Task[] } | null {
  try {
    const parsed = JSON.parse(jsonString);
    if (!parsed || (!Array.isArray(parsed.tasks) && !Array.isArray(parsed.history))) {
      return null;
    }
    return {
      tasks: Array.isArray(parsed.tasks) ? parsed.tasks : [],
      history: Array.isArray(parsed.history) ? parsed.history : [],
    };
  } catch {
    return null;
  }
}
