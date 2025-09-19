<template>
  <div class="w-full max-w-4xl space-y-6">
    <!-- HomeKey Info Card -->
    <div v-if="loading" class="text-center">
      <span class="loading loading-spinner loading-lg"></span>
    </div>
    <div v-else-if="error" class="text-center text-error">
      <p>Error: {{ error }}</p>
    </div>
    <div v-else class="card bg-base-200 shadow-xl">
      <h2 class="card-title p-4 pb-0"><div class="badge badge-info badge-md">Info</div> HomeKey</h2>
      <div class="card-body p-4">
        <div>
          <div class="stats stats-vertical md:stats-horizontal shadow bg-base-100 w-full">
            <div class="stat">
              <div class="stat-title">Reader GID</div>
              <div class="stat-value text-xl">{{ hkInfo.group_identifier || 'N/A' }}</div>
            </div>
            <div class="stat">
              <div class="stat-title">Reader ID</div>
              <div class="stat-value text-xl">{{ hkInfo.unique_identifier || 'N/A' }}</div>
            </div>
            <div class="stat">
              <div class="stat-title">Issuers</div>
              <div class="stat-value text-xl">{{ hkInfo.issuers ? hkInfo.issuers.length : 0 }}</div>
            </div>
          </div>

          <div v-if="hkInfo.issuers && hkInfo.issuers.length > 0" class="mt-8">
            <div v-for="(issuer, index) in hkInfo.issuers" :key="`issuer-${index}`" class="list-row">
              <div class="collapse collapse-arrow bg-base-100 shadow">
                <input type="checkbox" name="info-collapse" />
                <div class="collapse-title font-semibold">
                  <div class="text-xs uppercase font-semibold opacity-60">Issuer</div>
                  {{ issuer.issuerId || 'N/A' }}
                </div>
                <div class="collapse-content text-sm">
                  <ul v-if="issuer.endpoints && issuer.endpoints.length > 0" class="ml-4">
                    <li v-for="(endpoint, epIndex) in issuer.endpoints" :key="`ep-${epIndex}`">
                      <div class="text-xs uppercase font-semibold opacity-60">Endpoint Id:</div>
                      {{ endpoint.endpointId || 'N/A' }}
                      <div v-if="epIndex < issuer.endpoints.length - 1" class="divider my-2 "></div>
                    </li>
                  </ul>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>

    <div class="card bg-base-200 shadow-xl">
      <h2 class="card-title p-4 pb-0"><div class="badge badge-info badge-md">Info</div> System</h2>
      <div class="card-body p-4">
        <div>
          <div class="stats stats-vertical md:stats-horizontal shadow bg-base-100 w-full">
            <div class="stat">
              <div class="stat-title">Version</div>
              <div class="stat-value text-xl">{{ sysInfo?.version || 'N/A' }}</div>
            </div>
            <div class="stat">
              <div class="stat-title">WiFi SSID</div>
              <div class="stat-value text-xl">{{ sysInfo?.wifi_ssid || 'N/A' }}</div>
            </div>
            <div v-if="!sysInfo?.eth_enabled" class="stat">
              <div class="stat-title">WiFi RSSI</div>
              <div class="stat-value text-xl">{{ sysInfo?.wifi_rssi || 'N/A' }} ({{ wifi_signal }})</div>
            </div>
            <div class="stat">
              <div class="stat-title">Ethernet enabled</div>
              <div class="stat-value text-xl">{{ sysInfo?.eth_enabled ? 'Yes' : 'No' }}</div>
            </div>
          </div>
        </div>
      </div>
    </div>
    
    <!-- WebSocket Test Component -->
    <WebSocketTestComponent />
  </div>
</template>

<script>
import { fetchConfig } from '../services/api';
import { defineAsyncComponent } from 'vue';

export default {
  inject: ['sysInfo'],
  data() {
    return {
      wifi_signal: "Unknown",
      hkInfo: {},
      loading: true,
      error: null,
    };
  },
  async mounted() {
    try {
      this.hkInfo = await fetchConfig('hkinfo');
    } catch (e) {
      this.error = e.message;
    } finally {
      this.loading = false;
      }
  },
  components: {
    WebSocketTestComponent : __DEV__ ? defineAsyncComponent(() => import('./WebSocketTest.vue')) : null,
  },
  watch: {
    'sysInfo.wifi_rssi': {
      handler(wifi_rssi) {
        if(wifi_rssi <= -30 || wifi_rssi >= -70) {
          this.wifi_signal = 'Excellent';
        } else if (wifi_rssi <= -71 || wifi_rssi >= -80) {
          this.wifi_signal = 'Good';
        } else if (wifi_rssi <= -81 || wifi_rssi >= -90) {
          this.wifi_signal = 'Fair';
        } else if (wifi_rssi <= -91 || wifi_rssi >= -100) {
          this.wifi_signal = 'Poor';
        } else {
          this.wifi_signal = 'Unknown';
        }
      }
    }
  }
};
</script>
