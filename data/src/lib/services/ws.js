import { updateWebsocketState } from '../stores/websocket.svelte.js';

/**
 * WebSocket service class for managing WebSocket connections with automatic reconnection
 */
class WSService {
  /**
   * @param {string} [url] - WebSocket URL (defaults to '/ws')
   */
  constructor(url) {
    /** @type {WebSocket | null} */
    this.socket = null;
    /** @type {string} */
    this.url = url || '/ws';
    /** @type {Set<Function>} */
    this.handlers = new Set();
    /** @type {boolean} */
    this.connected = false;
    /** @type {number} */
    this.reconnectAttempts = 0;
    /** @type {number} */
    this.maxReconnectAttempts = 10;
    /** @type {boolean} */
    this._shouldReconnect = true;
    /** @type {ReturnType<typeof setTimeout> | null} */
    this._readyStateTimer = null;
    /** @type {ReturnType<typeof setTimeout> | null} */
    this._reconnectTimer = null;
    /** @type {ReturnType<typeof setTimeout> | null} */
    this._pingTimer = null;
    /** @type {ReturnType<typeof setTimeout> | null} */
    this._pingTimeoutTimer = null;
    /** @type {string} */
    this._lastState = 'disconnected';
    /** @type {number} */
    this._lastPongTime = 0;
    /** @type {number} */
    this._lastPingTime = 0;
    /** @type {number} */
    this._pingInterval = 10000; // 10 seconds
    /** @type {number} */
    this._pongTimeout = 3000; // 3 seconds
    /** @type {number} */
    this._consecutivePingFailures = 0;
    /** @type {number} */
    this._maxPingFailures = 3;
    /** @type {number} */
    this._stablePongCount = 0;

    // Exponential backoff configuration
    /** @type {number} */
    this.backoffInitialDelay = 1000; // 1 second
    /** @type {number} */
    this.backoffMaxDelay = 30000; // 30 seconds
    /** @type {number} */
    this.backoffMultiplier = 2;
    /** @type {number} */
    this.backoffCurrentDelay = this.backoffInitialDelay;
    /** @type {number} */
    this.backoffNextReconnectTime = 0;
  }

  /**
   * @param {string} [url]
   */
  connect(url) {
    // Prevent multiple simultaneous connection attempts
    if (this.socket && (this.socket.readyState === WebSocket.CONNECTING || this.socket.readyState === WebSocket.OPEN)) {
      console.log('Connection already in progress or established, ignoring connect() call');
      return;
    }
    if(url)
      this.url = url;

    console.log(`Connecting to WebSocket: ${this.url}`);

    // Clean up existing connection and timers
    this._cleanup();
    
    // Clear any pending reconnection attempts
    if (this._reconnectTimer) {
      clearTimeout(this._reconnectTimer);
      this._reconnectTimer = null;
    }

    // Emit connecting
    this.connected = false;
    this._emitStatus('connecting');

    try {
      this.socket = new WebSocket(this.url);
    } catch (error) {
      console.error('Failed to create WebSocket:', error);
      this._emitStatus('error');
      if (this._shouldReconnect) this._scheduleReconnect();
      return;
    }

    this.socket.addEventListener('open', () => {
      console.log('WebSocket connected');
      this.connected = true;
      // Reset backoff on successful connection
      this.backoffCurrentDelay = this.backoffInitialDelay;
      this.backoffNextReconnectTime = 0;
      // Only reset reconnectAttempts after a successful connection that lasts
      // We'll reset it after the first successful ping/pong cycle
      this._lastPongTime = Date.now();
      this._lastPingTime = 0;
      this._consecutivePingFailures = 0;

      // Clear any pending reconnection timer
      if (this._reconnectTimer) {
        clearTimeout(this._reconnectTimer);
        this._reconnectTimer = null;
      }

      this._emitStatus('open');
      this._startPingTimer();
    });

    this.socket.addEventListener('message', (evt) => {
      this._handleMessage(evt.data);
    });

    this.socket.addEventListener('close', (evt) => {
      console.log('WebSocket closed:', evt.code, evt.reason);
      this.connected = false;
      this._stopPingTimer();
      this._stopPingTimeoutTimer();
      this._stablePongCount = 0; // Reset stable connection counter

      // Determine the appropriate state based on the close reason
      let state = 'closed';
      if (evt.code !== 1000) { // Not a clean close
        state = 'connection_lost';
      }

      this._emitStatus(state);

      // Only reconnect if it wasn't a clean close (code 1000) or if we should reconnect
      if (this._shouldReconnect && (evt.code !== 1000 || this.reconnectAttempts < this.maxReconnectAttempts)) {
        this._scheduleReconnect();
      }
    });

    this.socket.addEventListener('error', (evt) => {
      console.error('WebSocket error:', evt);
      this._emitStatus('error');
      // Don't close here, let the close event handle reconnection
    });

    // start readyState watcher to detect half-open connections
    this._startReadyStateWatcher();
  }

