let notificationId = 0;

class NotificationStore {
  /** @type {import('../types/api.js').Notification[]} */
  notifications = $state([]);

  /**
   * @param {import('../types/api.js').NotificationType} type
   * @param {string} message
   * @param {number} [timeout=5000]
   */
  add(type, message, timeout = 5000) {
    const id = (++notificationId).toString();
    /** @type {import('../types/api.js').Notification} */
    const notification = { id, type, message, timeout };
    this.notifications.push(notification);

    if (timeout > 0) {
      setTimeout(() => this.remove(id), timeout);
    }
  }

  /**
   * @param {string} message
   * @param {number} [timeout]
   */
  addSuccess(message, timeout) {
    this.add('success', message, timeout);
  }

  /**
   * @param {string} message
   * @param {number} [timeout]
   */
  addError(message, timeout) {
    this.add('error', message, timeout);
  }

  /**
   * @param {string} message
   * @param {number} [timeout]
   */
  addWarning(message, timeout) {
    this.add('warning', message, timeout);
  }

  /**
   * @param {string} message
   * @param {number} [timeout]
   */
  addInfo(message, timeout) {
    this.add('info', message, timeout);
  }

  /**
   * @param {string} id
   */
  remove(id) {
    const index = this.notifications.findIndex(n => n.id === id);
    if (index !== -1) {
      this.notifications.splice(index, 1);
    }
  }

  /**
   * Reset the store for testing
   */
  reset() {
    this.notifications = [];
    notificationId = 0;
  }
}

export const notifications = new NotificationStore();