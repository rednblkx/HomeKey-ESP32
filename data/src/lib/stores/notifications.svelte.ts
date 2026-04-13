import type { Notification, NotificationType } from "$lib/types/api";

let notificationId = 0;

class NotificationStore {
  notifications : Notification[] = $state([]);

  add(type : NotificationType, message : string, timeout : number = 5000) {
    const id = (++notificationId).toString();
    const notification : Notification = { id, type, message, timeout };
    this.notifications.push(notification);

    if (timeout > 0) {
      setTimeout(() => this.remove(id), timeout);
    }
  }

  addSuccess(message : string, timeout? : number) {
    this.add('success', message, timeout);
  }

  addError(message : string, timeout? : number) {
    this.add('error', message, timeout);
  }

  addWarning(message : string, timeout? : number) {
    this.add('warning', message, timeout);
  }

  addInfo(message : string, timeout? : number) {
    this.add('info', message, timeout);
  }

  remove(id : string) {
    const index = this.notifications.findIndex(n => n.id === id);
    if (index !== -1) {
      this.notifications.splice(index, 1);
    }
  }

  reset() {
    this.notifications = [];
    notificationId = 0;
  }
}

export const notifications = new NotificationStore();