  _calculateBackoffDelay() {
    // Calculate exponential backoff: initialDelay * (multiplier ^ attempt)
    const exponentialDelay = this.backoffInitialDelay * Math.pow(this.backoffMultiplier, this.reconnectAttempts);

    // Cap at maximum delay
    const cappedDelay = Math.min(exponentialDelay, this.backoffMaxDelay);

    // Add jitter (±25% randomization) to prevent thundering herd
    const jitter = cappedDelay * 0.25 * (Math.random() * 2 - 1); // -25% to +25%
    const delayWithJitter = Math.max(100, cappedDelay + jitter); // Minimum 100ms

    return Math.floor(delayWithJitter);
  }

  _scheduleReconnect() {
    if (this._reconnectTimer) {
      clearTimeout(this._reconnectTimer);
    }

    if (this.reconnectAttempts >= this.maxReconnectAttempts) {
      console.warn('Max reconnection attempts reached');
      this._emitStatus('failed');
      return;
    }

    this.reconnectAttempts++;
    this.backoffCurrentDelay = this._calculateBackoffDelay();
    this.backoffNextReconnectTime = Date.now() + this.backoffCurrentDelay;

    console.log(`Scheduling reconnect attempt ${this.reconnectAttempts}/${this.maxReconnectAttempts} in ${this.backoffCurrentDelay}ms (reason: connection lost)`);

    this._emitStatus('backoff_active');

    this._reconnectTimer = setTimeout(() => {
      if (!this._shouldReconnect) return;
      console.log(`Reconnect attempt ${this.reconnectAttempts}/${this.maxReconnectAttempts} - attempting connection`);
      this._emitStatus('retrying');
      this.connect(this.url);
    }, this.backoffCurrentDelay);
  }

  /**
   * @param {string | object} objOrString
   */
  send(objOrString) {
    if (!this.socket || this.socket.readyState !== WebSocket.OPEN) {
      console.warn('Cannot send message: WebSocket not connected');
      return false;
    }
    
    try {
      const payload = typeof objOrString === 'string' ? objOrString : JSON.stringify(objOrString);
      this.socket.send(payload);
      return true;
    } catch (error) {
      console.error('Failed to send WebSocket message:', error);
      return false;
    }
  }

  /**
   * @param {{ (evt: any): void; (event: any): void; (evt: any): void; (event: any): void; }} handler
   */
  on(handler) {
    this.handlers.add(handler);
    return () => this.handlers.delete(handler);
  }

  /**
   * @param {{ type: any; state?: any; data?: any; raw?: any; }} event
   */
  _emit(event) {
    for (const h of this.handlers) {
      try { h(event); } catch (e) { /* ignore */ }
    }
  }

