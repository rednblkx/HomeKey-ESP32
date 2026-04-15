<script lang="ts">
  import { onMount } from 'svelte';
  import type { CertificatesStatus, CertificateType, MqttConfig } from '$lib/types/api';
  import { saveConfig, uploadCertificate, getCertificateStatus, deleteCertificate } from '$lib/services/api';
  import { diff } from '$lib/utils/objDiff';

  let { mqtt, error }: { mqtt: MqttConfig; error: string | null } = $props();

  // svelte-ignore state_referenced_locally
  let mqttConfig = $state<MqttConfig>($state.snapshot(mqtt));
  let activeTab = $state<'broker' | 'topics' | 'ssl'>('broker');
  let certificateStatus = $state<CertificatesStatus>();
  let uploadProgress = $state({ ca: 0, client: 0, privateKey: 0 });
  let uploadErrors = $state({ ca: '', client: '', privateKey: '' });

  onMount(() => {
    if (mqtt) fetchCertificateStatus();
  });

  const fetchCertificateStatus = async () => {
    try {
      const response = await getCertificateStatus();
      if (response.success && response.data) {
        certificateStatus = response.data;
      }
    } catch (e) {
      console.error('Error fetching certificate status:', e);
    }
  };

  const handleCertificateUpload = async (event: Event) => {
    const type = (event.target as HTMLInputElement).dataset['type'] as keyof typeof uploadErrors;
    const file = (event.target as HTMLInputElement).files?.[0];
    if (!file) return;
    uploadErrors[type] = '';

    const validExtensions = ['.pem', '.crt', '.cer', '.der', '.key'];
    const fileNameParts = file.name.split('.');
    const extension = fileNameParts.length > 1 ? fileNameParts.pop()! : '';
    const fileExtension = '.' + extension.toLowerCase();

    if (!validExtensions.includes(fileExtension)) {
      uploadErrors[type] = 'Invalid file type. Must be .pem, .crt, .cer, .der, or .key';
      return;
    }

    uploadProgress[type] = 0;
    const reader = new FileReader();

    reader.onloadstart = () => { uploadProgress[type] = 10; };
    reader.onprogress = (e) => {
      if (e.lengthComputable) {
        uploadProgress[type] = Math.min(90, (e.loaded / e.total) * 100);
      }
    };
    reader.onload = async (e) => {
      try {
        uploadProgress[type] = 95;
        const content = e.target?.result ?? null;
        if (content === null) {
          uploadErrors[type] = 'Failed to read file content';
          uploadProgress[type] = 0;
          return;
        }
        await uploadCertificate(type, content);
        uploadProgress[type] = 100;
        await fetchCertificateStatus();
        (event.target as HTMLInputElement).value = '';
        setTimeout(() => { uploadProgress[type] = 0; }, 1000);
      } catch (error) {
        uploadErrors[type] = `Upload failed: ${(error as Error).message}`;
        uploadProgress[type] = 0;
      }
    };
    reader.onerror = () => {
      uploadErrors[type] = 'Failed to read file';
      uploadProgress[type] = 0;
    };

    if (fileExtension === '.der') {
      reader.readAsArrayBuffer(file);
    } else {
      reader.readAsText(file);
    }
  };

  const deleteCertificateHandler = async (type: CertificateType) => {
    if (!confirm(`Are you sure you want to delete the ${type} certificate?`)) return;
    try {
      await deleteCertificate(type);
      await fetchCertificateStatus();
    } catch (e) {
      alert(`Error deleting certificate: ${e instanceof Error ? e.message : String(e)}`);
    }
  };

  const onSSLToggleChange = () => {
    if (!mqttConfig.useSSL) {
      mqttConfig!.allowInsecure = false;
    }
  };

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
          <svg
            xmlns="http://www.w3.org/2000/svg"
            fill="none"
            viewBox="0 0 24 24"
            stroke-width="1.5"
            stroke="currentColor"
            class="size-4"
          >
            <path
              stroke-linecap="round"
              stroke-linejoin="round"
              d="M21.75 17.25v-.228a4.5 4.5 0 0 0-.12-1.03l-2.268-9.64a3.375 3.375 0 0 0-3.285-2.602H7.923a3.375 3.375 0 0 0-3.285 2.602l-2.268 9.64a4.5 4.5 0 0 0-.12 1.03v.228m19.5 0a3 3 0 0 1-3 3H5.25a3 3 0 0 1-3-3m19.5 0a3 3 0 0 0-3-3H5.25a3 3 0 0 0-3 3m16.5 0h.008v.008h-.008v-.008Zm-3 0h.008v.008h-.008v-.008Z"
            />
          </svg>
          <span class="text-[10px] sm:text-xs mt-0.5">Broker</span>
        </button>
        <button
          type="button"
          class="flex-1 tab flex-col py-2 rounded-lg transition-colors {activeTab === 'topics' ? 'bg-base-100 text-primary font-medium shadow-sm' : 'text-base-content/60 hover:bg-base-200'}"
          onclick={() => activeTab = 'topics'}
        >
          <svg viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg" width="24" height="24" fill="currentColor" class="size-4">
            <path d="M6 8V7C6 3.68629 8.68629 1 12 1C15.3137 1 18 3.68629 18 7V8H20C20.5523 8 21 8.44772 21 9V21C21 21.5523 20.5523 22 20 22H4C3.44772 22 3 21.5523 3 21V9C3 8.44772 3.44772 8 4 8H6ZM19 10H5V20H19V10ZM11 15.7324C10.4022 15.3866 10 14.7403 10 14C10 12.8954 10.8954 12 12 12C13.1046 12 14 12.8954 14 14C14 14.7403 13.5978 15.3866 13 15.7324V18H11V15.7324ZM8 8H16V7C16 4.79086 14.2091 3 12 3C9.79086 3 8 4.79086 8 7V8Z">
            </path>
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
                <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
                  <div class="form-control">
                    <label class="label">
                      <span class="label-text text-sm">Custom State Topic</span>
                    </label>
                    <input
                      type="text"
                      bind:value={mqttConfig.lockCustomStateTopic}
                      placeholder="homekey/lock/custom_state"
                      class="input input-sm input-bordered w-full"
                    />
                  </div>
                  <div class="form-control">
                    <label class="label">
                      <span class="label-text text-sm">Custom State Cmd Topic</span>
                    </label>
                    <input
                      type="text"
                      bind:value={mqttConfig.lockCustomStateCmd}
                      placeholder="homekey/lock/custom_cmd"
                      class="input input-sm input-bordered w-full"
                    />
                  </div>
                </div>

                <!-- Custom Lock Actions -->
                <div class="bg-base-100 rounded-lg p-3">
                  <p class="font-medium text-sm mb-2">Custom Lock Actions</p>
                  <div class="grid grid-cols-2 gap-3">
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
                </div>

                <!-- Custom Lock States -->
                <div class="bg-base-100 rounded-lg p-3">
                  <p class="font-medium text-sm mb-2">Custom Lock States</p>
                  <div class="grid grid-cols-3 gap-3">
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
                onchange={onSSLToggleChange}
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
                <!-- CA Certificate -->
                <div class="bg-base-100 rounded-lg p-4">
                  <div class="flex items-center justify-between mb-3">
                    <div>
                      <p class="font-medium text-sm text-base-content">CA Certificate</p>
                      <p class="text-xs text-base-content/70">
                        {certificateStatus?.ca ? `${certificateStatus.ca.subject} (Expires: ${certificateStatus.ca.expiration.to})` : 'Not uploaded'}
                      </p>
                    </div>
                    <div class="flex items-center gap-2">
                      {#if certificateStatus?.ca}
                        <button
                          type="button"
                          onclick={() => deleteCertificateHandler('ca')}
                          class="btn btn-ghost btn-xs text-error"
                          aria-label="Delete CA Certificate"
                        >
                          <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-4">
                            <path stroke-linecap="round" stroke-linejoin="round" d="m14.74 9-.346 9m-4.788 0L9.26 9m9.968-3.21c.342.052.682.107 1.022.166m-1.022-.165L18.16 19.673a2.25 2.25 0 0 1-2.244 2.077H8.084a2.25 2.25 0 0 1-2.244-2.077L4.772 5.79m14.456 0a48.108 48.108 0 0 0-3.478-.397m-12 .562c.34-.059.68-.114 1.022-.165m0 0a48.11 48.11 0 0 1 3.478-.397m7.5 0v-.916c0-1.18-.91-2.164-2.09-2.201a51.964 51.964 0 0 0-3.32 0c-1.18.037-2.09 1.022-2.09 2.201v.916m7.5 0a48.667 48.667 0 0 0-7.5 0" />
                          </svg>
                        </button>
                      {/if}
                      <label class="btn btn-ghost btn-xs">
                        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-4">
                          <path stroke-linecap="round" stroke-linejoin="round" d="M3 16.5v2.25A2.25 2.25 0 0 0 5.25 21h13.5A2.25 2.25 0 0 0 21 18.75V16.5m-13.5-9L12 3m0 0 4.5 4.5M12 3v13.5" />
                        </svg>
                        <span class="ml-1">Upload</span>
                        <input
                          type="file"
                          onchange={handleCertificateUpload}
                          accept=".pem,.crt,.cer,.der"
                          class="hidden"
                          data-type="ca"
                        />
                      </label>
                    </div>
                  </div>
                  {#if uploadProgress.ca > 0 && uploadProgress.ca < 100}
                    <progress class="progress progress-primary w-full" value={uploadProgress.ca} max="100"></progress>
                  {/if}
                  {#if uploadErrors.ca}
                    <p class="text-error text-xs mt-2">{uploadErrors.ca}</p>
                  {/if}
                </div>

                <!-- Client Certificate -->
                <div class="bg-base-100 rounded-lg p-4">
                  <div class="flex items-center justify-between mb-3">
                    <div>
                      <p class="font-medium text-sm text-base-content">Client Certificate</p>
                      <p class="text-xs text-base-content/70">
                        {certificateStatus?.client ? `${certificateStatus.client.subject} (Expires: ${certificateStatus.client.expiration.to})` : 'Not uploaded'}
                      </p>
                    </div>
                    <div class="flex items-center gap-2">
                      {#if certificateStatus?.client}
                        <button
                          type="button"
                          onclick={() => deleteCertificateHandler('client')}
                          class="btn btn-ghost btn-xs text-error"
                          aria-label="Delete Client Certificate"
                        >
                          <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-4">
                            <path stroke-linecap="round" stroke-linejoin="round" d="m14.74 9-.346 9m-4.788 0L9.26 9m9.968-3.21c.342.052.682.107 1.022.166m-1.022-.165L18.16 19.673a2.25 2.25 0 0 1-2.244 2.077H8.084a2.25 2.25 0 0 1-2.244-2.077L4.772 5.79m14.456 0a48.108 48.108 0 0 0-3.478-.397m-12 .562c.34-.059.68-.114 1.022-.165m0 0a48.11 48.11 0 0 1 3.478-.397m7.5 0v-.916c0-1.18-.91-2.164-2.09-2.201a51.964 51.964 0 0 0-3.32 0c-1.18.037-2.09 1.022-2.09 2.201v.916m7.5 0a48.667 48.667 0 0 0-7.5 0" />
                          </svg>
                        </button>
                      {/if}
                      <label class="btn btn-ghost btn-xs">
                        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-4">
                          <path stroke-linecap="round" stroke-linejoin="round" d="M3 16.5v2.25A2.25 2.25 0 0 0 5.25 21h13.5A2.25 2.25 0 0 0 21 18.75V16.5m-13.5-9L12 3m0 0 4.5 4.5M12 3v13.5" />
                        </svg>
                        <span class="ml-1">Upload</span>
                        <input
                          type="file"
                          onchange={handleCertificateUpload}
                          accept=".pem,.crt,.cer,.der"
                          class="hidden"
                          data-type="client"
                        />
                      </label>
                    </div>
                  </div>
                  {#if uploadProgress.client > 0 && uploadProgress.client < 100}
                    <progress class="progress progress-primary w-full" value={uploadProgress.client} max="100"></progress>
                  {/if}
                  {#if uploadErrors.client}
                    <p class="text-error text-xs mt-2">{uploadErrors.client}</p>
                  {/if}
                  {#if certificateStatus?.client && certificateStatus?.privateKey?.exists && !certificateStatus.client.keyMatchesCert}
                    <p class="text-error text-xs mt-2">Private Key doesn't match the certificate public key</p>
                  {/if}
                </div>

                <!-- Private Key -->
                <div class="bg-base-100 rounded-lg p-4">
                  <div class="flex items-center justify-between mb-3">
                    <div>
                      <p class="font-medium text-sm text-base-content">Private Key</p>
                      <p class="text-xs text-base-content/70">
                        {certificateStatus?.privateKey?.exists ? 'Uploaded' : 'Not uploaded'}
                      </p>
                    </div>
                    <div class="flex items-center gap-2">
                      {#if certificateStatus?.privateKey?.exists}
                        <button
                          type="button"
                          onclick={() => deleteCertificateHandler('privateKey')}
                          class="btn btn-ghost btn-xs text-error"
                          aria-label="Delete Private Key"
                        >
                          <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-4">
                            <path stroke-linecap="round" stroke-linejoin="round" d="m14.74 9-.346 9m-4.788 0L9.26 9m9.968-3.21c.342.052.682.107 1.022.166m-1.022-.165L18.16 19.673a2.25 2.25 0 0 1-2.244 2.077H8.084a2.25 2.25 0 0 1-2.244-2.077L4.772 5.79m14.456 0a48.108 48.108 0 0 0-3.478-.397m-12 .562c.34-.059.68-.114 1.022-.165m0 0a48.11 48.11 0 0 1 3.478-.397m7.5 0v-.916c0-1.18-.91-2.164-2.09-2.201a51.964 51.964 0 0 0-3.32 0c-1.18.037-2.09 1.022-2.09 2.201v.916m7.5 0a48.667 48.667 0 0 0-7.5 0" />
                          </svg>
                        </button>
                      {/if}
                      <label class="btn btn-ghost btn-xs">
                        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="size-4">
                          <path stroke-linecap="round" stroke-linejoin="round" d="M3 16.5v2.25A2.25 2.25 0 0 0 5.25 21h13.5A2.25 2.25 0 0 0 21 18.75V16.5m-13.5-9L12 3m0 0 4.5 4.5M12 3v13.5" />
                        </svg>
                        <span class="ml-1">Upload</span>
                        <input
                          type="file"
                          onchange={handleCertificateUpload}
                          accept=".pem,.key,.der"
                          class="hidden"
                          data-type="privateKey"
                        />
                      </label>
                    </div>
                  </div>
                  {#if uploadProgress.privateKey > 0 && uploadProgress.privateKey < 100}
                    <progress class="progress progress-primary w-full" value={uploadProgress.privateKey} max="100"></progress>
                  {/if}
                  {#if uploadErrors.privateKey}
                    <p class="text-error text-xs mt-2">{uploadErrors.privateKey}</p>
                  {/if}
                </div>
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
