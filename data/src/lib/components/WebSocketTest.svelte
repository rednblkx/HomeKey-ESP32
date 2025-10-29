<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import ws from '$lib/services/ws.js';

	/**
	 * WebSocket event types emitted by the WS service.
	 */
	type WebSocketEvent =
		| { type: 'status'; state: string }
		| { type: 'message'; data: unknown; raw: string };

	/**
	 * Connection information structure returned by WS service.
	 */
	interface ConnectionInfo {
		connected: boolean;
		state: string;
		url: string;
		reconnectAttempts: number;
		maxReconnectAttempts: number;
		lastPongTime: number;
		lastPingTime: number;
		consecutivePingFailures: number;
		maxPingFailures: number;
	}

	/**
	 * Message structure used internally by the component for display.
	 */
	interface ComponentMessage {
		type: 'sent' | 'received';
		content: string;
		timestamp: number;
	}

	/**
	 * WebSocket state values.
	 */
	type WebSocketState = 'connecting' | 'open' | 'closed' | 'failed' | 'error' | 'unknown';

	let messages: ComponentMessage[] = $state([]);
	let customMessage: string = $state('');
	let wsState: WebSocketState = $state('connecting');
	let wsConnected: boolean = $state(false);
	let wsInfo: ConnectionInfo = $state({
		connected: false,
		reconnectAttempts: 0,
		maxReconnectAttempts: 0,
		url: '',
		state: 'disconnected',
		lastPongTime: 0,
		lastPingTime: 0,
		consecutivePingFailures: 0,
		maxPingFailures: 0
	});
	let wsOff: (() => void) | null = $state(null);
	let updateTimer: ReturnType<typeof setInterval> | null = $state(null);

	let statusColor = $derived((): string => {
		switch (wsState) {
			case 'open': return 'text-success';
			case 'closed': return 'text-error';
			case 'failed': return 'text-error';
			case 'error': return 'text-warning';
			case 'connecting': return 'text-info';
			default: return 'text-base-content';
		}
	});

	/**
	 * Updates WebSocket connection information from the WS service.
	 * Handles potential errors when accessing WS service methods.
	 */
	function updateWebSocketInfo(): void {
		try {
			if (ws && typeof ws.getConnectionInfo === 'function') {
				const info = ws.getConnectionInfo();
				if (info && typeof info === 'object') {
					wsInfo = info as ConnectionInfo;
					wsConnected = wsInfo.connected || false;
					wsState = (ws.state() as WebSocketState) || 'unknown';
				}
			}
		} catch (error) {
			console.error('Error updating WebSocket info:', error);
			wsState = 'error';
			wsConnected = false;
		}
	}

	/**
	 * Adds a message to the component's message log.
	 * @param type - The type of message ('sent' or 'received')
	 * @param content - The message content (string or object to be JSON stringified)
	 */
	function addMessage(type: 'sent' | 'received', content: unknown): void {
		try {
			const messageContent = typeof content === 'string' ? content : JSON.stringify(content, null, 2);
			messages.unshift({
				type,
				content: messageContent,
				timestamp: Date.now()
			});

			// Keep only last 100 messages
			if (messages.length > 100) {
				messages = messages.slice(0, 100);
			}
		} catch (error) {
			console.error('Error adding message to log:', error);
			messages.unshift({
				type: 'received',
				content: `Error adding message: ${error instanceof Error ? error.message : 'Unknown error'}`,
				timestamp: Date.now()
			});
		}
	}

	/**
	 * Sends a ping message via WebSocket.
	 * Includes error handling for failed ping attempts.
	 */
	function sendPing(): void {
		try {
			if (ws && typeof ws.ping === 'function') {
				const success = ws.ping();
				if (success) {
					addMessage('sent', { type: 'ping', timestamp: Date.now() });
				} else {
					addMessage('received', 'Failed to send ping - WebSocket not connected');
				}
			} else {
				addMessage('received', 'WebSocket service not available');
			}
		} catch (error) {
			console.error('Error sending ping:', error);
			addMessage('received', `Error sending ping: ${error instanceof Error ? error.message : 'Unknown error'}`);
		}
	}

	/**
	 * Requests status/metrics from the WebSocket server.
	 * Includes error handling for failed requests.
	 */
	function requestStatus(): void {
		try {
			if (ws && typeof ws.requestStatus === 'function') {
				const success = ws.requestStatus();
				if (success) {
					addMessage('sent', { type: 'metrics' });
				} else {
					addMessage('received', 'Failed to request status - WebSocket not connected');
				}
			} else {
				addMessage('received', 'WebSocket service not available');
			}
		} catch (error) {
			console.error('Error requesting status:', error);
			addMessage('received', `Error requesting status: ${error instanceof Error ? error.message : 'Unknown error'}`);
		}
	}

	/**
	 * Sends an echo message with test data via WebSocket.
	 * Includes error handling for failed echo attempts.
	 */
	function sendEcho(): void {
		try {
			const data = {
				message: 'Hello from WebSocket test!',
				timestamp: Date.now(),
				random: Math.random()
			};

			if (ws && typeof ws.echo === 'function') {
				const success = ws.echo(data);
				if (success) {
					addMessage('sent', { type: 'echo', data });
				} else {
					addMessage('received', 'Failed to send echo - WebSocket not connected');
				}
			} else {
				addMessage('received', 'WebSocket service not available');
			}
		} catch (error) {
			console.error('Error sending echo:', error);
			addMessage('received', `Error sending echo: ${error instanceof Error ? error.message : 'Unknown error'}`);
		}
	}

	/**
	 * Sends a custom message entered by the user.
	 * Attempts JSON parsing first, falls back to plain text.
	 * Includes comprehensive error handling.
	 */
	function sendCustomMessage(): void {
		if (!customMessage.trim()) return;

		try {
			let parsed: unknown;
			try {
				parsed = JSON.parse(customMessage);
			} catch {
				// If JSON parsing fails, treat as plain text
				parsed = customMessage;
			}

			if (ws && typeof ws.send === 'function') {
				const success = ws.send(parsed as string | object);
				if (success) {
					addMessage('sent', parsed);
					customMessage = '';
				} else {
					addMessage('received', 'Failed to send custom message - WebSocket not connected');
				}
			} else {
				addMessage('received', 'WebSocket service not available');
			}
		} catch (error) {
			console.error('Error sending custom message:', error);
			addMessage('received', `Error sending custom message: ${error instanceof Error ? error.message : 'Unknown error'}`);
		}
	}

	/**
	 * Manually triggers WebSocket reconnection.
	 * Includes error handling for reconnection failures.
	 */
	function reconnect(): void {
		try {
			if (ws) {
				addMessage('sent', 'Reconnecting...');
				if (typeof ws.disconnect === 'function') {
					ws.disconnect();
				}
				setTimeout(() => {
					try {
						if (typeof ws.connect === 'function') {
							ws.connect();
						} else {
							addMessage('received', 'WebSocket connect method not available');
						}
					} catch (error) {
						console.error('Error during reconnection:', error);
						addMessage('received', `Reconnection failed: ${error instanceof Error ? error.message : 'Unknown error'}`);
					}
				}, 1000);
			} else {
				addMessage('received', 'WebSocket service not available');
			}
		} catch (error) {
			console.error('Error initiating reconnect:', error);
			addMessage('received', `Error initiating reconnect: ${error instanceof Error ? error.message : 'Unknown error'}`);
		}
	}

	/**
	 * Clears all messages from the component's message log.
	 */
	function clearMessages(): void {
		messages = [];
	}

	/**
	 * Formats a timestamp into a localized time string.
	 * @param timestamp - Unix timestamp in milliseconds
	 * @returns Formatted time string
	 */
	function formatTime(timestamp: number): string {
		try {
			return new Date(timestamp).toLocaleTimeString();
		} catch (error) {
			console.error('Error formatting timestamp:', error);
			return 'Invalid time';
		}
	}

	onMount(() => {
		try {
			updateWebSocketInfo();

			if (ws && typeof ws.on === 'function') {
				wsOff = ws.on((evt: WebSocketEvent) => {
					try {
						if (evt.type === 'status') {
							updateWebSocketInfo();
						} else if (evt.type === 'message') {
							addMessage('received', evt.data);
						}
					} catch (error) {
						console.error('Error handling WebSocket event:', error);
						addMessage('received', `Error handling WebSocket event: ${error instanceof Error ? error.message : 'Unknown error'}`);
					}
				});
			} else {
				console.warn('WebSocket service on method not available');
			}

			updateTimer = setInterval(() => {
				try {
					updateWebSocketInfo();
				} catch (error) {
					console.error('Error in periodic update:', error);
				}
			}, 1000);

			customMessage = '{"type": "ping"}';
		} catch (error) {
			console.error('Error in onMount:', error);
			addMessage('received', `Component initialization error: ${error instanceof Error ? error.message : 'Unknown error'}`);
		}
	});

	onDestroy(() => {
		try {
			if (wsOff) {
				try {
					wsOff();
				} catch (error) {
					console.warn('Error cleaning up WebSocket listener:', error);
				}
				wsOff = null;
			}
			if (updateTimer) {
				clearInterval(updateTimer);
				updateTimer = null;
			}
		} catch (error) {
			console.error('Error in onDestroy:', error);
		}
	});
