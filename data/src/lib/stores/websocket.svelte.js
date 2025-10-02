/**
 * WebSocket connection state interface
 * @typedef {Object} WebSocketState
 * @property {boolean} connected - Whether WebSocket is currently connected
 * @property {string} state - Current connection state
 * @property {string} url - WebSocket URL
 * @property {number} reconnectAttempts - Number of reconnection attempts made
 * @property {number} maxReconnectAttempts - Maximum allowed reconnection attempts
 * @property {number} lastPongTime - Timestamp of last pong received
 * @property {number} lastPingTime - Timestamp of last ping sent
 * @property {number} consecutivePingFailures - Number of consecutive ping failures
 * @property {number} maxPingFailures - Maximum allowed ping failures
 * @property {number} backoffInitialDelay - Initial backoff delay in milliseconds
 * @property {number} backoffMaxDelay - Maximum backoff delay in milliseconds
 * @property {number} backoffMultiplier - Backoff delay multiplier
 * @property {number} backoffCurrentDelay - Current backoff delay
 * @property {number} backoffNextReconnectTime - Timestamp for next reconnection attempt
 */

/**
 * WebSocket connection state store
 * @type {WebSocketState}
 */
export const websocketState = $state({
  connected: false,
  state: 'disconnected',
  url: '/ws',
  reconnectAttempts: 0,
  maxReconnectAttempts: 10,
  lastPongTime: 0,
  lastPingTime: 0,
  consecutivePingFailures: 0,
  maxPingFailures: 3,
  // Exponential backoff state
  backoffInitialDelay: 1000, // 1 second
  backoffMaxDelay: 30000, // 30 seconds
  backoffMultiplier: 2,
  backoffCurrentDelay: 1000,
  backoffNextReconnectTime: 0,
});

/**
 * Update WebSocket state with partial new values
 * @param {Partial<WebSocketState>} newState - Partial state updates
 */
export function updateWebsocketState(newState) {
  Object.assign(websocketState, newState);
}
