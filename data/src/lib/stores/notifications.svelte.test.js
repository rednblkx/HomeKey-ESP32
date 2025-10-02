import { describe, it, expect, beforeEach, afterEach, vi } from 'vitest';
import { notifications } from './notifications.svelte.js';

describe('notifications.svelte.js', () => {
	beforeEach(() => {
		// Reset the store before each test
		notifications.reset();
	});

	describe('notifications store initialization', () => {
		it('should start with empty notifications array', () => {
			expect(notifications.notifications).toEqual([]);
		});
	});

	describe('add method', () => {
		it('should add a notification with default timeout', () => {
			notifications.add('info', 'Test message');

			expect(notifications.notifications).toHaveLength(1);
			expect(notifications.notifications[0]).toEqual({
				id: '1',
				type: 'info',
				message: 'Test message',
				timeout: 5000,
			});
		});

		it('should add a notification with custom timeout', () => {
			notifications.add('success', 'Custom timeout', 10000);

			expect(notifications.notifications[0].timeout).toBe(10000);
		});

		it('should add notification with zero timeout (no auto-dismiss)', () => {
			notifications.add('warning', 'Persistent message', 0);

			expect(notifications.notifications[0].timeout).toBe(0);
		});

		it('should generate unique IDs for multiple notifications', () => {
			notifications.add('info', 'First');
			notifications.add('success', 'Second');
			notifications.add('error', 'Third');

			expect(notifications.notifications[0].id).toBe('1');
			expect(notifications.notifications[1].id).toBe('2');
			expect(notifications.notifications[2].id).toBe('3');
		});
	});

	describe('convenience methods', () => {
		it('should add success notification', () => {
			notifications.addSuccess('Operation completed');

			expect(notifications.notifications[0]).toEqual({
				id: '1',
				type: 'success',
				message: 'Operation completed',
				timeout: 5000,
			});
		});

		it('should add error notification', () => {
			notifications.addError('Something went wrong');

			expect(notifications.notifications[0].type).toBe('error');
			expect(notifications.notifications[0].message).toBe('Something went wrong');
		});

		it('should add warning notification', () => {
			notifications.addWarning('Warning message');

			expect(notifications.notifications[0].type).toBe('warning');
		});

		it('should add info notification', () => {
			notifications.addInfo('Information');

			expect(notifications.notifications[0].type).toBe('info');
		});

		it('should respect custom timeout in convenience methods', () => {
			notifications.addSuccess('Success with custom timeout', 3000);

			expect(notifications.notifications[0].timeout).toBe(3000);
		});
	});

	describe('remove method', () => {
		beforeEach(() => {
			notifications.add('info', 'First');
			notifications.add('success', 'Second');
			notifications.add('error', 'Third');
		});

		it('should remove notification by ID', () => {
			expect(notifications.notifications).toHaveLength(3);

			notifications.remove('2');

			expect(notifications.notifications).toHaveLength(2);
			expect(notifications.notifications.map(n => n.id)).toEqual(['1', '3']);
		});

		it('should not remove anything if ID does not exist', () => {
			const originalLength = notifications.notifications.length;

			notifications.remove('nonexistent');

			expect(notifications.notifications).toHaveLength(originalLength);
		});

		it('should handle removing the first notification', () => {
			notifications.remove('1');

			expect(notifications.notifications).toHaveLength(2);
			expect(notifications.notifications[0].id).toBe('2');
		});

		it('should handle removing the last notification', () => {
			notifications.remove('3');

			expect(notifications.notifications).toHaveLength(2);
			expect(notifications.notifications[1].id).toBe('2');
		});
	});

	describe('auto-dismiss functionality', () => {
		beforeEach(() => {
			vi.useFakeTimers();
		});

		afterEach(() => {
			vi.restoreAllMocks();
		});

		it('should auto-dismiss notification after timeout', () => {
			notifications.add('info', 'Auto-dismiss test', 1000);

			expect(notifications.notifications).toHaveLength(1);

			vi.advanceTimersByTime(1000);

			expect(notifications.notifications).toHaveLength(0);
		});

		it('should not auto-dismiss notification with zero timeout', () => {
			notifications.add('info', 'Persistent', 0);

			vi.advanceTimersByTime(10000);

			expect(notifications.notifications).toHaveLength(1);
		});

		it('should clear timeout when notification is manually removed', () => {
			notifications.add('info', 'Manual remove test', 5000);
			const notificationId = notifications.notifications[0].id;

			vi.advanceTimersByTime(2000);
			notifications.remove(notificationId);

			vi.advanceTimersByTime(3000); // Would have triggered if not cleared

			expect(notifications.notifications).toHaveLength(0);
		});
	});

	describe('reactive behavior', () => {
		it('should be reactive to notification changes', () => {
			let notificationCount = 0;

			// Simulate reactive effect
			const effect = () => {
				notificationCount = notifications.notifications.length;
			};

			effect();
			expect(notificationCount).toBe(0);

			notifications.add('info', 'New notification');
			effect();
			expect(notificationCount).toBe(1);

			notifications.remove('1');
			effect();
			expect(notificationCount).toBe(0);
		});

		it('should maintain notification order', () => {
			notifications.add('info', 'First');
			notifications.add('success', 'Second');
			notifications.add('error', 'Third');

			expect(notifications.notifications.map(n => n.message)).toEqual([
				'First',
				'Second',
				'Third',
			]);

			notifications.remove('2'); // Remove 'Second'

			expect(notifications.notifications.map(n => n.message)).toEqual([
				'First',
				'Third',
			]);
		});
	});

	describe('edge cases', () => {
		it('should handle empty message', () => {
			notifications.add('info', '');

			expect(notifications.notifications[0].message).toBe('');
		});

		it('should handle very long messages', () => {
			const longMessage = 'A'.repeat(1000);
			notifications.add('info', longMessage);

			expect(notifications.notifications[0].message).toBe(longMessage);
		});

		it('should handle special characters in message', () => {
			const specialMessage = 'Message with émojis 🎉 and spëcial chärs';
			notifications.add('info', specialMessage);

			expect(notifications.notifications[0].message).toBe(specialMessage);
		});
	});
});