  /**
   * @param {string} state
   */
  _emitStatus(state) {
    this._lastState = state;
    // Update Svelte stores
    updateWebsocketState({
      connected: this.connected,
      state: this._lastState,
      url: this.url,
      reconnectAttempts: this.reconnectAttempts,
      maxReconnectAttempts: this.maxReconnectAttempts,
      lastPongTime: this._lastPongTime,
      lastPingTime: this._lastPingTime,
      consecutivePingFailures: this._consecutivePingFailures,
      maxPingFailures: this._maxPingFailures,
      backoffInitialDelay: this.backoffInitialDelay,
      backoffMaxDelay: this.backoffMaxDelay,
      backoffMultiplier: this.backoffMultiplier,
      backoffCurrentDelay: this.backoffCurrentDelay,
      backoffNextReconnectTime: this.backoffNextReconnectTime,
    });
    this._emit({ type: 'status', state });
  }

  _startReadyStateWatcher() {
    if (this._readyStateTimer) clearInterval(this._readyStateTimer);
    this._readyStateTimer = setInterval(() => {
      // Only check readyState if we have an actual socket
      if (!this.socket) return;
      
      const rs = this.socket.readyState;
      let state = 'connecting';
      switch (rs) {
        case WebSocket.CONNECTING: state = 'connecting'; break;
        case WebSocket.OPEN: state = 'open'; break;
        case WebSocket.CLOSING: state = 'closing'; break;
        case WebSocket.CLOSED: state = 'closed'; break;
      }
      if (state !== this._lastState) {
        this._emitStatus(state);
      }
    }, 1000);
  }

  disconnect() {
    console.log('Disconnecting WebSocket');
    this._shouldReconnect = false;
    this._cleanup();
  }
  
  _cleanup() {
    if (this.socket) {
      try { 
        this.socket.close(1000, 'Client disconnect'); 
      } catch(e) {
        console.warn('Error closing WebSocket:', e);
      }
      this.socket = null;
    }
    
    if (this._readyStateTimer) {
      clearInterval(this._readyStateTimer);
      this._readyStateTimer = null;
    }
    
    if (this._reconnectTimer) {
      clearTimeout(this._reconnectTimer);
      this._reconnectTimer = null;
    }
    
    this._stopPingTimer();
    this._stopPingTimeoutTimer();
  }

  // Message handling
  /**
   * @param {string} data
   */
  _handleMessage(data) {
    try {
      const parsed = JSON.parse(data);

      // Handle built-in message types
      if (parsed.type === 'pong') {
        this._lastPongTime = Date.now();
        this._consecutivePingFailures = 0; // Reset failure counter on successful pong
        this._stopPingTimeoutTimer(); // Clear timeout since we got a response

        // Reset reconnection attempts after connection has been stable for a while
        // Only reset if we've had multiple successful pongs (connection is truly stable)
        if (this.reconnectAttempts > 0) {
          // Wait for multiple successful pongs before considering connection stable
          this._stablePongCount = (this._stablePongCount || 0) + 1;
          if (this._stablePongCount >= 3) {
            console.log('Connection stable after multiple pongs - resetting reconnect attempts');
            this.reconnectAttempts = 0;
            this._stablePongCount = 0;
          }
        }

        if(import.meta.env.DEV) console.log('Received pong');
        return;
      }

      if (parsed.type === 'welcome') {
        console.log('Received welcome message:', parsed.message);
      }
      // Emit to handlers
      this._emit({ type: 'message', data: parsed, raw: data });
    } catch (error) {
      console.warn('Failed to parse WebSocket message as JSON:', error);
      // Emit raw data for non-JSON messages
      this._emit({ type: 'message', data, raw: data });
    }
  }
  
