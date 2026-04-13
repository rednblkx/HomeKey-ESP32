<script lang="ts">
  import { onMount, onDestroy, untrack } from 'svelte';
  import { fade } from 'svelte/transition';
  import type { LogLevel } from '$lib/types/api';
  import ws from '$lib/services/ws.js';
  import { systemInfo } from '$lib/stores/system.svelte';
  import { clearLogs, logs } from '$lib/stores/logs.svelte';
  import { createVirtualizer } from '@tanstack/svelte-virtual';

  let scrollEl = $state<HTMLDivElement | null>(null);
  let virtualizer = $state<ReturnType<
    typeof createVirtualizer<HTMLDivElement, Element>
  > | null>(null);

  let autoScrollActive = $state(true);
  let previousLogCount = $state(0);
  let isProgrammaticScroll = false;
  let lastScrollTop = 0;
  let isInitialized = false;

  let atBottom = $derived(() => {
    if (!virtualizer) return true;
    const lastItem = $virtualizer?.getVirtualItems().at(-1);
    if (!lastItem) return true;
    return lastItem.index >= filteredCount() - 1;
  });

  let searchText = $state('');
  let logLevels = $state<Record<LogLevel, boolean>>({
    ERROR: true,
    WARN: true,
    INFO: true,
    DEBUG: true,
    VERBOSE: true,
  });

  let currentTime = $state(new Date());
  let timeUpdateInterval = $state<NodeJS.Timeout>();

  let sys_log_level = $derived(() => String(systemInfo.log_level));

  function getLogLevelColor(level: LogLevel): string {
    const colors: Record<LogLevel, string> = {
      ERROR: 'text-error',
      WARN: 'text-warning',
      INFO: 'text-info',
      DEBUG: 'text-accent',
      VERBOSE: 'text-secondary',
    };
    return colors[level] || 'text-base-content';
  }

  function getLogLevelBadgeStyle(level: LogLevel, active: boolean): string {
    if (!active) return 'bg-base-300/50 text-base-content/50';
    const styles: Record<LogLevel, string> = {
      ERROR: 'bg-error text-error-content',
      WARN: 'bg-warning text-warning-content',
      INFO: 'bg-info text-info-content',
      DEBUG: 'bg-teal-600 text-white',
      VERBOSE: 'bg-teal-700 text-white',
    };
    return styles[level] || 'bg-base-300 text-base-content';
  }

  function formatRelativeTime(ts: string | number | Date) {
    const then = new Date(ts);
    const diffInSeconds = Math.floor(
      (currentTime.getTime() - then.getTime()) / 1000,
    );
    if (diffInSeconds < 5) return 'just now';
    if (diffInSeconds < 60) return `${diffInSeconds}s ago`;
    if (diffInSeconds < 3600) return `${Math.floor(diffInSeconds / 60)}m ago`;
    if (diffInSeconds < 86400)
      return `${Math.floor(diffInSeconds / 3600)}h ago`;
    return then.toLocaleDateString();
  }

  function toggleLogLevel(level: LogLevel): void {
    logLevels[level] = !logLevels[level];
    localStorage.setItem('logviewer-levels', JSON.stringify(logLevels));
  }

  let filteredLogs = $derived(() =>
    logs.filter((log) => {
      const logLevel = log.level ? log.level.toUpperCase() : 'UNKNOWN';
      const levelMatch =
        logLevel in logLevels && logLevels[logLevel as LogLevel];
      const searchMatch =
        !searchText ||
        log.msg.toLowerCase().includes(searchText.toLowerCase()) ||
        (log.tag && log.tag.toLowerCase().includes(searchText.toLowerCase()));
      return levelMatch && searchMatch;
    }),
  );

  let filteredCount = $derived(() => filteredLogs().length);

  const virtualizerOptions = {
    getScrollElement: () => scrollEl,
    estimateSize: () => 46
  };

  function doScroll(index: number, behavior: ScrollBehavior = 'smooth') {
    isProgrammaticScroll = true;
    $virtualizer?.scrollToIndex(index, { align: 'end', behavior });
    setTimeout(() => {
      isProgrammaticScroll = false;
    }, 100);
  }

  function onScroll(e: Event) {
      if (isProgrammaticScroll) {
          lastScrollTop = (e.target as HTMLDivElement).scrollTop;
          return;
      }
      const scrollTop = (e.target as HTMLDivElement).scrollTop;
      const el = e.target as HTMLDivElement;

      if (scrollTop < lastScrollTop) {
          autoScrollActive = false;
      } else if (scrollTop + el.clientHeight >= el.scrollHeight - 4) {
          autoScrollActive = true;
      }

      lastScrollTop = scrollTop;
  }

  $effect(() => {
      if (!scrollEl || virtualizer) return;

      virtualizer = createVirtualizer<HTMLDivElement, Element>({
          count: filteredCount(),
          ...virtualizerOptions,
      });
  });

  $effect(() => {
    if (!scrollEl) return;
    scrollEl.addEventListener('scroll', onScroll);
    return () => scrollEl?.removeEventListener('scroll', onScroll);
  });

  $effect(() => {
    const count = filteredCount();
    if (!virtualizer) return;
    untrack(() => {
      $virtualizer?.setOptions({ count, ...virtualizerOptions });
      $virtualizer?._willUpdate();
    });
  });

  $effect(() => {
    const count = filteredCount();
    if (!autoScrollActive) {
      previousLogCount = count;
      return;
    }
    if (count > previousLogCount && virtualizer) {
      const isInitialScroll = previousLogCount === 0;

      if (!isInitialScroll && isInitialized) {
        doScroll(count - 1);
      }
    }
    previousLogCount = count;
  });

  $effect(() => {
    if (isInitialized || !virtualizer) return;
    const totalSize = $virtualizer?.getTotalSize() || 0;
    if (totalSize > 0) {
      isInitialized = true;
      doScroll(filteredCount() - 1);
    }
  });

  function scrollToBottom() {
    const count = filteredCount();
    if (virtualizer && count > 0) {
      doScroll(count - 1);
      autoScrollActive = true;
    }
  }

  function exportLogs(format = 'json') {
    const logsToExport = filteredLogs();
    let data: string;
    let filename: string;

    if (format === 'json') {
      data = JSON.stringify(logsToExport, null, 2);
      filename = 'logs.json';
    } else {
      data = logsToExport
        .map((log) => `[${log.localts}] [${log.level}] ${log.msg}`)
        .join('\n');
      filename = 'logs.txt';
    }

    const blob = new Blob([data], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = filename;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
  }

  function measureEl(el: HTMLElement) {
    $virtualizer?.measureElement(el);
    return { destroy() {} };
  }

  onMount(() => {
    const savedLogLevels = localStorage.getItem('logviewer-levels');
    if (savedLogLevels) {
      logLevels = JSON.parse(savedLogLevels);
    }
    timeUpdateInterval = setInterval(() => {
      currentTime = new Date();
    }, 1000);
  });

  onDestroy(() => {
    if (timeUpdateInterval) clearInterval(timeUpdateInterval);
  });
</script>

<div class="h-full flex flex-col text-base-content">
  <!-- Header -->
  <div class="pt-4 pb-2">
    <h1 class="text-xl font-semibold">System Logs</h1>
    <p class="text-sm text-base-content/60">Real-time device logs via WebSocket.</p>
  </div>

  <!-- Toolbar Card -->
  <div class="pb-4">
    <div class="bg-base-200 rounded-lg border border-base-300 p-4">
      <!-- Row 1: Search and Log Level -->
      <div class="flex flex-col sm:flex-row gap-3 mb-4">
        <div class="join flex-1">
          <div class="join-item bg-base-200 border border-base-300 border-r-0 px-3 flex items-center">
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-4 text-base-content/50">
              <path stroke-linecap="round" stroke-linejoin="round" d="m21 21-5.197-5.197m0 0A7.5 7.5 0 1 0 5.196 5.196a7.5 7.5 0 0 0 10.607 10.607Z" />
            </svg>
          </div>
          <input
            type="text"
            placeholder="Search logs..."
            class="input input-sm join-item flex-1 bg-base-100 border-base-300 focus:outline-none"
            bind:value={searchText}
            aria-label="Search logs"
          />
        </div>
        <div class="flex items-center gap-2 shrink-0">
          <span class="text-sm whitespace-nowrap">Log Level:</span>
          <select
            class="select select-sm bg-base-100 border-base-300 text-sm min-w-[100px]"
            value={sys_log_level()}
            onchange={(e) => {
              ws.send({
                type: 'set_log_level',
                data: Number((e.target as HTMLSelectElement).value),
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
        </div>
      </div>

      <!-- Row 2: Filter Levels and Actions -->
      <div class="flex flex-col sm:flex-row items-start sm:items-center justify-between gap-3">
        <div class="flex items-center gap-2 flex-wrap">
          <span class="text-sm text-base-content/60 mr-1">Filter levels:</span>
          {#each Object.keys(logLevels) as level}
            <button
              onclick={() => toggleLogLevel(level as LogLevel)}
              class="badge badge-sm cursor-pointer border-0 transition-all duration-200 {getLogLevelBadgeStyle(level as LogLevel, logLevels[level as LogLevel])}"
              aria-pressed={logLevels[level as LogLevel]}
            >
              {level}
            </button>
          {/each}
        </div>

        <div class="flex items-center gap-2 flex-wrap">
          <label class="label cursor-pointer gap-2 py-0">
            <input
              type="checkbox"
              class="toggle toggle-sm toggle-accent"
              bind:checked={autoScrollActive}
              aria-label="Toggle auto-scroll"
            />
            <span class="label-text text-sm">Auto-scroll</span>
          </label>
          <button onclick={() => exportLogs()} class="btn btn-sm btn-outline gap-1">
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-4">
              <path stroke-linecap="round" stroke-linejoin="round" d="M3 16.5v2.25A2.25 2.25 0 0 0 5.25 21h13.5A2.25 2.25 0 0 0 21 18.75V16.5M16.5 12 12 16.5m0 0L7.5 12m4.5 4.5V3" />
            </svg>
            Export
          </button>
          <button onclick={clearLogs} class="btn btn-sm btn-outline gap-1">
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-4">
              <path stroke-linecap="round" stroke-linejoin="round" d="m14.74 9-.346 9m-4.788 0L9.26 9m9.968-3.21c.342.052.682.107 1.022.166m-1.022-.165L18.16 19.673a2.25 2.25 0 0 1-2.244 2.077H8.084a2.25 2.25 0 0 1-2.244-2.077L4.772 5.79m14.456 0a48.108 48.108 0 0 0-3.478-.397m-12 .562c.34-.059.68-.114 1.022-.165m0 0a48.11 48.11 0 0 1 3.478-.397m7.5 0v-.916c0-1.18-.91-2.164-2.09-2.201a51.964 51.964 0 0 0-3.32 0c-1.18.037-2.09 1.022-2.09 2.201v.916m7.5 0a48.667 48.667 0 0 0-7.5 0" />
            </svg>
            Clear
          </button>
        </div>
      </div>
    </div>
  </div>

  <!-- Live Logs Card -->
  <div class="flex-1 pb-4 min-h-0">
    <div class="bg-base-200 rounded-lg border border-base-300 h-full flex flex-col">
      <!-- Card Header -->
      <div class="px-4 py-3 border-b border-base-300 flex items-center gap-2">
        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-4 text-teal-500">
          <path stroke-linecap="round" stroke-linejoin="round" d="M3.75 3v11.25A2.25 2.25 0 0 0 6 16.5h2.25M3.75 3h-1.5m1.5 0h16.5m0 0h1.5m-1.5 0v11.25A2.25 2.25 0 0 1 18 16.5h-2.25m-7.5 0h7.5m-7.5 0-1 3m8.5-3 1 3m0 0 .5 1.5m-.5-1.5h-9.5m0 0-.5 1.5M9 11.25v1.5M12 9v3.75m3-6v6" />
        </svg>
        <span class="font-medium">Live Logs</span>
        <span class="badge badge-sm bg-base-100 text-base-content">{filteredCount()}</span>
      </div>

      <!-- Log Content -->
      <div class="flex-1 relative overflow-hidden">
        {#if filteredCount() === 0}
          <div class="absolute inset-0 flex items-center justify-center">
            <span class="text-base-content/50 text-sm">No logs to display</span>
          </div>
        {:else}
          <div
            class="absolute inset-0 overflow-y-auto font-mono text-sm p-2"
            bind:this={scrollEl}
          >
            {#if virtualizer}
              <div
                style="height: {$virtualizer?.getTotalSize()}px; width: 100%; position: relative;"
              >
                {#each $virtualizer?.getVirtualItems() as virtualRow (virtualRow.index)}
                  {@const item = filteredLogs()[virtualRow.index]}
                  <div
                    style="position: absolute; top: 0; left: 0; width: 100%; transform: translateY({virtualRow.start}px);"
                    data-index={virtualRow.index}
                    use:measureEl
                  >
                    <div class="w-full mb-1 last:mb-0">
                      <button
                        type="button"
                        class="flex items-center justify-between sm:gap-4 md:gap-6 p-3 cursor-pointer hover:bg-base-200 transition-colors bg-base-100 border border-base-300 rounded w-full text-left"
                        onclick={() => (item.expanded = !item?.expanded)}
                      >
                        <div class="flex items-center gap-2 min-w-0 flex-1">
                          <span
                            class="transition-transform duration-200 shrink-0"
                            class:rotate-90={item?.expanded}
                          >
                            <svg
                              xmlns="http://www.w3.org/2000/svg"
                              viewBox="0 0 20 20"
                              fill="currentColor"
                              class="size-4 text-base-content/50"
                            >
                              <path
                                fill-rule="evenodd"
                                d="M8.22 5.22a.75.75 0 0 1 1.06 0l4.25 4.25a.75.75 0 0 1 0 1.06l-4.25 4.25a.75.75 0 0 1-1.06-1.06L11.94 10 8.22 6.28a.75.75 0 0 1 0-1.06Z"
                                clip-rule="evenodd"
                              />
                            </svg>
                          </span>
                          <span
                            class="font-semibold text-xs shrink-0 {getLogLevelColor(item?.level)}"
                            >{item?.level}</span
                          >
                          <span class="badge badge-xs badge-ghost shrink-0">{item?.tag}</span>
                          <span
                            class="line-clamp-1 text-sm text-base-content/80 break-all"
                            >{item?.msg}</span
                          >
                        </div>
                        <div class="tooltip tooltip-left shrink-0" data-tip={item?.localts}>
                          <span
                            class="text-xs text-base-content/40 whitespace-nowrap"
                          >
                            {formatRelativeTime(item?.localts)}
                          </span>
                        </div>
                      </button>
                      {#if item?.expanded}
                        <div
                          class="bg-base-200/50 border-t border-base-300 p-3"
                          transition:fade={{ duration: 150 }}
                        >
                          <div class="flex flex-col gap-1 text-xs">
                            <p class="text-base-content/70">
                              <span class="font-semibold">RTC Timestamp:</span> {item.uptime}
                            </p>
                            <p class="text-base-content/70">
                              <span class="font-semibold">Boot Timestamp:</span> {item.ts}
                            </p>
                            <p class="mt-2 text-base-content/90 whitespace-pre-wrap break-all">{item.msg}</p>
                          </div>
                        </div>
                      {/if}
                    </div>
                  </div>
                {/each}
              </div>
            {/if}
          </div>
        {/if}
      </div>
    </div>
  </div>

  <!-- Scroll to bottom button -->
  {#if !atBottom() && !autoScrollActive && filteredCount() > 0}
    <div
      class="fixed bottom-15 right-15 z-10"
      transition:fade={{ duration: 200 }}
    >
      <button
        onclick={scrollToBottom}
        class="btn btn-circle btn-accent shadow-lg"
        aria-label="Scroll to bottom"
      >
        <svg
          xmlns="http://www.w3.org/2000/svg"
          fill="none"
          viewBox="0 0 24 24"
          stroke-width="1.5"
          stroke="currentColor"
          class="size-5"
        >
          <path
            stroke-linecap="round"
            stroke-linejoin="round"
            d="M3 16.5v2.25A2.25 2.25 0 0 0 5.25 21h13.5A2.25 2.25 0 0 0 21 18.75V16.5M16.5 12 12 16.5m0 0L7.5 12m4.5 4.5V3"
          />
        </svg>
      </button>
    </div>
  {/if}
</div>

