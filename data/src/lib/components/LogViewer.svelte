<script lang="ts">
	import { onMount, onDestroy } from "svelte";
	import { fade } from "svelte/transition";
	import type { LogLevel } from "$lib/types/api";
	import ws from "$lib/services/ws.js";
	import { systemInfo } from "$lib/stores/system.svelte";
	import { clearLogs, logs } from "$lib/stores/logs.svelte";
	import VirtualList from "@humanspeak/svelte-virtual-list";

	let virtualListRef: ReturnType<typeof VirtualList>;
	let atBottom = $state(false);
	let lastScrollTop = $state(0);
	let scrollDirection = $state(0);
	let autoScrollActive = $state(true);
	let scrollDebounceTimer = $state<NodeJS.Timeout | null>(null);
	let previousLogCount = $state(0);
	let cumulativeScrollDistance = $state(0);
	let lastScrollDirection = $state(0);

	const SCROLL_THRESHOLD = 5;

	let searchText = $state("");
	let logLevels = $state<Record<LogLevel, boolean>>({
		ERROR: true,
		WARN: true,
		INFO: true,
		DEBUG: true,
		VERBOSE: true,
	});

	let currentTime = $state(new Date());

	let sys_log_level = $derived(systemInfo.log_level);
	function getLogLevelColor(level: LogLevel): string {
		const colors: Record<LogLevel, string> = {
			ERROR: "text-error",
			WARN: "text-warning",
			INFO: "text-info",
			DEBUG: "text-accent",
			VERBOSE: "text-secondary",
		};
		return colors[level] || "text-base-content";
	}

	/**
	 * Formats a timestamp into a relative time string.
	 * @param {string | number | Date} ts
	 */
	function formatRelativeTime(ts: string | number | Date) {
		const then = new Date(ts);
		const diffInSeconds = Math.floor(
			(currentTime.getTime() - then.getTime()) / 1000,
		);

		if (diffInSeconds < 5) return "just now";
		if (diffInSeconds < 60) return `${diffInSeconds}s ago`;
		if (diffInSeconds < 3600)
			return `${Math.floor(diffInSeconds / 60)}m ago`;
		if (diffInSeconds < 86400)
			return `${Math.floor(diffInSeconds / 3600)}h ago`;
		return then.toLocaleDateString();
	}

	function toggleLogLevel(level: LogLevel): void {
		logLevels[level] = !logLevels[level];
		localStorage.setItem("logviewer-levels", JSON.stringify(logLevels));
	}

	/**
	 * A computed property that filters logs based on search text and log level.
	 */
	let filteredLogs = $derived(() => {
		return logs.filter((log) => {
			const logLevel = log.level ? log.level.toUpperCase() : "UNKNOWN";

			const levelMatch =
				logLevel in logLevels && logLevels[logLevel as LogLevel];

			const searchMatch =
				!searchText ||
				log.msg.toLowerCase().includes(searchText.toLowerCase()) ||
				(log.tag &&
					log.tag.toLowerCase().includes(searchText.toLowerCase()));

			return levelMatch && searchMatch;
		});
	});

	$effect(() => {
		const currentLogCount = filteredLogs().length;
		if (currentLogCount > previousLogCount && autoScrollActive) {
			virtualListRef.scroll({
				index: currentLogCount - 1,
				smoothScroll: true,
				align: "bottom",
			});
		}
		previousLogCount = currentLogCount;
	});

	$effect(() => {
		if (scrollDirection) {
			autoScrollActive = false;
		}
	});

	/**
	 * Exports the currently filtered logs to a file.
	 */
	function exportLogs(format = "json") {
		const logsToExport = filteredLogs();
		let data;
		let filename;

		if (format === "json") {
			data = JSON.stringify(logsToExport, null, 2);
			filename = "logs.json";
		} else {
			data = logsToExport
				.map((log) => `[${log.localts}] [${log.level}] ${log.msg}`)
				.join("\n");
			filename = "logs.txt";
		}

		const blob = new Blob([data], {
			type: "text/plain",
		});
		const url = URL.createObjectURL(blob);
		const a = document.createElement("a");
		a.href = url;
		a.download = filename;
		document.body.appendChild(a);
		a.click();
		document.body.removeChild(a);
		URL.revokeObjectURL(url);
	}
	let timeUpdateInterval = $state<NodeJS.Timeout>();
  var resizeObserver : ResizeObserver;

	onMount(() => {
		const savedLogLevels = localStorage.getItem("logviewer-levels");
		if (savedLogLevels) {
			logLevels = JSON.parse(savedLogLevels);
		}
		timeUpdateInterval = setInterval(() => {
			currentTime = new Date();
		}, 1000);
		lastScrollTop =
			document.querySelector("#virtual-list-viewport")?.scrollTop || 0;
		const contentList = document.getElementById("virtual-list-content");
		if (contentList) {
			resizeObserver = new ResizeObserver(() => {
				atBottom =
					document.querySelector("#virtual-list-viewport")!
						.scrollTop >=
					document
						.querySelector("#virtual-list-content")!
						.getBoundingClientRect().height -
						document
							.querySelector("#virtual-list-container")!
							.getBoundingClientRect().height;
			});
			resizeObserver.observe(contentList);
		}
		document
			.querySelector("#virtual-list-viewport")
			?.addEventListener("scroll", (e) => {
				const target = e.target as HTMLDivElement;
				const scrollTop = target.scrollTop;
				const contentHeight = document
					.querySelector("#virtual-list-content")!
					.getBoundingClientRect().height;
				const containerHeight = document
					.querySelector("#virtual-list-container")!
					.getBoundingClientRect().height;
				atBottom = scrollTop >= contentHeight - containerHeight;
				const scrollDelta = lastScrollTop - scrollTop;
				const currentDirection =
					scrollDelta > 0 ? 1 : scrollDelta < 0 ? -1 : 0;

				if (
					currentDirection !== 0 &&
					currentDirection !== lastScrollDirection
				) {
					cumulativeScrollDistance = 0;
					lastScrollDirection = currentDirection;
				}

				if (currentDirection !== 0) {
					cumulativeScrollDistance += Math.abs(scrollDelta);
				}

				if (cumulativeScrollDistance > SCROLL_THRESHOLD) {
					if (scrollDebounceTimer) {
						clearTimeout(scrollDebounceTimer);
					}

					scrollDebounceTimer = setTimeout(() => {
						if (currentDirection === 1) {
							scrollDirection = 1; // Scrolling up
						} else if (currentDirection === -1) {
							scrollDirection = 0; // Scrolling down
						}
						cumulativeScrollDistance = 0;
					}, 30);
				}

				lastScrollTop = scrollTop;
			});
	});

	onDestroy(() => {
		if (timeUpdateInterval) {
			clearInterval(timeUpdateInterval);
		}
		if (scrollDebounceTimer) {
			clearTimeout(scrollDebounceTimer);
		}
    if (resizeObserver) {
      resizeObserver.disconnect();
    }
	});
