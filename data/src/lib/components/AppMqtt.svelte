<script lang="ts">
  import { onMount } from "svelte";
  import { type CertificatesStatus, type CertificateType, type MqttConfig } from '$lib/types/api';
  import {
    saveConfig,
    uploadCertificateBundle,
    getCertificateStatus,
    deleteCertificate,
  } from "$lib/services/api.js";

  let { mqtt, error }: { mqtt: MqttConfig | null, error: string | null } = $props();

  let mqttConfig = $state<MqttConfig>(mqtt ?? {
    mqttBroker: "",
    mqttPort: 1883,
    mqttClientId: "",
    mqttUsername: "",
    mqttPassword: "",
    hassMqttDiscoveryEnabled: false,
    lwtTopic: "",
    hkTopic: "",
    lockStateTopic: "",
    lockStateCmd: "",
    lockCStateCmd: "",
    lockTStateCmd: "",
    btrLvlCmdTopic: "",
    hkAltActionTopic: "",
    lockCustomStateTopic: "",
    lockCustomStateCmd: "",
    lockEnableCustomState: false,
    nfcTagNoPublish: false,
    useSSL: false,
    caCert: "",
    clientCert: "",
    clientKey: "",
    allowInsecure: false,
    customLockStates: {}, 
    customLockActions: {}
  });

  let certificateStatus = $state<CertificatesStatus>();

  let reconnectionStatus = $state({
    isReconnecting: false,
    message: "",
    lastReconnection: null as string | null,
  });

  let uploadProgress = $state({ ca: 0, client: 0, privateKey: 0 });

  let uploadErrors = $state({ ca: "", client: "", privateKey: "" });

  // Set config from props
  onMount(() => {
    if (mqtt) {
      fetchCertificateStatus();
    }
  });

  const fetchCertificateStatus = async () => {
    try {
      const response = await getCertificateStatus();
      if (response.success && response.data) {
        certificateStatus = response.data;
      } else {
        console.error("Error fetching certificate status:", response.success);
      }
    } catch (e) {
      console.error("Error fetching certificate status:", e);
    }
  };

  const handleCertificateUpload = async (event: Event) => {
    const type = (event.target as HTMLInputElement).dataset["type"] as keyof typeof uploadErrors;
    const file = (event.target as HTMLInputElement).files?.[0];
    if (!file) return;
    // Reset error
    uploadErrors[type] = "";

    // Validate file type
    const validExtensions = [".pem", ".crt", ".cer", ".der", ".key"];
    const fileNameParts = file.name.split(".");
    const extension = fileNameParts.length > 1 ? fileNameParts.pop()! : "";
    const fileExtension = "." + extension.toLowerCase();

    if (!validExtensions.includes(fileExtension)) {
      uploadErrors.ca =
        "Invalid file type. Must be .pem, .crt, .cer, .der, or .key";
      return;
    }

    // Simulate upload progress
    uploadProgress[type] = 0;

    const reader = new FileReader();
    reader.onloadstart = () => {
      uploadProgress[type] = 10;
    };
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
          uploadErrors[type] = "Failed to read file content";
          uploadProgress[type] = 0;
          return;
        }
        const result = await uploadCertificateBundle(type, content);
        uploadProgress[type] = 100;

        // Refresh certificate status
        await fetchCertificateStatus();

        // Clear file input
        (event.target as HTMLInputElement).value = "";

        // Reset progress after a delay
        setTimeout(() => {
          uploadProgress[type] = 0;
        }, 1000);
      } catch (error) {
        uploadErrors[type] = `Upload failed: ${(error as Error).message}`;
        uploadProgress[type] = 0;
      }
    };
    reader.onerror = () => {
      uploadErrors[type] = "Failed to read file";
      uploadProgress[type] = 0;
    };

    if (fileExtension === ".der") {
      reader.readAsArrayBuffer(file);
    } else {
      reader.readAsText(file);
    }
  };

  const deleteCertificateHandler = async (type: CertificateType) => {
    if (!confirm(`Are you sure you want to delete the ${type} certificate?`)) {
      return;
    }

    try {
      await deleteCertificate(type);
      await fetchCertificateStatus();
    } catch (e) {
      alert(
        `Error deleting certificate: ${e instanceof Error ? e.message : String(e)}`,
      );
    }
  };

  const onSSLToggleChange = () => {
    // If SSL is disabled, reset allowInsecure to false
    if (!mqttConfig.useSSL) {
      mqttConfig!.allowInsecure = false;
    }
  };

  const saveMqttConfig = async () => {
    try {
      if (!mqttConfig) return;
      const result = await saveConfig("mqtt", mqttConfig);
    } catch (e) {
      const message = e instanceof Error ? e.message : String(e);
      alert(`Error saving config: ${message}`); // Replace with a more user-friendly notification
    }
  };

  const resetForm = () => {
    if (mqtt) {
      mqttConfig = mqtt;
    }
  };

