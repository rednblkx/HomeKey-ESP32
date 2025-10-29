<script lang="ts">
	import { onMount, type Component } from 'svelte';
	import type { HKInfo } from '$lib/types/api.js';
	import { systemInfo } from '$lib/stores/system.svelte.js';
	import { loadingStates, setLoadingState } from '$lib/stores/loading.svelte.js';
	import { calculateWifiSignal } from '$lib/utils/wifi.js';
  import { version } from '$app/environment';

	/**
	 * Props for HKInfo component
	 * @type {HKInfo | null} hkInfo - HomeKey information data
	 * @type {string | null} error - Error message if data loading failed
	 */
	let { hkInfo, error }: { hkInfo: HKInfo | null; error: string | null } = $props();

	let webSocketTestPromise: Promise<Component> | null = $state(null);

	onMount(async (): Promise<void> => {
		systemInfo.uptime == 0 && setLoadingState('systemInfoLoading', true);

		if (import.meta.env.DEV) {
			webSocketTestPromise = import('$lib/components/WebSocketTest.svelte').then(module => module.default);
		}
	});

	let wifi_rssi = $derived(systemInfo?.wifi_rssi);
	let wifi_signal = $derived.by(() => calculateWifiSignal(wifi_rssi));
</script>

<div class="w-full py-6 flex flex-col items-center">
  <div class="flex flex-col max-w-4xl gap-6">
    <!-- HomeKey Info Card -->
    {#if loadingStates.hkInfoLoading}
      <div class="card bg-base-200 shadow-xl" aria-live="polite" aria-label="Loading HomeKey information">
        <h2 class="card-title p-4 pb-0">
          <div class="badge badge-info badge-md" aria-label="HomeKey information section">Info</div>
          HomeKey
        </h2>
        <div class="card-body p-4">
          <div class="flex justify-center">
            <span class="loading loading-spinner loading-lg" aria-label="Loading HomeKey data"></span>
          </div>
        </div>
      </div>
    {:else if error}
      <div class="text-center text-error" aria-live="assertive">
        <p>Error: {error}</p>
      </div>
    {:else if hkInfo}
      <div class="card bg-base-200 shadow-xl">
        <h2 class="card-title p-4 pb-0">
          <div class="badge badge-info badge-md">Info</div>
          HomeKey
        </h2>
        <div class="card-body p-4">
          <div>
            <div class="stats stats-vertical md:stats-horizontal shadow bg-base-100 w-full">
              <div class="stat">
                <div class="stat-title">Reader GID</div>
                <div class="stat-value text-xl">{hkInfo.group_identifier || 'N/A'}</div>
              </div>
              <div class="stat">
                <div class="stat-title">Reader ID</div>
                <div class="stat-value text-xl">{hkInfo.unique_identifier || 'N/A'}</div>
              </div>
              <div class="stat">
                <div class="stat-title">Issuers</div>
                <div class="stat-value text-xl">{hkInfo.issuers ? hkInfo.issuers.length : 0}</div>
              </div>
            </div>

            {#if hkInfo.issuers && hkInfo.issuers.length > 0}
              <div class="mt-8">
                {#each hkInfo.issuers as issuer, index (`issuer-${index}`)}
                  <div class="list-row">
                    <div class="collapse collapse-arrow bg-base-100 shadow">
                      <input type="checkbox" name="info-collapse" />
                      <div class="collapse-title font-semibold">
                        <div class="text-xs uppercase font-semibold opacity-60">Issuer</div>
                        {issuer.issuerId || 'N/A'}
                      </div>
                      <div class="collapse-content text-sm">
                        {#if issuer.endpoints && issuer.endpoints.length > 0}
                          <ul class="ml-4">
                            {#each issuer.endpoints as endpoint, epIndex (`ep-${epIndex}`)}
                              <li>
                                <div class="text-xs uppercase font-semibold opacity-60">Endpoint Id:</div>
                                {endpoint.endpointId || 'N/A'}
                                {#if epIndex < issuer.endpoints.length - 1}
                                  <div class="divider my-2"></div>
                                {/if}
                              </li>
                            {/each}
                          </ul>
                        {/if}
                      </div>
                    </div>
                  </div>
                {/each}
              </div>
            {/if}
          </div>
        </div>
      </div>
    {/if}

    <div class="card bg-base-200 shadow-xl" aria-live="polite" aria-label="System information">
      <h2 class="card-title p-4 pb-0">
        <div class="badge badge-info badge-md" aria-label="System information section">Info</div>
        System
      </h2>
      <div class="card-body p-4">
        {#if loadingStates.systemInfoLoading}
          <div class="flex flex-col gap-4" aria-label="Loading system information">
            <div class="stats stats-vertical md:stats-horizontal shadow bg-base-100 w-full">
              <div class="stat">
                <div class="skeleton h-4 w-20 mb-2"></div>
                <div class="skeleton h-6 w-32"></div>
              </div>
              <div class="stat">
                <div class="skeleton h-4 w-16 mb-2"></div>
                <div class="skeleton h-6 w-24"></div>
              </div>
              <div class="stat">
                <div class="skeleton h-4 w-14 mb-2"></div>
                <div class="skeleton h-6 w-28"></div>
              </div>
            </div>
            <div class="stats stats-vertical md:stats-horizontal shadow bg-base-100 w-full">
              <div class="stat">
                <div class="skeleton h-4 w-18 mb-2"></div>
                <div class="skeleton h-6 w-36"></div>
              </div>
              <div class="stat">
                <div class="skeleton h-4 w-16 mb-2"></div>
                <div class="skeleton h-6 w-40"></div>
              </div>
              <div class="stat">
                <div class="skeleton h-4 w-24 mb-2"></div>
                <div class="skeleton h-6 w-12"></div>
              </div>
            </div>
          </div>
        {:else}
          <div class="flex flex-col gap-4">
            <div class="stats stats-vertical md:stats-horizontal shadow bg-base-100 w-full">
              <div class="stat">
                <div class="stat-title">Version</div>
                <div class="stat-value text-xl">{systemInfo?.version || 'N/A'}</div>
              </div>
              <div class="stat">
                <div class="stat-title">UI Version</div>
                <div class="stat-value text-xl">{version || 'N/A'}</div>
              </div>
            </div>
            <div class="stats stats-vertical md:stats-horizontal shadow bg-base-100 w-full">
              <div class="stat">
                <div class="stat-title">Device Name</div>
                <div class="stat-value text-xl">{systemInfo?.deviceName || 'N/A'}</div>
              </div>
              <div class="stat">
                <div class="stat-title">Uptime</div>
                <div class="stat-value text-xl">{systemInfo?.uptime || 'N/A'}</div>
              </div>
              <div class="stat">
                <div class="stat-title">Free Heap</div>
                <div class="stat-value text-xl">{systemInfo?.free_heap || 'N/A'}</div>
              </div>
            </div>
            <div class="stats stats-vertical md:stats-horizontal shadow bg-base-100 w-full">
              <div class="stat">
                <div class="stat-title">WiFi SSID</div>
                <div class="stat-value text-xl">{systemInfo?.wifi_ssid || 'N/A'}</div>
              </div>
              {#if !systemInfo?.eth_enabled}
                <div class="stat">
                  <div class="stat-title">WiFi RSSI</div>
                  <div class="stat-value text-xl">{systemInfo?.wifi_rssi || 'N/A'} ({wifi_signal})</div>
                </div>
              {/if}
              <div class="stat">
                <div class="stat-title">Ethernet enabled</div>
                <div class="stat-value text-xl">{systemInfo?.eth_enabled ? 'Yes' : 'No'}</div>
              </div>
            </div>
          </div>
        {/if}
      </div>
    </div>

    <!-- WebSocket Test Component (lazy-loaded in dev mode) -->
    {#if webSocketTestPromise}
      {#await webSocketTestPromise}
        <div class="card bg-base-200 shadow-xl" aria-live="polite" aria-label="Loading WebSocket Test component">
          <div class="card-body p-4">
            <div class="flex justify-center">
              <span class="loading loading-spinner loading-lg" aria-label="Loading WebSocket Test component"></span>
              <span class="ml-2">Loading WebSocket Test...</span>
            </div>
          </div>
        </div>
        {:then WebSocketTest}
        <WebSocketTest />
        {:catch error}
        <div class="card bg-base-200 shadow-xl" aria-live="assertive" aria-label="Error loading WebSocket Test component">
          <div class="card-body p-4">
            <div class="text-center text-error">
              <p>Failed to load WebSocket Test component: {error.message || 'Unknown error'}</p>
            </div>
          </div>
        </div>
      {/await}
    {/if}
  </div>
</div>
