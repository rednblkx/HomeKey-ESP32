import { updateWebsocketState } from '../stores/websocket.svelte.js';

/**
 * @typedef {Object} StatusEvent
 * @property {'status'} type
 * @property {import('../stores/websocket.svelte.js').WebSocketState} data
 */

/**
 * @typedef {Object} MessageEventData
 * @property {'message'} type
 * @property {object | string} data - The parsed message data (if JSON) or raw data.
 * @property {string} raw - The raw, unparsed message string.
 */

/**
 * @typedef {StatusEvent | MessageEventData} WebSocketEvent
 */

/**
 * @callback MessageHandler
 * @param {WebSocketEvent} event
 * @returns {void}
 */


/**
 * A robust WebSocket service class for managing WebSocket connections with features like
 * automatic reconnection, connection timeouts, proactive liveness checks (ping/pong),
 * and protection against race conditions from old connections.
 */
class WSService {
  /**
   * @param {string} [url] - WebSocket URL (defaults to '/ws').
   */
  constructor(url = '/ws') {
    /** @type {WebSocket | null} */
    this.socket = null;
    /** @type {string} */
    this.url = url;
    /** @type {Set<MessageHandler>} */
    this.handlers = new Set();
    /** @type {boolean} */
    this.connected = false;
    /** @type {number} */
    this.reconnectAttempts = 0;
    /** @type {boolean} */
    this._shouldReconnect = true;

    /** @type {ReturnType<typeof setTimeout> | null} */
    this._reconnectTimer = null;
    /** @type {ReturnType<typeof setInterval> | null} */
    this._pingTimer = null;
    /** @type {ReturnType<typeof setTimeout> | null} */
    this._connectionTimeout = null;

    /** @type {import('../stores/websocket.svelte.js').WebSocketConnectionState} */
    this._lastState = 'disconnected';
    /** @type {number} */
    this._lastPongTime = 0;
    
    /** 
     * A unique ID for each connection attempt. 
     * This is used in preventing events from old, "zombie" connections 
     * from interfering with the current, active one.
     * @private
     * @type {number} 
     */
    this._currentAttemptId = 0;

    /** @type {Omit<import('../stores/websocket.svelte.js').WebSocketState, 'connected' | 'state' | 'url' | 'reconnectAttempts'>} */
    this.config = {
      maxReconnectAttempts: 10,
      connectionTimeout: 5000,
      pingInterval: 10000,
      pongTimeout: 5000,
      backoffInitialDelay: 1000,
      backoffMaxDelay: 30000,
      backoffMultiplier: 2,
    };
  }

  /**
   * Establishes a WebSocket connection.
   * @param {string} [url] - The WebSocket URL to connect to.
   */
  connect(url) {
    if (this._lastState === 'connecting' || this._reconnectTimer) {
      console.log('Connection or reconnection attempt already in progress.');
      return;
    }

    if (url) {
      this.url = url;
    }

    const attemptId = ++this._currentAttemptId;

    this._cleanup();
    this._updateState('connecting');

    try {
      const socket = new WebSocket(this.url);
      this.socket = socket;

      this._connectionTimeout = setTimeout(() => {
        if (attemptId !== this._currentAttemptId) return;
        console.warn('Connection attempt timed out.');
        this._handleConnectionLoss();
      }, this.config.connectionTimeout);

      socket.addEventListener('open', () => this._onOpen(attemptId));
      socket.addEventListener('message', (event) => this._onMessage(event, attemptId));
      socket.addEventListener('close', (event) => this._onClose(event, attemptId));
      socket.addEventListener('error', (event) => this._onError(event, attemptId));

    } catch (error) {
      console.error('Failed to create WebSocket instance:', error);
      this._handleConnectionLoss();
    }
  }

  /**
   * Disconnects the WebSocket connection gracefully.
   */
  disconnect() {
    console.log('Manual disconnection initiated.');
    this._shouldReconnect = false;
    
    if (this._reconnectTimer) {
      clearTimeout(this._reconnectTimer);
    }
    this._reconnectTimer = null;
    
    this._cleanup();
    this._updateState('disconnected');
  }

  /**
   * Sends a message through the WebSocket.
   * @param {string | object} payload - The message to send.
   * @returns {boolean} - True if the message was sent, false otherwise.
   */
  send(payload) {
    if (!this.socket || this.socket.readyState !== WebSocket.OPEN) {
      console.warn('Cannot send message: WebSocket is not open.');
      return false;
    }
    try {
      const message = typeof payload === 'string' ? payload : JSON.stringify(payload);
      this.socket.send(message);
      return true;
    } catch (error) {
      console.error('Failed to send WebSocket message:', error);
      return false;
    }
  }

  /**
   * Registers a handler for incoming messages.
   * @param {MessageHandler} handler - The message handler function.
   * @returns {() => void} - A function to unregister the handler.
   */
  on(handler) {
    this.handlers.add(handler);
    return () => {
      this.handlers.delete(handler);
    };
  }

