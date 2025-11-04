/**
 * @typedef {'disconnected' | 'connecting' | 'open' | 'reconnecting' | 'failed' | 'error'} WebSocketConnectionState
 */
/**
 * WebSocket connection state interface
 * @typedef {Object} WebSocketState
 * @property {boolean} connected
 * @property {WebSocketConnectionState} state
 * @property {string} url
 * @property {number} reconnectAttempts
 * @property {number} maxReconnectAttempts
 * @property {number} connectionTimeout
 * @property {number} pingInterval
 * @property {number} pongTimeout
 * @property {number} backoffInitialDelay
 * @property {number} backoffMaxDelay
 * @property {number} backoffMultiplier
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
  connectionTimeout: 5000,
  pingInterval: 10000,
  pongTimeout: 3000,
  backoffInitialDelay: 1000,
  backoffMaxDelay: 30000,
  backoffMultiplier: 2,
});

/**
 * Update WebSocket state with partial new values
 * @param {Partial<WebSocketState>} newState - Partial state updates
 */
export function updateWebsocketState(newState) {
  Object.assign(websocketState, newState);
}
