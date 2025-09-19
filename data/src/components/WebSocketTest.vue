<template>
  <div class="card bg-base-200 shadow-xl">
    <div class="card-body">
      <h2 class="card-title">WebSocket Test Console</h2>

      <!-- Connection Status -->
      <div class="stats shadow mb-4 bg-base-100">
        <div class="stat">
          <div class="stat-title">Connection Status</div>
          <div class="stat-value text-sm" :class="statusColor">{{ wsState }}</div>
          <div class="stat-desc">{{ wsConnected ? 'Connected' : 'Disconnected' }}</div>
        </div>
        <div class="stat">
          <div class="stat-title">Reconnect Attempts</div>
          <div class="stat-value text-sm">{{ wsInfo.reconnectAttempts || 0 }}/{{ wsInfo.maxReconnectAttempts || 0 }}
          </div>
        </div>
      </div>

      <!-- Test Buttons -->
      <div class="flex flex-wrap gap-2 mb-4">
        <button @click="sendPing" class="btn btn-primary btn-sm" :disabled="!wsConnected">
          Send Ping
        </button>
        <button @click="requestStatus" class="btn btn-secondary btn-sm" :disabled="!wsConnected">
          Request Status
        </button>
        <button @click="sendEcho" class="btn btn-accent btn-sm" :disabled="!wsConnected">
          Send Echo
        </button>
        <button @click="reconnect" class="btn btn-warning btn-sm">
          Reconnect
        </button>
        <button @click="clearMessages" class="btn btn-ghost btn-sm">
          Clear Log
        </button>
      </div>

      <!-- Custom Message -->
      <div class="form-control mb-4">
        <label class="label">
          <span class="label-text">Send Custom Message</span>
        </label>
        <div class="join">
          <input v-model="customMessage" type="text" placeholder="Enter JSON message..."
            class="input input-bordered join-item flex-1" @keyup.enter="sendCustomMessage" :disabled="!wsConnected" />
          <button @click="sendCustomMessage" class="btn btn-primary join-item" :disabled="!wsConnected">
            Send
          </button>
        </div>
      </div>

      <!-- Message Log -->
      <div class="card bg-base-100">
        <div class="form-control p-2">
          <label class="label">
            <span class="label-text">Message Log ({{ messages.length }} messages)</span>
          </label>
          <div class="bg-base-100 p-2 rounded-lg h-64 overflow-y-auto font-mono text-xs">
            <div v-if="messages.length === 0" class="text-base-content/50 text-center py-8">
              No messages yet. Try sending a test message!
            </div>
            <div v-for="(msg, index) in messages" :key="index"
              class="mb-2 pb-2 border-b border-base-300 last:border-b-0">
              <div class="flex justify-between items-start mb-1">
                <span class="badge badge-sm" :class="msg.type === 'sent' ? 'badge-primary' : 'badge-secondary'">
                  {{ msg.type }}
                </span>
                <span class="text-xs opacity-60">{{ formatTime(msg.timestamp) }}</span>
              </div>
              <pre class="whitespace-pre-wrap break-all">{{ msg.content }}</pre>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'WebSocketTest',
  data() {
    return {
      messages: [],
      customMessage: '',
      wsState: 'connecting',
      wsConnected: false,
      wsInfo: {},
      _wsOff: null,
      _updateTimer: null
    };
  },
  computed: {
    statusColor() {
      switch (this.wsState) {
        case 'open': return 'text-success';
        case 'closed': return 'text-error';
        case 'failed': return 'text-error';
        case 'error': return 'text-warning';
        case 'connecting': return 'text-info';
        default: return 'text-base-content';
      }
    }
  },
  methods: {
    updateWebSocketInfo() {
      if (this.$ws && typeof this.$ws.getConnectionInfo === 'function') {
        this.wsInfo = this.$ws.getConnectionInfo();
        this.wsConnected = this.wsInfo.connected || false;
        this.wsState = this.$ws.state() || 'unknown';
      }
    },

    addMessage(type, content) {
      this.messages.unshift({
        type,
        content: typeof content === 'string' ? content : JSON.stringify(content, null, 2),
        timestamp: Date.now()
      });

      // Keep only last 100 messages
      if (this.messages.length > 100) {
        this.messages = this.messages.slice(0, 100);
      }
    },

    sendPing() {
      if (this.$ws && this.$ws.ping()) {
        this.addMessage('sent', { type: 'ping', timestamp: Date.now() });
      }
    },

    requestStatus() {
      if (this.$ws && this.$ws.requestStatus()) {
        this.addMessage('sent', { type: 'metrics' });
      }
    },

    sendEcho() {
      const data = {
        message: 'Hello from WebSocket test!',
        timestamp: Date.now(),
        random: Math.random()
      };
      if (this.$ws && this.$ws.echo(data)) {
        this.addMessage('sent', { type: 'echo', data });
      }
    },

    sendCustomMessage() {
      if (!this.customMessage.trim()) return;

      try {
        const parsed = JSON.parse(this.customMessage);
        if (this.$ws && this.$ws.send(parsed)) {
          this.addMessage('sent', parsed);
          this.customMessage = '';
        }
      } catch (error) {
        // Try sending as plain text
        if (this.$ws && this.$ws.send(this.customMessage)) {
          this.addMessage('sent', this.customMessage);
          this.customMessage = '';
        }
      }
    },

    reconnect() {
      if (this.$ws) {
        this.addMessage('sent', 'Reconnecting...');
        this.$ws.disconnect();
        setTimeout(() => {
          this.$ws.connect();
        }, 1000);
      }
    },

    clearMessages() {
      this.messages = [];
    },

    formatTime(timestamp) {
      return new Date(timestamp).toLocaleTimeString();
    }
  },

  mounted() {
    this.updateWebSocketInfo();

    // Listen for WebSocket events
    this._wsOff = this.$ws?.on((evt) => {
      if (evt.type === 'status') {
        this.updateWebSocketInfo();
      } else if (evt.type === 'message') {
        this.addMessage('received', evt.data);
      }
    }) || null;

    // Update connection info periodically
    this._updateTimer = setInterval(() => {
      this.updateWebSocketInfo();
    }, 1000);

    // Add some example messages
    this.customMessage = '{"type": "ping"}';
  },

  beforeUnmount() {
    if (this._wsOff) {
      try { this._wsOff(); } catch (_) { }
      this._wsOff = null;
    }
    if (this._updateTimer) {
      clearInterval(this._updateTimer);
      this._updateTimer = null;
    }
  }
};
</script>
