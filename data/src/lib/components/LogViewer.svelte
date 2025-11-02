<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import type { LogEntry, LogLevel, LogMessage } from '$lib/types/api';
	import ws from '$lib/services/ws.js';
    import { systemInfo } from '$lib/stores/system.svelte';

	const LOG_LEVELS = ['ERROR', 'WARN', 'INFO', 'DEBUG'] as const;
	type UsedLogLevel = typeof LOG_LEVELS[number];

	let searchText = $state('');
	let realtime = $state(true);
	let autoScroll = $state(true);
	let userHasScrolled = $state(false);
	let logLevels = $state<Record<UsedLogLevel, boolean>>({
		ERROR: true,
		WARN: true,
		INFO: true,
		DEBUG: true,
	});
	let logs = $state<LogEntry[]>([]);

	let currentTime = $state(new Date());

	let isMobile = $state(false);

	let touchStartY = $state(0);
	let touchEndY = $state(0);
	let isScrolling = $state(false);
  let sys_log_level = $derived(systemInfo.log_level);
	const SCROLL_EDGE_MARGIN = 16;
	const SMOOTH_SCROLL_DURATION = 300;

	function getLogLevelColor(level: LogLevel): string {
		const colors: Record<LogLevel, string> = {
			ERROR: 'text-error',
			WARN: 'text-warning',
			INFO: 'text-info',
			DEBUG: 'text-accent',
			VERBOSE: 'text-secondary',
			NONE: 'text-neutral',
		};
		return colors[level] || 'text-base-content';
	}

	/**
	 * Formats a timestamp into a relative time string.
	 * @param {string | number | Date} ts
	 */
	function formatRelativeTime(ts: string | number | Date) {
		const then = new Date(ts);
		const diffInSeconds = Math.floor((currentTime.getTime() - then.getTime()) / 1000);

		if (diffInSeconds < 5) return 'just now';
		if (diffInSeconds < 60) return `${diffInSeconds}s ago`;
		if (diffInSeconds < 3600) return `${Math.floor(diffInSeconds / 60)}m ago`;
		if (diffInSeconds < 86400) return `${Math.floor(diffInSeconds / 3600)}h ago`;
		return then.toLocaleDateString();
	}

	async function copyLog(log: LogEntry): Promise<void> {
		const logText = `[${log.localts}] [${log.level}] ${log.msg}`;
		try {
			await navigator.clipboard.writeText(logText);
		} catch (err) {
			console.error('Failed to copy log:', err);
		}
	}

	/**
	 * Clears all logs from the display.
	 */
	function clearLogs() {
		logs = [];
	}

	function toggleLogLevel(level: UsedLogLevel): void {
		logLevels[level] = !logLevels[level];
		localStorage.setItem('logviewer-levels', JSON.stringify(logLevels));
	}

	/**
	 * A computed property that filters logs based on search text and log level.
	 */
	let filteredLogs = $derived(() => {
		return logs.filter(log => {
			const logLevel = log.level ? log.level.toUpperCase() : 'UNKNOWN';

			const levelMatch = logLevel in logLevels && logLevels[logLevel as UsedLogLevel];

			const searchMatch = !searchText ||
				log.msg.toLowerCase().includes(searchText.toLowerCase()) ||
				(log.tag && log.tag.toLowerCase().includes(searchText.toLowerCase()));

			return levelMatch && searchMatch;
		});
	});

	/**
	 * Exports the currently filtered logs to a file.
	 */
	function exportLogs(format = 'json') {
		const logsToExport = filteredLogs();
		let data;
		let filename;

		if (format === 'json') {
			data = JSON.stringify(logsToExport, null, 2);
			filename = 'logs.json';
		} else {
			data = logsToExport.map((log) => `[${log.localts}] [${log.level}] ${log.msg}`).join('\n');
			filename = 'logs.txt';
		}

		const blob = new Blob([data], {
			type: 'text/plain'
		});
		const url = URL.createObjectURL(blob);
		const a = document.createElement('a');
		a.href = url;
		a.download = filename;
		document.body.appendChild(a);
		a.click();
		document.body.removeChild(a);
		URL.revokeObjectURL(url);
	}

	/**
	 * Enhanced smooth scroll with requestAnimationFrame fallback
	 */
	function smoothScrollToBottom(duration = SMOOTH_SCROLL_DURATION) {
		const container = document.querySelector('.log-container');
		if (!container || filteredLogs().length === 0) return;

		const startTime = performance.now();
		const startScrollTop = container.scrollTop;
		const targetScrollTop = container.scrollHeight - container.clientHeight;

		const scrollStep = (/** @type {number} */ currentTime: number) => {
			const elapsed = currentTime - startTime;
			const progress = Math.min(elapsed / duration, 1);

			// Easing function for smooth animation
			const easeInOutQuad = (/** @type {number} */ t: number) => t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;

			const currentScrollTop = startScrollTop + (targetScrollTop - startScrollTop) * easeInOutQuad(progress);
			container.scrollTop = currentScrollTop;

			if (progress < 1) {
				requestAnimationFrame(scrollStep);
			}
		};

		requestAnimationFrame(scrollStep);
	}

	/**
	 * Standard scroll to bottom
	 */
	function scrollToBottom() {
		const container = document.querySelector('.log-container');
		if (container && filteredLogs().length > 0) {
			if (isMobile) {
				// Use smooth scroll for mobile/touch devices
				smoothScrollToBottom();
			} else {
				// Use direct scroll for desktop
				container.scrollTop = container.scrollHeight;
			}
		}
	}

	/**
	 * Enhanced scroll detection with edge margins
	 */
	function handleScroll() {
		const container = document.querySelector('.log-container');
		if (!container) return;

		const scrollOffset = container.scrollTop;
		const scrollHeight = container.scrollHeight;
		const clientHeight = container.clientHeight;

		const edgeMargin = isMobile ? SCROLL_EDGE_MARGIN : 10;

		const isAtBottom = scrollHeight - scrollOffset <= clientHeight + edgeMargin;
		userHasScrolled = !isAtBottom;
	}

	function handleTouchStart(event: Event): void {
		if (!isMobile) return;
		const touchEvent = event as TouchEvent;
		touchStartY = touchEvent.touches[0].clientY;
		isScrolling = false;
	}

	function handleTouchMove(event: Event): void {
		if (!isMobile) return;
		const touchEvent = event as TouchEvent;
		touchEndY = touchEvent.touches[0].clientY;
		const touchDiff = touchStartY - touchEndY;

		if (Math.abs(touchDiff) > 10) {
			isScrolling = true;

			if (touchDiff > 0) {
				userHasScrolled = true;
			}
		}
	}

	function handleTouchEnd() {
		if (!isMobile) return;

		setTimeout(() => {
			isScrolling = false;
		}, 150);
	}

	/**
	 * Media query change handler
	 * @param {{ matches: boolean; }} e
	 */
	function handleMediaQueryChange(e: MediaQueryListEvent): void {
		isMobile = e.matches;

		if (autoScroll && !userHasScrolled) {
			setTimeout(() => {
				scrollToBottom();
			}, 0);
		}
	}

	$effect(() => {
		if (logs.length > 0 && autoScroll && !userHasScrolled) {
			setTimeout(scrollToBottom, 0);
		}
	});

	$effect(() => {
		if (autoScroll) {
			setTimeout(scrollToBottom, 0);
			userHasScrolled = false;
		}
	});

	let wsUnsubscribe = $state<(() => void) | null>(null);

	let timeUpdateInterval = $state<NodeJS.Timeout>();

	onMount(() => {
		const savedLogLevels = localStorage.getItem('logviewer-levels');
		if (savedLogLevels) {
			logLevels = JSON.parse(savedLogLevels);
		}

		const mobileMediaQuery = window.matchMedia('(max-width: 767px)');
		mobileMediaQuery.addEventListener("change",handleMediaQueryChange);
		isMobile = mobileMediaQuery.matches;

		const container = document.querySelector('.log-container');
		if (container) {
			container.addEventListener('scroll', handleScroll);
			container.addEventListener('touchstart', handleTouchStart, { passive: true });
			container.addEventListener('touchmove', handleTouchMove, { passive: true });
			container.addEventListener('touchend', handleTouchEnd, { passive: true });
		}

		if (ws) {
			const handleWebSocketMessage = (event: { data: LogMessage; }) => {
				if (!realtime) return;
				try {
					let messageData = event.data;

					if (messageData && (messageData.type === 'log' || messageData.level || messageData.msg)) {
						const log : LogEntry = {
							id: Date.now() + Math.floor(Math.random() * 1234),
							localts: new Date().toISOString(),
							expanded: false,
							...messageData
						};

						if (log.msg && log.level) {
							logs.push(log);
						} else {
							console.warn('Invalid log message structure:', messageData);
						}
					}
				} catch (e) {
					console.error('Error handling WebSocket message:', e);
					try {
						logs.push({
							type: 'log',
							level: 'ERROR',
							msg: `WebSocket message processing error: ${e instanceof Error ? e.message : String(e)}`,
							tag: 'WS_ERROR',
							ts: new Date().toISOString(),
							id: Date.now() + Math.floor(Math.random() * 1234),
							localts: new Date().toISOString(),
							expanded: false,
						});
					} catch (logError) {
						console.error('Failed to log WebSocket error:', logError);
					}
				}
			};
			wsUnsubscribe = ws.on ? (ws.on(handleWebSocketMessage) as (() => void) | null) : null;
		timeUpdateInterval = setInterval(() => {
			currentTime = new Date();
		}, 1000);

		}
	});

	onDestroy(() => {
		const mobileMediaQuery = window.matchMedia('(max-width: 767px)');
		mobileMediaQuery.removeEventListener("change",handleMediaQueryChange);

		const container = document.querySelector('.log-container');
		if (container) {
			container.removeEventListener('scroll', handleScroll);
			container.removeEventListener('touchstart',handleTouchStart);
			container.removeEventListener('touchmove',handleTouchMove);
			container.removeEventListener('touchend', handleTouchEnd);
		}

		if (wsUnsubscribe && typeof wsUnsubscribe === 'function') {
			wsUnsubscribe();
		}

		if (timeUpdateInterval) {
			clearInterval(timeUpdateInterval);
		}
	});
