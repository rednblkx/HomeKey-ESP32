type WebSocketConnectionState = 'disconnected' | 'connecting' | 'open' | 'reconnecting' | 'failed' | 'error';

type WebSocketState = {
  connected : boolean,
  state : WebSocketConnectionState,
  url: string,
  reconnectAttempts: number,
  maxReconnectAttempts: number,
  connectionTimeout: number,
  pingInterval: number,
  pongTimeout: number,
  backoffInitialDelay: number,
  backoffMaxDelay: number,
  backoffMultiplier: number
}

export const websocketState : WebSocketState = $state({
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

export function updateWebsocketState(newState : Partial<WebSocketState>) {
  Object.assign(websocketState, newState);
}
