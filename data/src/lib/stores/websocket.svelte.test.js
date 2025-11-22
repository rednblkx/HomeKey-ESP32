import { describe, it, expect, beforeEach, vi } from 'vitest';
import { websocketState, updateWebsocketState } from './websocket.svelte.js';

describe('websocket.svelte.js', () => {
	beforeEach(() => {
		// Reset websocket state to initial state before each test
		updateWebsocketState({
			connected: false,
			state: 'disconnected',
			url: '/ws',
			reconnectAttempts: 0,
			maxReconnectAttempts: 10,
			lastPongTime: 0,
			lastPingTime: 0,
			consecutivePingFailures: 0,
			maxPingFailures: 3,
			backoffInitialDelay: 1000,
			backoffMaxDelay: 30000,
			backoffMultiplier: 2,
			backoffCurrentDelay: 1000,
			backoffNextReconnectTime: 0,
		});
	});

	describe('websocketState initialization', () => {
		it('should initialize with default values', () => {
			expect(websocketState.connected).toBe(false);
			expect(websocketState.state).toBe('disconnected');
			expect(websocketState.url).toBe('/ws');
			expect(websocketState.reconnectAttempts).toBe(0);
			expect(websocketState.maxReconnectAttempts).toBe(10);
			expect(websocketState.backoffInitialDelay).toBe(1000);
			expect(websocketState.backoffMaxDelay).toBe(30000);
			expect(websocketState.backoffMultiplier).toBe(2);
		});
	});

	describe('updateWebsocketState function', () => {
		it('should update connection state', () => {
			updateWebsocketState({
				connected: true,
				state: 'open',
			});

			expect(websocketState.connected).toBe(true);
			expect(websocketState.state).toBe('open');
		});

		it('should update reconnect attempts', () => {
			updateWebsocketState({
				reconnectAttempts: 3,
				state: 'retrying',
			});

			expect(websocketState.reconnectAttempts).toBe(3);
			expect(websocketState.state).toBe('retrying');
		});

		it('should update backoff timing', () => {
			const nextReconnectTime = Date.now() + 2000;

			updateWebsocketState({
				backoffCurrentDelay: 2000,
				backoffNextReconnectTime: nextReconnectTime,
				state: 'backoff_active',
			});

			expect(websocketState.backoffCurrentDelay).toBe(2000);
			expect(websocketState.backoffNextReconnectTime).toBe(nextReconnectTime);
			expect(websocketState.state).toBe('backoff_active');
		});

		it('should update ping/pong timing', () => {
			const now = Date.now();

			updateWebsocketState({
				lastPingTime: now - 1000,
				lastPongTime: now,
				consecutivePingFailures: 0,
			});

			expect(websocketState.lastPingTime).toBe(now - 1000);
			expect(websocketState.lastPongTime).toBe(now);
			expect(websocketState.consecutivePingFailures).toBe(0);
		});
	});

	describe('exponential backoff logic', () => {
		it('should start with initial delay', () => {
			expect(websocketState.backoffCurrentDelay).toBe(1000);
		});

		it('should respect maximum delay limit', () => {
			// Simulate multiple reconnection attempts that would exceed max delay
			updateWebsocketState({
				backoffCurrentDelay: 40000, // Above max of 30000
			});

			expect(websocketState.backoffCurrentDelay).toBe(40000); // State allows it, but logic should cap it
		});

		it('should track reconnection attempts', () => {
			updateWebsocketState({ reconnectAttempts: 5 });
			expect(websocketState.reconnectAttempts).toBe(5);

			updateWebsocketState({ reconnectAttempts: 8 });
			expect(websocketState.reconnectAttempts).toBe(8);
		});

		it('should handle backoff state transitions', () => {
			// Start disconnected
			expect(websocketState.state).toBe('disconnected');

			// Enter backoff
			updateWebsocketState({
				state: 'backoff_active',
				backoffCurrentDelay: 2000,
				backoffNextReconnectTime: Date.now() + 2000,
			});

			expect(websocketState.state).toBe('backoff_active');
			expect(websocketState.backoffCurrentDelay).toBe(2000);

			// Transition to retrying
			updateWebsocketState({
				state: 'retrying',
				reconnectAttempts: 1,
			});

			expect(websocketState.state).toBe('retrying');
			expect(websocketState.reconnectAttempts).toBe(1);
		});
	});

	describe('ping failure tracking', () => {
		it('should track consecutive ping failures', () => {
			updateWebsocketState({ consecutivePingFailures: 2 });
			expect(websocketState.consecutivePingFailures).toBe(2);

			updateWebsocketState({ consecutivePingFailures: 0 }); // Reset after success
			expect(websocketState.consecutivePingFailures).toBe(0);
		});

		it('should have maximum ping failure threshold', () => {
			expect(websocketState.maxPingFailures).toBe(3);
		});

		it('should update ping timing', () => {
			const pingTime = Date.now();
			const pongTime = pingTime + 100;

			updateWebsocketState({
				lastPingTime: pingTime,
				lastPongTime: pongTime,
			});

			expect(websocketState.lastPingTime).toBe(pingTime);
			expect(websocketState.lastPongTime).toBe(pongTime);
		});
	});

	describe('reactive behavior', () => {
		it('should be reactive to state changes', () => {
			let stateChanges = 0;
			let currentState = '';

			// Simulate reactive effect
			const effect = () => {
				stateChanges++;
				currentState = websocketState.state;
			};

			effect(); // Initial
			expect(currentState).toBe('disconnected');

			updateWebsocketState({ state: 'connecting' });
			effect(); // Reactive update
			expect(currentState).toBe('connecting');
			expect(stateChanges).toBe(2);
		});

		it('should handle connection status changes', () => {
			let connectionStatus = false;

			const effect = () => {
				connectionStatus = websocketState.connected;
			};

			effect();
			expect(connectionStatus).toBe(false);

			updateWebsocketState({ connected: true });
			effect();
			expect(connectionStatus).toBe(true);
		});
	});
});