</script>

<div class="h-full flex flex-col bg-base-200 text-base-content font-sans">
	<!-- Toolbar -->
	<div class="flex-shrink-0 bg-base-100 border-b border-base-300 p-2">
		<div class="flex items-center justify-between flex-wrap gap-2">
			<div class="flex items-center gap-4 flex-wrap justify-center">
				<div class="form-control">
					<input type="text" placeholder="Search…" class="input input-sm input-bordered w-48" bind:value={searchText}
						aria-label="Search logs" />
				</div>
				<!-- Log Level Filter -->
				<div class="form-control">
					<div class="flex items-center gap-2" role="group" aria-label="Log level filters">
            <span class="label-text mr-2">Filter levels:</span>
						{#each LOG_LEVELS as level}
							<button onclick={() => toggleLogLevel(level)}
								class="btn btn-xs" class:btn-accent={logLevels[level]} class:btn-outline={!logLevels[level]} aria-pressed={logLevels[level]}>
								{level}
							</button>
						{/each}
					</div>
				</div>
        <div class="form-control"> 
					<div class="flex items-center gap-2" role="group" aria-label="Log level filters">
            <span class="label-text mr-2">Log Level: </span>
            <select class="select" value={String(sys_log_level)} onchange={(e) => {ws.send({"type": "set_log_level", "data": Number((e.target as HTMLSelectElement).value)})}}>
              <option value="1">ERROR</option>
              <option value="2">WARNING</option>
              <option value="3">INFO</option>
              <option value="4">DEBUG</option>
            </select>
					</div>
        </div>
			</div>

			<div class="flex items-center gap-2 flex-wrap">
				<div class="flex gap-2">
					<div class="form-control">
						<label class="label cursor-pointer p-0">
							<span class="label-text mr-2">Real-time</span>
							<input type="checkbox" class="toggle toggle-sm" bind:checked={realtime}
								aria-label="Toggle real-time log updates" />
						</label>
					</div>
					<div class="form-control">
						<label class="label cursor-pointer p-0">
							<span class="label-text mr-2">Auto-scroll</span>
							<input type="checkbox" class="toggle toggle-sm" bind:checked={autoScroll} aria-label="Toggle auto-scroll" />
						</label>
					</div>
				</div>
				<div class="flex gap-2">
					<button onclick={() => exportLogs()} class="btn btn-sm btn-primary">Export</button>
					<button onclick={clearLogs} class="btn btn-sm btn-error">Clear Logs</button>
				</div>
			</div>
		</div>
	</div>

	<!-- Log Display -->
	<div class="bg-base-200 rounded-xl overflow-hidden flex-1">
		<div class="bg-base-200 rounded-xl p-2 h-full">
			<div class="log-container h-full overflow-y-auto font-mono text-sm">
				{#each filteredLogs() as item (item.id)}
					<div class="w-full mb-1 last:mb-0">
						<button
              type="button"
              class="flex items-center justify-between sm:gap-4 md:gap-6 p-4 cursor-pointer hover:bg-base-200 transition-colors bg-base-100 border border-base-300 rounded-md"
							onclick={() => item.expanded = !item.expanded}
							>
							<div class="flex items-center">
								<div class="flex items-center justify-center mr-1">
									<span class="transition-transform duration-200" class:rotate-90={item.expanded}>
										<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 20 20" fill="currentColor" class="size-5">
											<path fill-rule="evenodd" d="M8.22 5.22a.75.75 0 0 1 1.06 0l4.25 4.25a.75.75 0 0 1 0 1.06l-4.25 4.25a.75.75 0 0 1-1.06-1.06L11.94 10 8.22 6.28a.75.75 0 0 1 0-1.06Z" clip-rule="evenodd" />
										</svg>
									</span>
								</div>
								<div class="flex items-center">
									<span class="font-bold mr-2 {getLogLevelColor(item.level)}">{item.level}</span>
								</div>
								<div class="flex max-sm:flex-col">
									<span class="text-base-content/90 mr-2 badge badge-dash">{item.tag}</span>
									<span class="line-clamp-1 text-base-content/90 break-all wrap-anywhere">{item.msg}</span>
								</div>
							</div>
							<div class="tooltip tooltip-left" data-tip={item.localts}>
								<span class="text-xs text-base-content/50 whitespace-wrap">{formatRelativeTime(item.localts)}</span>
							</div>
						</button>
						<div
							class="bg-base-100/50 w-full flex border-t border-base-300 transition-all duration-200 ease-in-out rounded-b-md"
							class:max-h-96={item.expanded} class:opacity-100={item.expanded} class:max-h-0={!item.expanded} class:opacity-0={!item.expanded}
							style="overflow: hidden;"
						>
							<div class="flex flex-col p-4">
								<p class="font-mono text-xs wrap-anywhere text-wrap break-all"><strong>Device Timestamp:</strong> {item.ts}</p>
								<div class="mt-2 text-xs whitespace-pre-wrap break-all">{item.msg}</div>
								<button onclick={() => copyLog(item)} class="btn btn-xs btn-soft btn-secondary mt-2"
									aria-label="Copy log message">
									📋
								</button>
							</div>
						</div>
					</div>
				{/each}
			</div>
		</div>
	</div>
</div>

<style>
	.log-container {
		scroll-behavior: smooth;
		-webkit-overflow-scrolling: touch;
		padding: 0 16px;
		margin: 0 -16px;
	}

	.log-container::-webkit-scrollbar {
		width: 6px;
	}

	.log-container::-webkit-scrollbar-track {
		background: rgba(0,0,0,0.1);
		border-radius: 3px;
	}

	.log-container::-webkit-scrollbar-thumb {
		background: rgba(255,255,255,0.3);
		border-radius: 3px;
	}

	.log-container::-webkit-scrollbar-thumb:hover {
		background: rgba(255,255,255,0.5);
	}
</style>
