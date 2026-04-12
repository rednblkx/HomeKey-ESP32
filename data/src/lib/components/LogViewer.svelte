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
    estimateSize: () => 64,
    overscan: 5,
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
    if (scrollTop < lastScrollTop) {
      autoScrollActive = false;
    }
    lastScrollTop = scrollTop;
  }

  $effect(() => {
    if (!scrollEl || virtualizer) return;

    virtualizer = createVirtualizer<HTMLDivElement, Element>({
      count: filteredCount(),
      ...virtualizerOptions,
    });

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
        doScroll(count - 1, isInitialScroll ? 'auto' : 'smooth');
      }
    }
    previousLogCount = count;
  });

  $effect(() => {
    if (isInitialized || !virtualizer) return;
    const totalSize = $virtualizer?.getTotalSize() || 0;
    if (totalSize > 0) {
      isInitialized = true;
      doScroll(filteredCount() - 1, 'auto');
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

<div class="h-full flex flex-col bg-base-200 text-base-content font-sans">
  <!-- Toolbar -->
  <div class="shrink-0 bg-base-100 border-b border-base-300 p-2">
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
            <div class="tooltip tooltip-bottom" data-tip="Persists on reboot">
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

      <div class="flex justify-between items-center gap-2 flex-wrap w-full">
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
        <div class="flex gap-2">
          <button onclick={() => exportLogs()} class="btn btn-sm btn-info"
            >Export</button
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
      <div
        class="log-container h-full overflow-y-auto font-mono text-sm"
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
                    class="flex items-center justify-between sm:gap-4 md:gap-6 p-4 cursor-pointer hover:bg-base-200 transition-colors bg-base-100 border border-base-300 rounded-md w-full"
                    onclick={() => (item.expanded = !item.expanded)}
                  >
                    <div class="flex items-center">
                      <div class="flex items-center justify-center mr-1">
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
                          class="font-bold mr-2 {getLogLevelColor(item.level)}"
                          >{item.level}</span
                        >
                      </div>
                      <div class="flex max-sm:flex-col">
                        <span class="text-base-content/90 mr-2 badge badge-dash"
                          >{item.tag}</span
                        >
                        <span
                          class="line-clamp-1 text-base-content/90 break-all wrap-anywhere"
                          >{item.msg}</span
                        >
                      </div>
                    </div>
                    <div class="tooltip tooltip-left" data-tip={item.localts}>
                      <span
                        class="text-xs text-base-content/50 whitespace-wrap"
                      >
                        {formatRelativeTime(item.localts)}
                      </span>
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
                        {item.uptime}
                      </p>
                      <p
                        class="font-mono text-xs wrap-anywhere text-wrap break-all"
                      >
                        <strong>Boot Timestamp:</strong>
                        {item.ts}
                      </p>
                      <div class="mt-2 text-xs whitespace-pre-wrap break-all">
                        {item.msg}
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            {/each}
          </div>
        {/if}
      </div>

      {#if !atBottom() && !autoScrollActive}
        <div
          class="absolute bottom-4 right-8 z-10"
          transition:fade={{ duration: 200 }}
        >
          <button
            onclick={scrollToBottom}
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

