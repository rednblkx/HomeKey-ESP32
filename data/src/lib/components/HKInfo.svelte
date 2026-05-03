<script lang="ts">
  import type { HKInfo } from "$lib/types/api";
  import { systemInfo } from "$lib/stores/system.svelte.js";
  import { calculateWifiSignal } from "$lib/utils/wifi.js";
  const version: string = __DEV__ ? "dev" : __VERSION__;

  let { hkInfo, error }: { hkInfo: HKInfo | null; error: string | null } =
    $props();

  let wifi_rssi = $derived(systemInfo?.wifi_rssi);
  let wifi_signal = $derived.by(() => calculateWifiSignal(wifi_rssi));
</script>

<div class="w-full py-6">
  <div class="mb-6">
    <h1 class="text-2xl font-bold">Dashboard</h1>
    <p class="text-base-content/60">
      Monitor your HomeKey-ESP32 device status and configuration.
    </p>
  </div>

  <div class="grid grid-cols-1 lg:grid-cols-2 gap-4 max-w-6xl items-start">
    <!-- HomeKey Card -->
    <div class="card bg-base-200 shadow-xl">
      <div class="card-body p-4">
        <div class="flex items-center gap-3 mb-4">
          <div class="w-10 h-10 rounded-lg bg-primary/20 flex items-center justify-center">
            <svg
              xmlns="http://www.w3.org/2000/svg"
              class="size-6 text-primary"
              fill="none"
              viewBox="0 0 24 24"
              stroke="currentColor"
            >
              <path
                stroke-linecap="round"
                stroke-linejoin="round"
                stroke-width="2"
                d="M15 7a2 2 0 012 2m4 0a6 6 0 01-7.743 5.743L11 17H9v2H7v2H4a1 1 0 01-1-1v-2.586a1 1 0 01.293-.707l5.964-5.964A6 6 0 1121 9z"
              />
            </svg>
          </div>
          <div>
            <h2 class="card-title text-lg">HomeKey</h2>
            <p class="text-xs text-base-content/60">Reader Information</p>
          </div>
        </div>

        <div class="space-y-2">
          <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
            <span class="text-sm text-base-content/70">Reader GID</span>
            <span class="text-sm font-medium">{hkInfo?.group_identifier || "N/A"}</span>
          </div>
          <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
            <span class="text-sm text-base-content/70">Reader ID</span>
            <span class="text-sm font-medium">{hkInfo?.unique_identifier || "N/A"}</span>
          </div>
          <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
            <span class="text-sm text-base-content/70">Issuers</span>
            <span class="text-sm font-medium">{hkInfo?.issuers?.length || 0}</span>
          </div>
        </div>

        <!-- Issuers Section inside HomeKey Card -->
        {#if hkInfo?.issuers && hkInfo.issuers.length > 0}
          <div class="mt-4 space-y-1">
            {#each hkInfo.issuers as issuer, index (`issuer-${index}`)}
              <div class="collapse collapse-arrow bg-base-100 rounded-lg">
                <input type="checkbox" name="info-collapse-{index}" />
                <div class="collapse-title font-semibold py-3 flex items-center gap-2">
                  <svg xmlns="http://www.w3.org/2000/svg" class="h-5 w-5 text-base-content/60" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                    <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M16 7a4 4 0 11-8 0 4 4 0 018 0zM12 14a7 7 0 00-7 7h14a7 7 0 00-7-7z" />
                  </svg>
                  <span>Issuer {index + 1}</span>
                </div>
                <div class="collapse-content text-sm">
                  <div class="py-2 px-3 bg-base-200 rounded-lg mb-2">
                    <span class="text-xs text-base-content/60 block mb-1">Issuer ID</span>
                    <span class="text-sm font-mono break-all">{issuer.issuerId || "N/A"}</span>
                  </div>
                  {#if issuer.endpoints && issuer.endpoints.length > 0}
                    <div class="mt-2">
                      <span class="text-xs text-base-content/60 block mb-2">Endpoints</span>
                      <ul class="space-y-2">
                        {#each issuer.endpoints as endpoint, epIndex (`ep-${epIndex}`)}
                          <li class="flex items-center gap-2 py-2 px-3 bg-base-200 rounded-lg">
                            <svg viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" width="24" height="24" fill="currentColor" class="size-4 text-primary">
                              <path d="M12 1C16.9706 1 21 5.02944 21 10V14C21 18.9706 16.9706 23 12 23C10.9137 23 9.8724 22.8076 8.90826 22.4549C9.03638 22.2782 9.15938 22.0977 9.27703 21.9134L9.44782 21.633C10.388 20.0636 10.9461 18.2391 10.9963 16.2884L11 16V9H13V16C13 17.7724 12.6453 19.4619 12.0031 21.0015C12.7954 21 13.5599 20.8673 14.2724 20.6229C14.7147 19.2616 14.966 17.8148 14.9968 16.3138L15 16L14.9998 12.999H16.9998L17 16C17 17.0885 16.8977 18.1531 16.7022 19.1847C18.0583 17.9552 18.9297 16.2 18.9959 14.2407L19 14V10C19 6.13401 15.866 3 12 3C10.4277 3 8.97638 3.51841 7.8078 4.39364L6.38282 2.96769C7.92242 1.73631 9.87522 1 12 1ZM7 10C7 7.23858 9.23858 5 12 5C14.7614 5 17 7.23858 17 10V11H15V10C15 8.34315 13.6569 7 12 7C10.4023 7 9.09634 8.24892 9.00509 9.82373L9 10V16C9 17.5669 8.5996 19.0402 7.89554 20.3233L7.87214 20.3627C7.64284 20.7771 7.38087 21.1711 7.09037 21.5417C6.6495 21.2545 6.23541 20.9297 5.85264 20.5719L5.5445 20.2711C3.96956 18.65 3 16.4382 3 14V10C3 7.87522 3.73631 5.92242 4.96769 4.38282L6.39364 5.8078C5.56325 6.91652 5.05405 8.27971 5.00406 9.75935L5 10V14C5 15.6748 5.58816 17.2122 6.56918 18.4169C6.82239 17.7351 6.97017 17.0034 6.99594 16.2407L7 16V10Z">
                              </path>
                            </svg>
                            <span class="text-sm font-mono">{endpoint.endpointId || "N/A"}</span>
                          </li>
                        {/each}
                      </ul>
                    </div>
                  {/if}
                </div>
              </div>
            {/each}
          </div>
        {/if}
      </div>
    </div>

    <!-- System Card -->
    <div class="card bg-base-200 shadow-xl">
      <div class="card-body p-4">
        <div class="flex items-center gap-3 mb-4">
          <div class="w-10 h-10 rounded-lg bg-info/20 flex items-center justify-center">
            <svg
              viewBox="0 0 24 24"
              xmlns="http://www.w3.org/2000/svg"
              width="24"
              height="24"
              fill="currentColor"
              class="size-6 text-info"
            >
              <path
                d="M6 18H18V6H6V18ZM14 20H10V22H8V20H5C4.44772 20 4 19.5523 4 19V16H2V14H4V10H2V8H4V5C4 4.44772 4.44772 4 5 4H8V2H10V4H14V2H16V4H19C19.5523 4 20 4.44772 20 5V8H22V10H20V14H22V16H20V19C20 19.5523 19.5523 20 19 20H16V22H14V20ZM8 8H16V16H8V8Z"
              >
              </path>
            </svg>
          </div>
          <div>
            <h2 class="card-title text-lg">System</h2>
            <p class="text-xs text-base-content/60">Device Information</p>
          </div>
        </div>

        <div class="space-y-2">
          <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
            <span class="text-sm text-base-content/70">Version</span>
            <span class="text-sm font-medium">{systemInfo?.version || "N/A"}</span>
          </div>
          <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
            <span class="text-sm text-base-content/70">UI Version</span>
            <span class="text-sm font-medium">{version}</span>
          </div>
          <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
            <span class="text-sm text-base-content/70">Device Name</span>
            <span class="text-sm font-medium">{systemInfo?.deviceName || "N/A"}</span>
          </div>
          <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
            <span class="text-sm text-base-content/70">Uptime</span>
            <span class="text-sm font-medium">{systemInfo?.uptime || "N/A"}</span>
          </div>
          <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
            <span class="text-sm text-base-content/70">Free Heap</span>
            <span class="text-sm font-medium">{systemInfo?.free_heap || "N/A"}</span>
          </div>
          <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
            <span class="text-sm text-base-content/70">WiFi SSID</span>
            <span class="text-sm font-medium">{systemInfo?.wifi_ssid || "N/A"}</span>
          </div>
          {#if !systemInfo?.eth_enabled}
            <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
              <span class="text-sm text-base-content/70">WiFi RSSI</span>
              <span class="text-sm font-medium">{systemInfo?.wifi_rssi || "N/A"} ({wifi_signal})</span>
            </div>
          {/if}
          <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
            <span class="text-sm text-base-content/70">Ethernet enabled</span>
            <span class="text-sm font-medium">{systemInfo?.eth_enabled ? "Yes" : "No"}</span>
          </div>
          <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
            <span class="text-sm text-base-content/70">NFC Module</span>
            <span class="text-sm font-medium">{systemInfo?.nfc_connected ? "Connected" : "Disconnected"}</span>
          </div>
          <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
            <span class="text-sm text-base-content/70">MQTT broker</span>
            <span class="text-sm font-medium">{systemInfo?.mqtt_connected ? "Connected" : "Disconnected"}</span>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