  /**
   * Gets the current connection information.
   * @returns {import('../stores/websocket.svelte.js').WebSocketState} - The current connection state.
   */
  getConnectionInfo() {
    return {
      connected: this.connected,
      state: this._lastState,
      url: this.url,
      reconnectAttempts: this.reconnectAttempts,
      ...this.config,
    };
  }

  /** @private @param {number} attemptId */
  _onOpen(attemptId) {
    if (attemptId !== this._currentAttemptId) {
      console.log('Ignoring open event from an old connection attempt.');
      return;
    }
    if (this._connectionTimeout) clearTimeout(this._connectionTimeout);
    console.log('WebSocket connected successfully.');
    this.reconnectAttempts = 0;
    this._lastPongTime = Date.now();
    this._updateState('open');
    this._startPingTimer();
  }

  /** @private @param {MessageEvent<string>} event @param {number} attemptId */
  _onMessage(event, attemptId) {
    if (attemptId !== this._currentAttemptId) return;
    try {
      const parsedData = JSON.parse(event.data);
      if (parsedData && parsedData.type === 'pong') {
        this._lastPongTime = Date.now();
        return;
      }
      this._emit({ type: 'message', data: parsedData, raw: event.data });
    } catch (error) {
      if (event.data === 'pong') {
        this._lastPongTime = Date.now();
        return;
      }
      this._emit({ type: 'message', data: event.data, raw: event.data });
    }
  }

  /** @private @param {CloseEvent} event @param {number} attemptId */
  _onClose(event, attemptId) {
    if (attemptId !== this._currentAttemptId) {
      console.log('Ignoring close event from an old connection attempt.');
      return;
    }
    console.log(`WebSocket closed: Code=${event.code}, Reason=${event.reason}`);
    if (this._shouldReconnect) {
      this._handleConnectionLoss();
    } else {
      this._updateState('disconnected');
    }
  }
  
  /** @private @param {Event} event @param {number} attemptId */
  _onError(event, attemptId) {
    if (attemptId !== this._currentAttemptId) {
      console.log('Ignoring error event from an old connection attempt.');
      return;
    }
    console.error('WebSocket error:', event);
    this._updateState('error');
  }

  /** @private */
  _handleConnectionLoss() {
    if (this._reconnectTimer) return;
    this._cleanup();
    if (this.reconnectAttempts >= this.config.maxReconnectAttempts) {
      console.warn('Max reconnection attempts reached. Halting.');
      this._updateState('failed');
      return;
    }
    this.reconnectAttempts++;
    const delay = this._calculateBackoffDelay();
    console.log(`Scheduling reconnect attempt ${this.reconnectAttempts}/${this.config.maxReconnectAttempts} in ${delay}ms.`);
    this._updateState('reconnecting');
    this._reconnectTimer = setTimeout(() => {
      this._reconnectTimer = null;
      this.connect();
    }, delay);
  }

  /** @private @returns {number} */
  _calculateBackoffDelay() {
    const exponentialDelay = this.config.backoffInitialDelay * Math.pow(this.config.backoffMultiplier, this.reconnectAttempts - 1);
    const cappedDelay = Math.min(exponentialDelay, this.config.backoffMaxDelay);
    const jitter = cappedDelay * 0.2 * (Math.random() - 0.5);
    return Math.floor(cappedDelay + jitter);
  }

  /** @private */
  _startPingTimer() {
    if (this._pingTimer) {
      clearInterval(this._pingTimer);
    }
    
    this._pingTimer = setInterval(() => {
      if (Date.now() - this._lastPongTime > this.config.pingInterval + this.config.pongTimeout) {
        console.warn('Ping timeout detected. Connection is considered dead.');
        this._handleConnectionLoss();
        return;
      }
      this.send({ type: 'ping', timestamp: Date.now() });
    }, this.config.pingInterval);
  }

  /** @private */
  _cleanup() {
    this._cleanupTimers();
    if (this.socket) {
      this.socket.close();
      this.socket = null;
    }
  }

  /** @private */
  _cleanupTimers() {
    if (this._pingTimer) clearInterval(this._pingTimer);
    if (this._connectionTimeout) clearTimeout(this._connectionTimeout);
    this._pingTimer = this._connectionTimeout = null;
  }
  
  /** @private @param {import('../stores/websocket.svelte.js').WebSocketConnectionState} state */
  _updateState(state) {
    if (this._lastState === state) return;
    this._lastState = state;
    this.connected = state === 'open';

    const connectionInfo = this.getConnectionInfo();

    updateWebsocketState(connectionInfo);
    this._emit({ type: 'status', data: connectionInfo });
  }
  
  /** @private @param {WebSocketEvent} event */
  _emit(event) {
    for (const handler of this.handlers) {
      try {
        handler(event);
      } catch (e) {
        console.error('Error in WebSocket event handler:', e);
      }
    }
  }
}

const ws = new WSService();
export default ws;
