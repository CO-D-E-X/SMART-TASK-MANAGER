import { LocalNotifications } from '@capacitor/local-notifications';
import { Capacitor } from '@capacitor/core';
import { Task } from '../types/task';

const CHANNEL_ID = 'task_reminders_channel_v1';

/**
 * Initialize high-importance Android notification channel
 */
export async function initNotificationService(): Promise<void> {
  if (Capacitor.isNativePlatform()) {
    try {
      await LocalNotifications.createChannel({
        id: CHANNEL_ID,
        name: 'Task Reminders',
        description: 'Urgent alerts and scheduled reminders for your prioritized tasks',
        importance: 5, // High importance (heads-up banner and sound)
        visibility: 1, // Public visibility on lockscreen
        sound: 'default',
        vibration: true,
        lights: true,
        lightColor: '#6366f1',
      });
    } catch (e) {
      console.warn('Could not create notification channel:', e);
    }
  }
}

/**
 * Request notification permissions from Android OS
 */
export async function requestNotificationPermission(): Promise<boolean> {
  if (Capacitor.isNativePlatform()) {
    try {
      const status = await LocalNotifications.checkPermissions();
      if (status.display === 'granted') {
        return true;
      }
      const requested = await LocalNotifications.requestPermissions();
      return requested.display === 'granted';
    } catch (e) {
      console.warn('Notification permission check failed:', e);
      return false;
    }
  } else if (typeof window !== 'undefined' && 'Notification' in window) {
    if (Notification.permission === 'granted') return true;
    if (Notification.permission !== 'denied') {
      const perm = await Notification.requestPermission();
      return perm === 'granted';
    }
  }
  return false;
}

/**
 * Schedule a 100% local device notification for a task
 */
export async function scheduleTaskReminder(task: Task): Promise<boolean> {
  if (!task.reminder?.enabled || !task.reminder.time || task.status === 'Completed') {
    return false;
  }

  const reminderDate = new Date(task.reminder.time);
  const now = new Date();

  // If reminder is in the past, do not schedule
  if (isNaN(reminderDate.getTime()) || reminderDate.getTime() <= now.getTime()) {
    return false;
  }

  await requestNotificationPermission();

  if (Capacitor.isNativePlatform()) {
    try {
      // First clear any existing notification for this task
      await LocalNotifications.cancel({
        notifications: [{ id: task.id }],
      });

      // Schedule exact local alarm notification
      await LocalNotifications.schedule({
        notifications: [
          {
            id: task.id,
            title: `⏰ Task Reminder: ${task.title}`,
            body: `Priority: ${task.priority} | Due: ${task.deadline}${task.description ? ' • ' + task.description : ''}`,
            schedule: {
              at: reminderDate,
              allowWhileIdle: true,
            },
            channelId: CHANNEL_ID,
            smallIcon: 'ic_launcher',
            sound: 'default',
            actionTypeId: '',
            extra: {
              taskId: task.id,
            },
          },
        ],
      });
      return true;
    } catch (e) {
      console.error('Failed to schedule local notification:', e);
      return false;
    }
  } else if (typeof window !== 'undefined' && 'Notification' in window && Notification.permission === 'granted') {
    // Browser fallback
    const delay = reminderDate.getTime() - now.getTime();
    if (delay > 0 && delay < 2147483647) {
      setTimeout(() => {
        new Notification(`⏰ Task Reminder: ${task.title}`, {
          body: `Priority: ${task.priority} | Due: ${task.deadline}`,
          icon: '/task_icon.png',
        });
      }, delay);
    }
    return true;
  }

  return false;
}

/**
 * Cancel a scheduled local notification for a task
 */
export async function cancelTaskReminder(taskId: number): Promise<void> {
  if (Capacitor.isNativePlatform()) {
    try {
      await LocalNotifications.cancel({
        notifications: [{ id: taskId }],
      });
    } catch (e) {
      console.warn('Error cancelling local notification:', e);
    }
  }
}

/**
 * Reschedule all active task reminders
 */
export async function syncAllReminders(tasks: Task[]): Promise<void> {
  await initNotificationService();
  for (const task of tasks) {
    if (task.status === 'Pending' && task.reminder?.enabled && task.reminder.time) {
      await scheduleTaskReminder(task);
    } else {
      await cancelTaskReminder(task.id);
    }
  }
}

/**
 * Helper to format reminder time string for UI
 */
export function formatReminderDisplay(timeStr?: string): string {
  if (!timeStr) return '';
  const d = new Date(timeStr);
  if (isNaN(d.getTime())) return timeStr;

  return d.toLocaleString([], {
    month: 'short',
    day: 'numeric',
    hour: '2-digit',
    minute: '2-digit',
  });
}
