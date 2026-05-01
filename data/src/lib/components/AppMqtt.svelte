<script lang="ts">
  import { type MqttConfig, CertificateType } from '$lib/types/api';
  import { saveConfig } from '$lib/services/api';
  import { diff } from '$lib/utils/objDiff';
  import CertManager from './CertManager.svelte';
  import { systemInfo } from '$lib/stores/system.svelte';

  let { mqtt, error }: { mqtt: MqttConfig; error: string | null } = $props();

  // svelte-ignore state_referenced_locally
  let mqttConfig = $state<MqttConfig>($state.snapshot(mqtt));
  let activeTab = $state<'broker' | 'topics' | 'ssl'>('broker');

  let mqttConnected = $derived<boolean>(systemInfo.mqtt_connected);
  let mqttErrorCode = $derived<number>(systemInfo.mqtt_error_code);
  let mqttErrorMessage = $derived<string | undefined>(systemInfo.mqtt_error_message);

  const saveMqttConfig = async (e: Event) => {
    e.preventDefault();
    try {
      if (!mqttConfig || !mqtt) return;
      const result = await saveConfig('mqtt', diff(mqtt as MqttConfig, mqttConfig));
      if (result.success) {
        mqttConfig = result.data;
        mqtt = result.data;
      }
    } catch (e) {
      const message = e instanceof Error ? e.message : String(e);
      alert(`Error saving config: ${message}`);
    }
  };

  const resetForm = () => {
    if (mqtt) mqttConfig = $state.snapshot(mqtt);
  };
</script>