</script>

<div class="card bg-base-200 shadow-xl">
	<div class="card-body">
		<h2 class="card-title">WebSocket Test Console</h2>

		<!-- Connection Status -->
		<div class="stats shadow mb-4 bg-base-100">
			<div class="stat">
				<div class="stat-title">Connection Status</div>
				<div class="stat-value text-sm {statusColor}">{wsState}</div>
				<div class="stat-desc">{wsConnected ? 'Connected' : 'Disconnected'}</div>
			</div>
			<div class="stat">
				<div class="stat-title">Reconnect Attempts</div>
				<div class="stat-value text-sm">{wsInfo.reconnectAttempts || 0}/{wsInfo.maxReconnectAttempts || 0}
				</div>
			</div>
		</div>

		<!-- Test Buttons -->
		<div class="flex flex-wrap gap-2 mb-4">
			<button onclick={sendPing} class="btn btn-primary btn-sm" disabled={!wsConnected}>
				Send Ping
			</button>
			<button onclick={requestStatus} class="btn btn-secondary btn-sm" disabled={!wsConnected}>
				Request Status
			</button>
			<button onclick={sendEcho} class="btn btn-accent btn-sm" disabled={!wsConnected}>
				Send Echo
			</button>
			<button onclick={reconnect} class="btn btn-warning btn-sm">
				Reconnect
			</button>
			<button onclick={clearMessages} class="btn btn-ghost btn-sm">
				Clear Log
			</button>
		</div>

		<!-- Custom Message -->
		<div class="form-control mb-4">
			<label class="label" for="custom-message-input">
				<span class="label-text">Send Custom Message</span>
			</label>
			<div class="join">
				<input id="custom-message-input" bind:value={customMessage} type="text" placeholder="Enter JSON message..."
					class="input input-bordered join-item flex-1" disabled={!wsConnected} onkeydown={(e) => e.key === 'Enter' && sendCustomMessage()} />
				<button onclick={sendCustomMessage} class="btn btn-primary join-item" disabled={!wsConnected}>
					Send
				</button>
			</div>
		</div>

		<!-- Message Log -->
		<div class="card bg-base-100">
			<div class="form-control p-2">
				<label class="label" for="message-log">
					<span class="label-text">Message Log ({messages.length} messages)</span>
				</label>
				<div id="message-log" class="bg-base-100 p-2 rounded-lg h-64 overflow-y-auto font-mono text-xs">
					{#if messages.length === 0}
						<div class="text-base-content/50 text-center py-8">
							No messages yet. Try sending a test message!
						</div>
					{:else}
						{#each messages as msg}
							<div class="mb-2 pb-2 border-b border-base-300 last:border-b-0">
								<div class="flex justify-between items-start mb-1">
									<span class="badge badge-sm" class:badge-primary={msg.type === 'sent'} class:badge-secondary={msg.type === 'received'}>
										{msg.type}
									</span>
									<span class="text-xs opacity-60">{formatTime(msg.timestamp)}</span>
								</div>
								<pre class="whitespace-pre-wrap break-all">{msg.content}</pre>
							</div>
						{/each}
					{/if}
				</div>
			</div>
		</div>
	</div>
</div>