</script>

<div class="w-full max-w-4xl py-6">
  <h1 class="md:text-3xl text-2xl font-bold mb-8">
    MQTT Settings
    <div
      class="tooltip tooltip-bottom tooltip-info"
      data-tip="Reboot required to apply!"
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
          d="m3.75 13.5 10.5-11.25L12 10.5h8.25L9.75 21.75 12 13.5H3.75Z"
        />
      </svg>
    </div>
  </h1>
  {#if !mqtt && error}
    <div class="text-center text-error">
      <p>Error: {error}</p>
    </div>
  {:else if mqtt}
    <form onsubmit={saveMqttConfig}>
      <div class="card bg-base-200 shadow-xl">
        <div class="card-body p-4">
          <div class="collapse collapse-arrow bg-base-100">
            <input type="checkbox" name="mqtt-accordion" checked />
            <div class="collapse-title font-medium flex items-center">
              <svg
                xmlns="http://www.w3.org/2000/svg"
                fill="none"
                viewBox="0 0 24 24"
                stroke-width="1.5"
                stroke="currentColor"
                class="w-5 mr-2"
              >
                <path
                  stroke-linecap="round"
                  stroke-linejoin="round"
                  d="M21.75 17.25v-.228a4.5 4.5 0 0 0-.12-1.03l-2.268-9.64a3.375 3.375 0 0 0-3.285-2.602H7.923a3.375 3.375 0 0 0-3.285 2.602l-2.268 9.64a4.5 4.5 0 0 0-.12 1.03v.228m19.5 0a3 3 0 0 1-3 3H5.25a3 3 0 0 1-3-3m19.5 0a3 3 0 0 0-3-3H5.25a3 3 0 0 0-3 3m16.5 0h.008v.008h-.008v-.008Zm-3 0h.008v.008h-.008v-.008Z"
                />
              </svg>
              Broker
            </div>
            <div class="collapse-content">
              <div
                class="text-xs uppercase font-semibold opacity-60 mb-4"
                class:hidden={mqttConfig.useSSL}
              >
                TCP - Without TLS
              </div>
              <div
                class="text-xs uppercase font-semibold opacity-60 mb-4"
                class:hidden={!mqttConfig.useSSL}
              >
                TCP - With TLS/SSL
              </div>
              <div class="grid grid-cols-1 md:grid-cols-2 gap-6">
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Address</span>
                  </label>
                  <input
                    type="text"
                    bind:value={mqttConfig.mqttBroker}
                    placeholder="0.0.0.0"
                    class="input input-bordered w-full"
                    required
                  />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Port</span>
                  </label>
                  <input
                    type="number"
                    bind:value={mqttConfig.mqttPort}
                    placeholder="1883"
                    class="input input-bordered w-full"
                    required
                    inputmode="numeric"
                    min="0"
                    max="65535"
                  />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Client ID</span>
                  </label>
                  <input
                    type="text"
                    bind:value={mqttConfig.mqttClientId}
                    placeholder="homekey_mqtt"
                    class="input input-bordered w-full"
                    required
                  />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">LWT Topic</span>
                  </label>
                  <input
                    type="text"
                    bind:value={mqttConfig.lwtTopic}
                    placeholder="homekey_mqtt/status"
                    class="input input-bordered w-full"
                    required
                  />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Username</span>
                  </label>
                  <input
                    type="text"
                    bind:value={mqttConfig.mqttUsername}
                    placeholder="username"
                    class="input input-bordered w-full"
                  />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Password</span>
                  </label>
                  <input
                    type="password"
                    bind:value={mqttConfig.mqttPassword}
                    placeholder="password"
                    class="input input-bordered w-full"
                  />
                </div>
                <div class="form-control md:col-span-2 mb-4">
                  <label class="label cursor-pointer">
                    <span class="label-text">HASS MQTT Discovery</span>
                    <input
                      type="checkbox"
                      bind:checked={mqttConfig.hassMqttDiscoveryEnabled}
                      class="toggle toggle-primary"
                    />
                  </label>
                </div>
                <div class="form-control md:col-span-2 mb-4">
                  <label class="label cursor-pointer">
                    <span class="label-text">Enable SSL/TLS</span>
                    <input
                      type="checkbox"
                      bind:checked={mqttConfig.useSSL}
                      class="toggle toggle-primary"
                      onchange={onSSLToggleChange}
                    />
                  </label>
                </div>
              </div>
              <!-- SSL/TLS Configuration Section -->
              {#if mqttConfig.useSSL}
                <div class="collapse collapse-arrow bg-base-200">
                  <input type="checkbox" name="mqtt-accordion" />
                  <div class="collapse-title font-medium flex items-center">
                    <svg
                      xmlns="http://www.w3.org/2000/svg"
                      fill="none"
                      viewBox="0 0 24 24"
                      stroke-width="1.5"
                      stroke="currentColor"
                      class="w-5 mr-2"
                    >
                      <path
                        stroke-linecap="round"
                        stroke-linejoin="round"
                        d="M16.5 10.5V6.75a4.5 4.5 0 10-9 0v3.75m-.75 11.25h10.5a2.25 2.25 0 002.25-2.25v-6.75a2.25 2.25 0 00-2.25-2.25H6.75a2.25 2.25 0 00-2.25 2.25v6.75a2.25 2.25 0 002.25 2.25z"
                      />
                    </svg>
                    SSL/TLS Settings
                  </div>
                  <div class="collapse-content">
                    <div class="space-y-6">
                      <div class="form-control">
                        <label class="label cursor-pointer">
                          <span class="label-text">Skip server cert validation</span
                          >
                          <input
                            type="checkbox"
                            bind:checked={mqttConfig.allowInsecure}
                            class="toggle toggle-warning"
                          />
                        </label>
                      </div>

                      <!-- Certificate Status Display -->
                      <div class="bg-base-200 rounded-lg">
                        <!-- Reconnection Status Alert -->
                        {#if reconnectionStatus.isReconnecting}
                          <div class="alert alert-info mb-4">
                            <svg
                              xmlns="http://www.w3.org/2000/svg"
                              fill="none"
                              viewBox="0 0 24 24"
                              stroke-width="1.5"
                              stroke="currentColor"
                              class="w-6 h-6"
                            >
                              <path
                                stroke-linecap="round"
                                stroke-linejoin="round"
                                d="M16.023 9.348h4.992v-.001M2.985 19.644v-4.992m0 0h4.992m-4.993 0l3.181 3.183a8.25 8.25 0 0011.664 0l3.181-3.183m-3.181-4.991v4.99"
                              />
                            </svg>
                            <span>{reconnectionStatus.message}</span>
                          </div>
                        {/if}

                        <div class="grid grid-cols-1 md:grid-cols-3 gap-4">
                          <!-- CA Certificate -->
                          <div class="flex flex-col p-3 bg-base-100 rounded-lg">
                            <div class="flex items-center justify-between mb-2">
                              <div class="flex items-center">
                                <svg
                                  class="w-5 h-5 mr-2"
                                  class:text-success={certificateStatus?.certificates?.ca}
                                  class:text-error={!certificateStatus?.certificates?.ca}
                                  fill="none"
                                  viewBox="0 0 24 24"
                                  stroke="currentColor"
                                >
                                  <path
                                    stroke-linecap="round"
                                    stroke-linejoin="round"
                                    d="M9 12.75L11.25 15 15 9.75M21 12a9 9 0 11-18 0 9 9 0 0118 0z"
                                  />
                                </svg>
                                <div class="flex flex-col">
                                  <span class="text-sm font-medium">CA Certificate</span>
                                  <!-- <span class="text-sm font-medium text-base-content/75">{certificateStatus?.certificates.ca?.validationMessage}</span> -->
                                  <span class="text-sm font-medium text-base-content/75">{certificateStatus?.certificates.ca?.expiration.from} - {certificateStatus?.certificates.ca?.expiration.to}</span>
                                  <span class="text-sm font-medium text-base-content/50">Subject: {certificateStatus?.certificates.ca?.subject}</span>
                                  <span class="text-sm font-medium text-base-content/50">Issuer: {certificateStatus?.certificates.ca?.issuer}</span>
                                </div>
                              </div>
                              {#if certificateStatus?.certificates?.ca}
                                <button
                                  onclick={() => deleteCertificateHandler("ca")}
                                  class="btn btn-ghost btn-xs btn-error"
                                >Delete</button
                                >
                              {/if}
                            </div>
                          </div>

                          <!-- Client Certificate -->
                          <div class="flex flex-col p-3 bg-base-100 rounded-lg">
                            <div class="flex items-center justify-between mb-2">
                              <div class="flex items-center">
                                <svg
                                  class="w-5 h-5 mr-2"
                                  class:text-success={certificateStatus?.certificates?.client}
                                  class:text-error={!certificateStatus?.certificates?.client}
                                  fill="none"
                                  viewBox="0 0 24 24"
                                  stroke="currentColor"
                                >
                                  <path
                                    stroke-linecap="round"
                                    stroke-linejoin="round"
                                    d="M9 12.75L11.25 15 15 9.75M21 12a9 9 0 11-18 0 9 9 0 0118 0z"
                                  />
                                </svg>
                                <div class="flex flex-col">
                                  <span class="text-sm font-medium">Client Certificate</span>
                                  <!-- <span class="text-sm font-medium text-base-content/75">{certificateStatus?.certificates.client?.validationMessage}</span> -->
                                  <span class="text-sm font-medium text-base-content/75">{certificateStatus?.certificates.client?.expiration.from} - {certificateStatus?.certificates.client?.expiration.to}</span>
                                  <span class="text-sm font-medium text-base-content/50">Subject: {certificateStatus?.certificates.client?.subject}</span>
                                  <span class="text-sm font-medium text-base-content/50">Issuer: {certificateStatus?.certificates.client?.issuer}</span>
                                </div>
                              </div>
                              {#if certificateStatus?.certificates?.client}
                                <button
                                  onclick={() => deleteCertificateHandler("client")}
                                  class="btn btn-ghost btn-xs btn-error"
                                >Delete</button
                                >
                              {/if}
                            </div>
                          </div>

                          <!-- Private Key -->
                          <div class="flex flex-col p-3 bg-base-100 rounded-lg">
                            <div class="flex items-center justify-between mb-2">
                              <div class="flex items-center">
                                <svg
                                  class="w-5 h-5 mr-2"
                                  class:text-success={certificateStatus?.certificates?.privateKey?.exists}
                                  class:text-error={!certificateStatus?.certificates?.privateKey?.exists}
                                  fill="none"
                                  viewBox="0 0 24 24"
                                  stroke="currentColor"
                                >
                                  <path
                                    stroke-linecap="round"
                                    stroke-linejoin="round"
                                    d="M9 12.75L11.25 15 15 9.75M21 12a9 9 0 11-18 0 9 9 0 0118 0z"
                                  />
                                </svg>
                                <div class="flex flex-col">
                                  <span class="text-sm font-medium">Private Key</span>
                                  <!-- <span class="text-sm font-medium text-base-content/50">{certificateStatus?.certificates.privateKey?.validationMessage}</span> -->
                                </div>
                              </div>
                              {#if certificateStatus?.certificates?.privateKey}
                                <button
                                  onclick={() =>
                                    deleteCertificateHandler("privateKey")}
                                  class="btn btn-ghost btn-xs btn-error"
                                >Delete</button
                                >
                              {/if}
                            </div>
                          </div>
                        </div>
                      </div>

                      <!-- Certificate Upload Section -->
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">CA Certificate</span>
                        </label>
                        <input
                          type="file"
                          onchange={(e) => handleCertificateUpload(e)}
                          accept=".pem,.crt,.cer,.der,.key"
                          class="file-input file-input-bordered w-full"
                          disabled={uploadProgress.ca > 0 &&
                            uploadProgress.ca < 100}
                          data-type="ca"
                        />
                        {#if uploadProgress.ca > 0 && uploadProgress.ca < 100}
                          <div class="progress progress-info mt-2">
                            <div
                              class="progress-bar"
                              style="width: {uploadProgress.ca}%"
                            ></div>
                          </div>
                        {/if}
                        {#if uploadErrors.ca}
                          <div class="text-error text-sm mt-1">
                            {uploadErrors.ca}
                          </div>
                        {/if}
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Client Certificate</span>
                        </label>
                        <input
                          type="file"
                          onchange={(e) => handleCertificateUpload(e)}
                          accept=".pem,.crt,.cer,.der,.key"
                          class="file-input file-input-bordered w-full"
                          disabled={uploadProgress.ca > 0 &&
                            uploadProgress.ca < 100}
                          data-type="client"
                        />
                        {#if uploadProgress.ca > 0 && uploadProgress.ca < 100}
                          <div class="progress progress-info mt-2">
                            <div
                              class="progress-bar"
                              style="width: {uploadProgress.ca}%"
                            ></div>
                          </div>
                        {/if}
                        {#if uploadErrors.ca}
                          <div class="text-error text-sm mt-1">
                            {uploadErrors.ca}
                          </div>
                        {/if}
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Private Key</span>
                        </label>
                        <input
                          type="file"
                          onchange={(e) => handleCertificateUpload(e)}
                          accept=".pem,.crt,.cer,.der,.key"
                          class="file-input file-input-bordered w-full"
                          disabled={uploadProgress.ca > 0 &&
                            uploadProgress.ca < 100}
                          data-type="privateKey"
                        />
                        {#if uploadProgress.ca > 0 && uploadProgress.ca < 100}
                          <div class="progress progress-info mt-2">
                            <div
                              class="progress-bar"
                              style="width: {uploadProgress.ca}%"
                            ></div>
                          </div>
                        {/if}
                        {#if uploadErrors.ca}
                          <div class="text-error text-sm mt-1">
                            {uploadErrors.ca}
                          </div>
                        {/if}
                      </div>
                    </div>
                  </div>
                </div>
              {/if}
            </div>
          </div>


          <div class="collapse collapse-arrow bg-base-100">
            <input type="checkbox" name="mqtt-accordion" />
            <div class="collapse-title font-medium flex items-center">
              <svg
                xmlns="http://www.w3.org/2000/svg"
                fill="none"
                viewBox="0 0 24 24"
                stroke-width="1.5"
                stroke="currentColor"
                class="w-5 mr-2"
              >
                <path
                  stroke-linecap="round"
                  stroke-linejoin="round"
                  d="M21.75 6.75a4.5 4.5 0 0 1-4.884 4.484c-1.076-.091-2.264.071-2.95.904l-7.152 8.684a2.548 2.548 0 1 1-3.586-3.586l8.684-7.152c.833-.686.995-1.874.904-2.95a4.5 4.5 0 0 1 6.336-4.486l-3.276 3.276a3.004 3.004 0 0 0 2.25 2.25l3.276-3.276c.256.565.398 1.192.398 1.852Z"
                />
                <path
                  stroke-linecap="round"
                  stroke-linejoin="round"
                  d="M4.867 19.125h.008v.008h-.008v-.008Z"
                />
              </svg>
              Topics
            </div>
            <div class="collapse-content">
              <div class="space-y-2">
                <div class="collapse collapse-arrow bg-base-200">
                  <input type="checkbox" name="topics-accordion" />
                  <div class="collapse-title font-medium">Core</div>
                  <div class="collapse-content">
                    <div class="grid grid-cols-1 md:grid-cols-2 gap-6">
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">NFC/HK Topic</span>
                        </label>
                        <input
                          type="text"
                          bind:value={mqttConfig.hkTopic}
                          placeholder="topic/auth"
                          class="input input-bordered w-full"
                          required
                        />
                      </div>
                      <div class="form-control">
                        <label class="label cursor-pointer">
                          <span class="label-text">Ignore NFC Tags</span>
                          <input
                            type="checkbox"
                            bind:checked={mqttConfig.nfcTagNoPublish}
                            class="toggle toggle-primary"
                          />
                        </label>
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Secondary action Topic</span>
                        </label>
                        <input
                          type="text"
                          bind:value={mqttConfig.hkAltActionTopic}
                          placeholder="topic/alt_action"
                          class="input input-bordered w-full"
                          required
                        />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Lock State Topic</span>
                        </label>
                        <input
                          type="text"
                          bind:value={mqttConfig.lockStateTopic}
                          placeholder="topic/state"
                          class="input input-bordered w-full"
                          required
                        />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Lock State Cmd Topic</span>
                        </label>
                        <input
                          type="text"
                          bind:value={mqttConfig.lockStateCmd}
                          placeholder="topic/set_state"
                          class="input input-bordered w-full"
                          required
                        />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text"
                            >Lock Current State Cmd Topic</span
                          >
                        </label>
                        <input
                          type="text"
                          bind:value={mqttConfig.lockCStateCmd}
                          placeholder="topic/set_current_state"
                          class="input input-bordered w-full"
                          required
                        />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text"
                            >Lock Target State Cmd Topic</span
                          >
                        </label>
                        <input
                          type="text"
                          bind:value={mqttConfig.lockTStateCmd}
                          placeholder="topic/set_target_state"
                          class="input input-bordered w-full"
                          required
                        />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text"
                            >SmartLock battery level Cmd Topic</span
                          >
                        </label>
                        <input
                          type="text"
                          bind:value={mqttConfig.btrLvlCmdTopic}
                          placeholder="topic/set_battery_level"
                          class="input input-bordered w-full"
                          required
                        />
                      </div>
                    </div>
                  </div>
                </div>
                <div class="collapse collapse-arrow bg-base-200">
                  <input type="checkbox" name="topics-accordion" />
                  <div class="collapse-title font-medium">Custom</div>
                  <div class="collapse-content">
                    <div class="form-control mb-4">
                      <label class="label cursor-pointer">
                        <span class="label-text">Enable Custom State</span>
                        <input
                          type="checkbox"
                          bind:checked={mqttConfig.lockEnableCustomState}
                          class="toggle toggle-primary"
                        />
                      </label>
                    </div>
                    {#if mqttConfig.lockEnableCustomState}
                      <div class="form-control mb-4">
                        <label class="label">
                          <span class="label-text">Custom State Topic</span>
                        </label>
                        <input
                          type="text"
                          bind:value={mqttConfig.lockCustomStateTopic}
                          placeholder="topic/set_target_state"
                          class="input input-bordered w-full"
                        />
                      </div>
                      <div class="form-control mb-4">
                        <label class="label">
                          <span class="label-text">Custom State Cmd Topic</span>
                        </label>
                        <input
                          type="text"
                          bind:value={mqttConfig.lockCustomStateCmd}
                          placeholder="topic/set_target_state"
                          class="input input-bordered w-full"
                        />
                      </div>
                      <div class="divider">Custom Lock Actions</div>
                      <div class="grid grid-cols-2 gap-6">
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Unlock</span>
                          </label>
                          <input
                            type="number"
                            bind:value={mqttConfig.customLockActions!.UNLOCK}
                            placeholder="255"
                            class="input input-bordered w-full"
                            min="0"
                            max="255"
                          />
                        </div>
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Lock</span>
                          </label>
                          <input
                            type="number"
                            bind:value={mqttConfig.customLockActions!.LOCK}
                            placeholder="255"
                            class="input input-bordered w-full"
                            min="0"
                            max="255"
                          />
                        </div>
                      </div>
                      <div class="divider">Custom Lock States</div>
                      <div class="grid grid-cols-2 md:grid-cols-3 gap-6">
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Unlocking</span>
                          </label>
                          <input
                            type="number"
                            bind:value={mqttConfig.customLockStates!.C_UNLOCKING}
                            placeholder="255"
                            class="input input-bordered w-full"
                            min="0"
                            max="255"
                          />
                        </div>
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Locking</span>
                          </label>
                          <input
                            type="number"
                            bind:value={mqttConfig.customLockStates!.C_LOCKING}
                            placeholder="255"
                            class="input input-bordered w-full"
                            min="0"
                            max="255"
                          />
                        </div>
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Unlocked</span>
                          </label>
                          <input
                            type="number"
                            bind:value={mqttConfig.customLockStates!.C_UNLOCKED}
                            placeholder="255"
                            class="input input-bordered w-full"
                            min="0"
                            max="255"
                          />
                        </div>
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Locked</span>
                          </label>
                          <input
                            type="number"
                            bind:value={mqttConfig.customLockStates!.C_LOCKED}
                            placeholder="255"
                            class="input input-bordered w-full"
                            min="0"
                            max="255"
                          />
                        </div>
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Jammed</span>
                          </label>
                          <input
                            type="number"
                            bind:value={mqttConfig.customLockStates!.C_JAMMED}
                            placeholder="255"
                            class="input input-bordered w-full"
                            min="0"
                            max="255"
                          />
                        </div>
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Unknown</span>
                          </label>
                          <input
                            type="number"
                            bind:value={mqttConfig.customLockStates!.C_UNKNOWN}
                            placeholder="255"
                            class="input input-bordered w-full"
                            min="0"
                            max="255"
                          />
                        </div>
                      </div>
                    {/if}
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
      <div class="card-actions justify-end mt-6 px-2">
        <button type="submit" class="btn btn-primary">Save & Apply</button>
        <button type="button" class="btn btn-ghost" onclick={resetForm}
          >Reset Form</button
        >
      </div>
    </form>
  {/if}
</div>