<div class="w-full py-6">
  <!-- Header -->
  <div class="mb-6">
    <h1 class="text-2xl font-bold text-base-content flex items-center gap-2">
      MQTT Settings
      <div class="tooltip tooltip-bottom tooltip-info" data-tip="Device will reboot to apply changes!">
        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-5 text-info">
          <path stroke-linecap="round" stroke-linejoin="round" d="m3.75 13.5 10.5-11.25L12 10.5h8.25L9.75 21.75 12 13.5H3.75Z" />
        </svg>
      </div>
    </h1>
    <p class="text-sm text-base-content/60">Configure MQTT broker connection and topic mappings.</p>
  </div>

  {#if !mqtt && error}
    <div class="text-center text-error">
      <p>Error: {error}</p>
    </div>
  {:else if mqtt}
    <form onsubmit={saveMqttConfig} class="max-w-5xl">
      <!-- Tabs -->
      <div class="flex bg-base-300 p-1 rounded-xl gap-1 mb-4">
        <button
          type="button"
          class="flex-1 tab flex-col py-2 rounded-lg transition-colors {activeTab === 'broker' ? 'bg-base-100 text-primary font-medium shadow-sm' : 'text-base-content/60 hover:bg-base-200'}"
          onclick={() => activeTab = 'broker'}
        >
          <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-4">
            <path stroke-linecap="round" stroke-linejoin="round" d="M12 21a9.004 9.004 0 0 0 8.716-6.747M12 21a9.004 9.004 0 0 1-8.716-6.747M12 21c2.485 0 4.5-4.03 4.5-9S14.485 3 12 3m0 18c-2.485 0-4.5-4.03-4.5-9S9.515 3 12 3m0 0a8.997 8.997 0 0 1 7.843 4.582M12 3a8.997 8.997 0 0 0-7.843 4.582m15.686 0A11.953 11.953 0 0 1 12 10.5c-2.998 0-5.74-1.1-7.843-2.918m15.686 0A8.959 8.959 0 0 1 21 12c0 .778-.099 1.533-.284 2.253m0 0A17.919 17.919 0 0 1 12 16.5c-3.162 0-6.133-.815-8.716-2.247m0 0A9.015 9.015 0 0 1 3 12c0-1.605.42-3.113 1.157-4.418" />
          </svg>
          <span class="text-[10px] sm:text-xs mt-0.5">Broker</span>
        </button>
        <button
          type="button"
          class="flex-1 tab flex-col py-2 rounded-lg transition-colors {activeTab === 'topics' ? 'bg-base-100 text-primary font-medium shadow-sm' : 'text-base-content/60 hover:bg-base-200'}"
          onclick={() => activeTab = 'topics'}
        >
          <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-4">
            <path stroke-linecap="round" stroke-linejoin="round" d="M2.25 13.5h3.86a2.25 2.25 0 0 1 2.012 1.244l.256.512a2.25 2.25 0 0 0 2.013 1.244h3.218a2.25 2.25 0 0 0 2.013-1.244l.256-.512a2.25 2.25 0 0 1 2.013-1.244h3.859m-19.5.338V18a2.25 2.25 0 0 0 2.25 2.25h15A2.25 2.25 0 0 0 21.75 18v-4.162c0-.224-.034-.447-.1-.661L19.24 5.338a2.25 2.25 0 0 0-2.15-1.588H6.911a2.25 2.25 0 0 0-2.15 1.588L2.35 13.177a2.25 2.25 0 0 0-.1.661Z" />
          </svg>
          <span class="text-[10px] sm:text-xs mt-0.5">Topics</span>
        </button>
        <button
          type="button"
          class="flex-1 tab flex-col py-2 rounded-lg transition-colors {activeTab === 'ssl' ? 'bg-base-100 text-primary font-medium shadow-sm' : 'text-base-content/60 hover:bg-base-200'}"
          onclick={() => activeTab = 'ssl'}
        >
          <svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
            <path stroke-linecap="round" stroke-linejoin="round" d="M16.5 10.5V6.75a4.5 4.5 0 1 0-9 0v3.75m-.75 11.25h10.5a2.25 2.25 0 0 0 2.25-2.25v-6.75a2.25 2.25 0 0 0-2.25-2.25H6.75a2.25 2.25 0 0 0-2.25 2.25v6.75a2.25 2.25 0 0 0 2.25 2.25Z" />
          </svg>
          <span class="text-[10px] sm:text-xs mt-0.5">SSL/TLS</span>
        </button>
      </div>

      <!-- Tab Content -->
      <div class="card bg-base-200 shadow-xl">
        <div class="card-body p-4">
        {#if activeTab === 'broker'}
          <!-- Broker Configuration -->
          <div class="space-y-6">
            <div>
              <h3 class="font-semibold text-base-content">Broker Configuration</h3>
              <p class="text-sm text-base-content/60">Configure the MQTT broker connection settings.</p>
            </div>

            <!-- MQTT Connection Status -->
            <div class="flex items-center gap-3 py-2 px-3 bg-base-100 rounded-lg">
              <div class="flex items-center gap-2">
                <span class="inline-flex h-3 w-3 rounded-full {mqttConnected === true ? 'bg-success' : mqttConnected === false ? (mqttErrorCode > 0 ? 'bg-error' : 'bg-warning') : 'bg-base-300'}"></span>
                <span class="text-sm font-medium">
                  {#if mqttConnected === true}
                    <span class="text-success">Connected</span>
                  {:else if mqttConnected === false}
                    {#if mqttErrorCode > 0}
                      <span class="text-error">Error</span>
                    {:else}
                      <span class="text-warning">Disconnected</span>
                    {/if}
                  {:else}
                    <span class="text-base-content/50">Unknown</span>
                  {/if}
                </span>
              </div>
              {#if mqttConnected === false && mqttErrorCode > 0 && mqttErrorMessage}
                <span class="text-xs text-error">({mqttErrorMessage})</span>
              {/if}
              {#if mqttConnected === false && mqttErrorCode > 0 && !mqttErrorMessage}
                <span class="text-xs text-error">(Code: {mqttErrorCode})</span>
              {/if}
            </div>

            <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
              <div class="form-control">
                <label class="label">
                  <span class="label-text text-sm">Broker Address</span>
                </label>
                <input
                  type="text"
                  bind:value={mqttConfig.mqttBroker}
                  placeholder="192.168.1.100"
                  class="input input-sm input-bordered w-full"
                  required
                />
              </div>
              <div class="form-control">
                <label class="label">
                  <span class="label-text text-sm">Port</span>
                </label>
                <input
                  type="number"
                  bind:value={mqttConfig.mqttPort}
                  placeholder="1883"
                  class="input input-sm input-bordered w-full"
                  required
                  min="0"
                  max="65535"
                />
              </div>
              <div class="form-control md:col-span-2">
                <label class="label">
                  <span class="label-text text-sm">Client ID</span>
                </label>
                <input
                  type="text"
                  bind:value={mqttConfig.mqttClientId}
                  placeholder="homekey-esp32-lock"
                  class="input input-sm input-bordered w-full"
                  required
                />
              </div>
              <div class="form-control">
                <label class="label">
                  <span class="label-text text-sm">Username</span>
                </label>
                <input
                  type="text"
                  bind:value={mqttConfig.mqttUsername}
                  placeholder="homeassistant"
                  class="input input-sm input-bordered w-full"
                />
              </div>
              <div class="form-control">
                <label class="label">
                  <span class="label-text text-sm">Password</span>
                </label>
                <input
                  type="password"
                  bind:value={mqttConfig.mqttPassword}
                  placeholder="••••••••"
                  class="input input-sm input-bordered w-full"
                />
              </div>
              <div class="form-control md:col-span-2">
                <label class="label">
                  <span class="label-text text-sm">LWT Topic</span>
                </label>
                <input
                  type="text"
                  bind:value={mqttConfig.lwtTopic}
                  placeholder="homekey/lock/status"
                  class="input input-sm input-bordered w-full"
                  required
                />
              </div>
            </div>

            <!-- Home Assistant Discovery Toggle -->
            <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
              <div>
                <p class="font-medium text-sm text-base-content">Home Assistant MQTT Discovery</p>
                <p class="text-xs text-base-content/70">Enable automatic device discovery in Home Assistant</p>
              </div>
              <input
                type="checkbox"
                bind:checked={mqttConfig.hassMqttDiscoveryEnabled}
                class="toggle toggle-primary toggle-md"
              />
            </div>
          </div>
        {/if}

        {#if activeTab === 'topics'}
          <!-- Topic Configuration -->
          <div class="space-y-6">
            <div>
              <h3 class="font-semibold text-base-content">Topic Configuration</h3>
              <p class="text-sm text-base-content/60">Configure MQTT topics for different events.</p>
            </div>

            <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
              <div class="form-control">
                <label class="label">
                  <span class="label-text text-sm">HomeKey Topic</span>
                </label>
                <input
                  type="text"
                  bind:value={mqttConfig.hkTopic}
                  placeholder="homekey/lock/homekey"
                  class="input input-sm input-bordered w-full"
                  required
                />
              </div>
              <div class="form-control">
                <label class="label">
                  <span class="label-text text-sm">Lock State Topic</span>
                </label>
                <input
                  type="text"
                  bind:value={mqttConfig.lockStateTopic}
                  placeholder="homekey/lock/state"
                  class="input input-sm input-bordered w-full"
                  required
                />
              </div>
              <div class="form-control">
                <label class="label">
                  <span class="label-text text-sm">Lock State Command</span>
                </label>
                <input
                  type="text"
                  bind:value={mqttConfig.lockStateCmd}
                  placeholder="homekey/lock/set"
                  class="input input-sm input-bordered w-full"
                  required
                />
              </div>
              <div class="form-control">
                <label class="label">
                  <span class="label-text text-sm">Lock Current State Command</span>
                </label>
                <input
                  type="text"
                  bind:value={mqttConfig.lockCStateCmd}
                  placeholder="homekey/lock/current"
                  class="input input-sm input-bordered w-full"
                  required
                />
              </div>
              <div class="form-control">
                <label class="label">
                  <span class="label-text text-sm">Lock Target State Command</span>
                </label>
                <input
                  type="text"
                  bind:value={mqttConfig.lockTStateCmd}
                  placeholder="homekey/lock/target"
                  class="input input-sm input-bordered w-full"
                  required
                />
              </div>
              <div class="form-control">
                <label class="label">
                  <span class="label-text text-sm">Battery Level Topic</span>
                </label>
                <input
                  type="text"
                  bind:value={mqttConfig.btrLvlCmdTopic}
                  placeholder="homekey/lock/battery"
                  class="input input-sm input-bordered w-full"
                  required
                />
              </div>
              <div class="form-control md:col-span-2">
                <label class="label">
                  <span class="label-text text-sm">Alt Action Topic</span>
                </label>
                <input
                  type="text"
                  bind:value={mqttConfig.hkAltActionTopic}
                  placeholder="homekey/lock/alt_action"
                  class="input input-sm input-bordered w-full"
                  required
                />
              </div>
            </div>

            <!-- Ignore NFC Tags Toggle -->
            <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
              <div>
                <p class="font-medium text-sm text-base-content">Ignore NFC Tags</p>
                <p class="text-xs text-base-content/70">Do not publish NFC tag events to MQTT</p>
              </div>
              <input
                type="checkbox"
                bind:checked={mqttConfig.nfcTagNoPublish}
                class="toggle toggle-primary toggle-md"
              />
            </div>

            <!-- Custom Lock States Toggle -->
            <div class="flex items-center justify-between py-2 mb-0 px-3 bg-base-100 rounded-lg">
              <div>
                <p class="font-medium text-sm text-base-content">Custom Lock States</p>
                <p class="text-xs text-base-content/70">Enable custom lock state mappings</p>
              </div>
              <input
                type="checkbox"
                bind:checked={mqttConfig.lockEnableCustomState}
                class="toggle toggle-primary toggle-md"
              />
            </div>

            {#if mqttConfig.lockEnableCustomState}
              <div class="space-y-4 border-t border-base-200 p-4">
                <!-- Custom State Topic + Actions Group -->
                <div class="bg-base-100 rounded-lg p-3 border border-base-200">
                  <div class="form-control mb-3">
                    <label class="label">
                      <span class="label-text text-sm font-medium">Payload Topic (Pub)</span>
                    </label>
                    <input
                      type="text"
                      bind:value={mqttConfig.lockCustomStateTopic}
                      placeholder="homekey/lock/custom_state"
                      class="input input-sm input-bordered w-full"
                    />
                  </div>
                  <fieldset class="border border-base-content/20 rounded-lg p-3">
                    <legend class="text-sm font-medium text-base-content/80 px-2">Custom Lock Actions</legend>
                    <div class="grid grid-cols-2 gap-3 pt-1">
                      <div class="form-control">
                        <label class="label py-1">
                          <span class="label-text text-sm">Unlock</span>
                        </label>
                        <input
                          type="number"
                          bind:value={mqttConfig.customLockActions['UNLOCK']}
                          class="input input-sm input-bordered w-full"
                        />
                      </div>
                      <div class="form-control">
                        <label class="label py-1">
                          <span class="label-text text-sm">Lock</span>
                        </label>
                        <input
                          type="number"
                          bind:value={mqttConfig.customLockActions['LOCK']}
                          class="input input-sm input-bordered w-full"
                        />
                      </div>
                    </div>
                  </fieldset>
                </div>

                <!-- Custom State Cmd Topic + States Group -->
                <div class="bg-base-100 rounded-lg p-3 border border-base-200">
                  <div class="form-control mb-3">
                    <label class="label">
                      <span class="label-text text-sm font-medium">Command Topic (Sub)</span>
                    </label>
                    <input
                      type="text"
                      bind:value={mqttConfig.lockCustomStateCmd}
                      placeholder="homekey/lock/custom_cmd"
                      class="input input-sm input-bordered w-full"
                    />
                  </div>
                  <fieldset class="border border-base-content/20 rounded-lg p-3">
                    <legend class="text-sm font-medium text-base-content/80 px-2">Custom Lock States</legend>
                    <div class="grid grid-cols-3 gap-3 pt-1">
                      <div class="form-control">
                        <label class="label py-1">
                          <span class="label-text text-sm">Unlocking</span>
                        </label>
                        <input
                          type="number"
                          bind:value={mqttConfig.customLockStates['C_UNLOCKING']}
                          class="input input-sm input-bordered w-full"
                        />
                      </div>
                      <div class="form-control">
                        <label class="label py-1">
                          <span class="label-text text-sm">Locking</span>
                        </label>
                        <input
                          type="number"
                          bind:value={mqttConfig.customLockStates['C_LOCKING']}
                          class="input input-sm input-bordered w-full"
                        />
                      </div>
                      <div class="form-control">
                        <label class="label py-1">
                          <span class="label-text text-sm">Unlocked</span>
                        </label>
                        <input
                          type="number"
                          bind:value={mqttConfig.customLockStates['C_UNLOCKED']}
                          class="input input-sm input-bordered w-full"
                        />
                      </div>
                      <div class="form-control">
                        <label class="label py-1">
                          <span class="label-text text-sm">Locked</span>
                        </label>
                        <input
                          type="number"
                          bind:value={mqttConfig.customLockStates['C_LOCKED']}
                          class="input input-sm input-bordered w-full"
                        />
                      </div>
                      <div class="form-control">
                        <label class="label py-1">
                          <span class="label-text text-sm">Jammed</span>
                        </label>
                        <input
                          type="number"
                          bind:value={mqttConfig.customLockStates['C_JAMMED']}
                          class="input input-sm input-bordered w-full"
                        />
                      </div>
                      <div class="form-control">
                        <label class="label py-1">
                          <span class="label-text text-sm">Unknown</span>
                        </label>
                        <input
                          type="number"
                          bind:value={mqttConfig.customLockStates['C_UNKNOWN']}
                          class="input input-sm input-bordered w-full"
                        />
                      </div>
                    </div>
                  </fieldset>
                </div>
              </div>
            {/if}
          </div>
        {/if}

        {#if activeTab === 'ssl'}
          <!-- SSL/TLS Configuration -->
          <div class="space-y-6">
            <div>
              <h3 class="font-semibold text-base-content">SSL/TLS Configuration</h3>
              <p class="text-sm text-base-content/60">Secure your MQTT connection with TLS certificates.</p>
            </div>

            <!-- Enable SSL/TLS Toggle -->
            <div class="flex items-center justify-between py-2 px-3 bg-base-100 rounded-lg">
              <div>
                <p class="font-medium text-sm text-base-content">Enable SSL/TLS</p>
                <p class="text-xs text-base-content/70">Use encrypted connection to the broker</p>
              </div>
              <input
                type="checkbox"
                bind:checked={mqttConfig.useSSL}
                class="toggle toggle-primary toggle-md"
              />
            </div>

            {#if mqttConfig.useSSL}
              <!-- Skip Certificate Validation -->
              <div class="bg-error/10 border border-error/20 rounded-lg p-4 flex items-center justify-between">
                <div class="flex items-center gap-3">
                  <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-5 text-error">
                    <path stroke-linecap="round" stroke-linejoin="round" d="M12 9v3.75m-9.303 3.376c-.866 1.5.217 3.374 1.948 3.374h14.71c1.73 0 2.813-1.874 1.948-3.374L13.949 3.378c-.866-1.5-3.032-1.5-3.898 0L2.697 16.126ZM12 15.75h.007v.008H12v-.008Z" />
                  </svg>
                  <div>
                    <p class="font-medium text-sm">Skip CN validation</p>
                    <p class="text-xs text-error">Leave it disabled if you don't know</p>
                  </div>
                </div>
                <input
                  type="checkbox"
                  bind:checked={mqttConfig.allowInsecure}
                  class="toggle toggle-error toggle-md"
                />
              </div>

              <!-- Certificate Upload Section -->
              <div class="space-y-4 pt-4">
                <CertManager
                  certType={CertificateType.MQTT_CLIENT}
                  keyType={CertificateType.MQTT_PRIVATE_KEY}
                  caType={CertificateType.MQTT_CA}
                  mode="mqtt"
                />
              </div>
            {/if}
          </div>
        {/if}
        </div>
      </div>

      <!-- Action Buttons -->
      <div class="flex gap-3 mt-6">
        <button type="submit" class="btn btn-primary">Save & Apply changes</button>
        <button type="button" class="btn btn-ghost" onclick={resetForm}>Reset</button>
      </div>
    </form>
  {/if}
</div>