  // Ping/Pong mechanism
  _startPingTimer() {
    this._stopPingTimer();
    this._pingTimer = setInterval(() => {
      if (this.connected && this.socket && this.socket.readyState === WebSocket.OPEN) {
        const now = Date.now();
        
        // Check if we have too many consecutive ping failures
        if (this._consecutivePingFailures >= this._maxPingFailures) {
          console.warn(`Max ping failures reached (${this._maxPingFailures}) - connection appears dead`);
          this._forceConnectionClose('Ping timeout - max failures reached');
          return;
        }
        
        // Check if previous ping timed out (no pong received within timeout period)
        if (this._lastPingTime > 0 && this._lastPongTime < this._lastPingTime && 
            (now - this._lastPingTime) > this._pongTimeout) {
          console.warn('Previous ping timed out - incrementing failure count');
          this._consecutivePingFailures++;
        }
        
        // Send ping with timeout protection
        this._lastPingTime = now;
        const pingSuccess = this.send({ type: 'ping', timestamp: now });
        
        if (pingSuccess) {
          if(import.meta.env.DEV) console.log('Sent ping');
          // Start timeout timer for this ping
          this._startPingTimeoutTimer();
        } else {
          console.warn('Failed to send ping - connection may be broken');
          this._consecutivePingFailures++;
        }
      }
    }, this._pingInterval);
  }
  
  _stopPingTimer() {
    if (this._pingTimer) {
      clearInterval(this._pingTimer);
      this._pingTimer = null;
    }
  }
  
  _startPingTimeoutTimer() {
    this._stopPingTimeoutTimer();
    this._pingTimeoutTimer = setTimeout(() => {
      console.warn('Ping timeout - no pong received within timeout period');
      this._consecutivePingFailures++;
      
      // If we've reached max failures, close the connection
      if (this._consecutivePingFailures >= this._maxPingFailures) {
        console.warn('Max ping failures reached - closing connection');
        this._forceConnectionClose('Ping timeout - max failures reached');
      }
    }, this._pongTimeout);
  }
  
  _stopPingTimeoutTimer() {
    if (this._pingTimeoutTimer) {
      clearTimeout(this._pingTimeoutTimer);
      this._pingTimeoutTimer = null;
    }
  }
  
  // Force connection close when normal close doesn't work (e.g., offline IP)
  /**
   * @param {string | undefined} reason
   */
  _forceConnectionClose(reason) {
    console.log(`Force closing connection: ${reason}`);
    
    setTimeout(() => {
      if (this.connected) {
        this.connected = false;
        this._stopPingTimer();
        this._stopPingTimeoutTimer();
        this._stablePongCount = 0; // Reset stable connection counter
        this._emitStatus('closed');
        
        // Clean up socket reference
        this.socket = null;
        
        // Trigger reconnection with current backoff (maintains exponential backoff)
        if (this._shouldReconnect) {
          console.log(`Forced close detected (${reason}) - scheduling reconnection with backoff`);
          this._scheduleReconnect();
        }
      }
    }, 1000); // Give 1 second for normal close to work
  }
  
  // Public methods
  ping() {
    return this.send({ type: 'ping', timestamp: Date.now() });
  }
  
  requestStatus() {
    return this.send({ type: 'metrics' });
  }
  
  /**
   * @param {{ message?: string; timestamp: number; random?: number; test?: string; }} data
   */
  echo(data) {
    return this.send({ type: 'echo', data });
  }
  
  // Public getter for current state
  state() {
    return this._lastState;
  }
  
  // Get connection info
  getConnectionInfo() {
    return {
      connected: this.connected,
      state: this._lastState,
      url: this.url,
      reconnectAttempts: this.reconnectAttempts,
      maxReconnectAttempts: this.maxReconnectAttempts,
      lastPongTime: this._lastPongTime,
      lastPingTime: this._lastPingTime,
      consecutivePingFailures: this._consecutivePingFailures,
      maxPingFailures: this._maxPingFailures,
      backoffInitialDelay: this.backoffInitialDelay,
      backoffMaxDelay: this.backoffMaxDelay,
      backoffMultiplier: this.backoffMultiplier,
      backoffCurrentDelay: this.backoffCurrentDelay,
      backoffNextReconnectTime: this.backoffNextReconnectTime,
    };
  }
}

const ws = new WSService();
export default ws;