</script>

<div class="h-full flex flex-col bg-base-200 text-base-content font-sans">
	<!-- Toolbar -->
	<div class="flex-shrink-0 bg-base-100 border-b border-base-300 p-2">
		<div class="flex items-center justify-between flex-wrap gap-2">
			<div class="flex items-center gap-4 justify-around flex-wrap">
				<div class="form-control">
					<input
						type="text"
						placeholder="Search…"
						class="input input-sm input-bordered w-48"
						bind:value={searchText}
						aria-label="Search logs"
					/>
				</div>
				<div class="form-control">
					<div
						class="flex items-center gap-2"
						role="group"
						aria-label="Log level filters"
					>
						<span class="label-text mr-2">Log Level: </span>
						<select
							class="select"
							value={String(sys_log_level)}
							onchange={(e) => {
								ws.send({
									type: "set_log_level",
									data: Number(
										(e.target as HTMLSelectElement).value,
									),
								});
							}}
						>
							<option value="0">NONE</option>
							<option value="1">ERROR</option>
							<option value="2">WARNING</option>
							<option value="3">INFO</option>
							<option value="4">DEBUG</option>
							<option value="5">VERBOSE</option>
						</select>
						<div
							class="tooltip tooltip-bottom"
							data-tip="Persists on reboot"
						>
							<svg
								xmlns="http://www.w3.org/2000/svg"
								fill="none"
								viewBox="0 0 24 24"
								stroke-width="1.5"
								stroke="currentColor"
								class="size-6"
							>
								<path
									stroke-linecap="round"
									stroke-linejoin="round"
									d="m11.25 11.25.041-.02a.75.75 0 0 1 1.063.852l-.708 2.836a.75.75 0 0 0 1.063.853l.041-.021M21 12a9 9 0 1 1-18 0 9 9 0 0 1 18 0Zm-9-3.75h.008v.008H12V8.25Z"
								/>
							</svg>
						</div>
					</div>
				</div>
			</div>

			<!-- Log Level Filter -->
			<div class="form-control">
				<div
					class="flex items-center gap-2"
					role="group"
					aria-label="Log level filters"
				>
					<span class="label-text mr-2">Filter levels:</span>
					{#each Object.keys(logLevels) as level}
						<button
							onclick={() => toggleLogLevel(level as LogLevel)}
							class="btn btn-xs"
							class:btn-accent={logLevels[level as LogLevel]}
							class:btn-outline={!logLevels[level as LogLevel]}
							aria-pressed={logLevels[level as LogLevel]}
						>
							{level}
						</button>
					{/each}
				</div>
			</div>
			<div
				class="flex justify-between items-center gap-2 flex-wrap w-full"
			>
				<div class="flex gap-2">
					<div class="form-control">
						<label class="label cursor-pointer p-0">
							<span class="label-text mr-2">Auto-scroll</span>
							<input
								type="checkbox"
								class="toggle toggle-sm"
								bind:checked={autoScrollActive}
								aria-label="Toggle auto-scroll"
							/>
						</label>
					</div>
				</div>
				<div class="flex gap-2">
					<button
						onclick={() => exportLogs()}
						class="btn btn-sm btn-info">Export</button
					>
					<button onclick={clearLogs} class="btn btn-sm btn-warning"
						>Clear Logs</button
					>
				</div>
			</div>
		</div>
	</div>

	<!-- Log Display -->
	<div class="bg-base-200 rounded-xl overflow-hidden flex-1">
		<div class="bg-base-200 rounded-xl p-2 h-full relative">
			<div class="log-container h-full overflow-y-auto font-mono text-sm">
				<VirtualList items={filteredLogs()} bind:this={virtualListRef}>
					{#snippet renderItem(item)}
						<div class="w-full mb-1 last:mb-0">
							<button
								type="button"
								class="flex items-center justify-between sm:gap-4 md:gap-6 p-4 cursor-pointer hover:bg-base-200 transition-colors bg-base-100 border border-base-300 rounded-md w-full"
								onclick={() => (item.expanded = !item.expanded)}
							>
								<div class="flex items-center">
									<div
										class="flex items-center justify-center mr-1"
									>
										<span
											class="transition-transform duration-200"
											class:rotate-90={item.expanded}
										>
											<svg
												xmlns="http://www.w3.org/2000/svg"
												viewBox="0 0 20 20"
												fill="currentColor"
												class="size-5"
											>
												<path
													fill-rule="evenodd"
													d="M8.22 5.22a.75.75 0 0 1 1.06 0l4.25 4.25a.75.75 0 0 1 0 1.06l-4.25 4.25a.75.75 0 0 1-1.06-1.06L11.94 10 8.22 6.28a.75.75 0 0 1 0-1.06Z"
													clip-rule="evenodd"
												/>
											</svg>
										</span>
									</div>
									<div class="flex items-center">
										<span
											class="font-bold mr-2 {getLogLevelColor(
												item.level,
											)}">{item.level}</span
										>
									</div>
									<div class="flex max-sm:flex-col">
										<span
											class="text-base-content/90 mr-2 badge badge-dash"
											>{item.tag}</span
										>
										<span
											class="line-clamp-1 text-base-content/90 break-all wrap-anywhere"
											>{item.msg}</span
										>
									</div>
								</div>
								<div
									class="tooltip tooltip-left"
									data-tip={item.localts}
								>
									<span
										class="text-xs text-base-content/50 whitespace-wrap"
										>{formatRelativeTime(
											item.localts,
										)}</span
									>
								</div>
							</button>
							<div
								class="bg-base-100/50 w-full flex border-t border-base-300 transition-all duration-200 ease-in-out rounded-b-md"
								class:max-h-96={item.expanded}
								class:opacity-100={item.expanded}
								class:max-h-0={!item.expanded}
								class:opacity-0={!item.expanded}
								style="overflow: hidden;"
							>
								<div class="flex flex-col p-4">
									<p
										class="font-mono text-xs wrap-anywhere text-wrap break-all"
									>
										<strong>RTC Timestamp:</strong>
										{item.ts}
									</p>
									<p
										class="font-mono text-xs wrap-anywhere text-wrap break-all"
									>
										<strong>Boot Timestamp:</strong>
										{item.uptime}
									</p>
									<div
										class="mt-2 text-xs whitespace-pre-wrap break-all"
									>
										{item.msg}
									</div>
								</div>
							</div>
						</div>
					{/snippet}
				</VirtualList>
			</div>
			{#if !atBottom && !autoScrollActive}
				<div
					class="absolute bottom-4 right-8 z-10"
					transition:fade={{ duration: 200 }}
				>
					<button
						onclick={async () => {
							await virtualListRef.scroll({
								index: filteredLogs().length - 1,
							});
							autoScrollActive = true;
						}}
						class="btn btn-circle btn-accent shadow-lg btn-xl"
						aria-label="Scroll to bottom"
					>
						<svg
							xmlns="http://www.w3.org/2000/svg"
							fill="none"
							viewBox="0 0 24 24"
							stroke-width="1.5"
							stroke="currentColor"
							class="size-6"
						>
							<path
								stroke-linecap="round"
								stroke-linejoin="round"
								d="M3 4.5h14.25M3 9h9.75M3 13.5h9.75m4.5-4.5v12m0 0-3.75-3.75M17.25 21 21 17.25"
							/>
						</svg>
					</button>
				</div>
			{/if}
		</div>
	</div>
</div>
