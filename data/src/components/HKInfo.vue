<template>
  <div class="w-full max-w-4xl">
    <div v-if="loading" class="text-center">
      <span class="loading loading-spinner loading-lg"></span>
    </div>
    <div v-else-if="error" class="text-center text-error">
      <p>Error: {{ error }}</p>
    </div>
    <div v-else class="card bg-base-200 shadow-xl">
      <h2 class="card-title p-4 pb-0">HomeKey Info</h2>
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
            <div v-for="(issuer, index) in hkInfo.issuers" :key="index" class="list-row">
              <div tabindex="0" class="collapse collapse-arrow bg-base-100 border-base-300 border">
                <div class="collapse-title font-semibold">
                  <div class="text-xs uppercase font-semibold opacity-60">Issuer</div>
                  {{ issuer.issuerId || 'N/A' }}
                </div>
                <div class="collapse-content text-sm">
                  <ul v-if="issuer.endpoints && issuer.endpoints.length > 0" class="ml-4">
                    <li v-for="(endpoint, epIndex) in issuer.endpoints" :key="epIndex">
                      <div class="text-xs uppercase font-semibold opacity-60">Endpoint Id:</div>
                      {{ endpoint.endpointId || 'N/A' }}
                      <div class="divider"></div>
                    </li>
                  </ul>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import { ref, onMounted } from 'vue';
import { fetchConfig } from '../services/api';

export default {
  setup() {
    const hkInfo = ref({});
    const loading = ref(true);
    const error = ref(null);

    const fetchHKInfo = async () => {
      try {
        hkInfo.value = await fetchConfig('hkinfo');
      } catch (e) {
        error.value = e.message;
      } finally {
        loading.value = false;
      }
    };

    onMounted(fetchHKInfo);

    return {
      hkInfo,
      loading,
      error,
    };
  },
};
</script>
