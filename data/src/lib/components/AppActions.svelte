<script lang="ts">
  import { saveConfig } from '$lib/services/api';
  import type { ActionsConfig } from '$lib/types/api';
  import { diff } from '$lib/utils/objDiff';

  let {
    actions,
    error,
  }: { actions: ActionsConfig; error?: string | null } = $props();

  let activeTab = $state<'nfc' | 'state'>('nfc');

  // svelte-ignore state_referenced_locally
  let actionsConfig = $state<ActionsConfig>($state.snapshot(actions));

  const saveActionsConfig = async (e: any): Promise<void> => {
    try {
      e.preventDefault();
      if (!actionsConfig || !actions) return;
      const result = await saveConfig('actions', diff(actions, actionsConfig));
      if (result.success) {
        actionsConfig = result.data;
        actions = result.data;
      }
    } catch (e) {
      const message = e instanceof Error ? e.message : String(e);
      error = message;
      alert(`Error saving config: ${message}`);
    }
  };

  const resetForm = (): void => {
    actionsConfig = actions!;
  };
</script>

<div class="w-full py-6">
  <!-- Header -->
  <div class="mb-6">
    <h1 class="text-2xl font-bold text-base-content flex items-center gap-2">
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
          d="M13.5 16.875h3.375m0 0h3.375m-3.375 0V13.5m0 3.375v3.375M6 10.5h2.25a2.25 2.25 0 0 0 2.25-2.25V6a2.25 2.25 0 0 0-2.25-2.25H6A2.25 2.25 0 0 0 3.75 6v2.25A2.25 2.25 0 0 0 6 10.5Zm0 9.75h2.25A2.25 2.25 0 0 0 10.5 18v-2.25a2.25 2.25 0 0 0-2.25-2.25H6a2.25 2.25 0 0 0-2.25 2.25V18A2.25 2.25 0 0 0 6 20.25Zm9.75-9.75H18a2.25 2.25 0 0 0 2.25-2.25V6A2.25 2.25 0 0 0 18 3.75h-2.25A2.25 2.25 0 0 0 13.5 6v2.25a2.25 2.25 0 0 0 2.25 2.25Z"
        />
      </svg>
      Hardware Actions
    </h1>
    <p class="text-sm text-base-content/60">Configure GPIO and NeoPixel actions for NFC and state events.</p>
  </div>

  {#if !actions && error}
    <div class="text-center text-error">
      <p>Error: {error}</p>
    </div>
  {:else if actions}
    <form onsubmit={saveActionsConfig} class="max-w-5xl">
      <!-- Tabs -->
      <div class="flex bg-base-300 p-1 rounded-xl gap-1 mb-4">
        <button
          type="button"
          class="flex-1 tab flex-col py-2 rounded-lg transition-colors {activeTab === 'nfc' ? 'bg-base-100 text-primary font-medium shadow-sm' : 'text-base-content/60 hover:bg-base-200'}"
          onclick={() => activeTab = 'nfc'}
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
              d="M8.288 15.038a5.25 5.25 0 0 1 7.424 0M5.106 11.856c3.807-3.808 9.98-3.808 13.788 0M1.924 8.674c5.565-5.565 14.587-5.565 20.152 0M12.53 18.22l-.53.53-.53-.53a.75.75 0 0 1 1.06 0Z"
            />
          </svg>
          <span class="text-[10px] sm:text-xs mt-0.5">NFC Triggers</span>
        </button>
        <button
          type="button"
          class="flex-1 tab flex-col py-2 rounded-lg transition-colors {activeTab === 'state' ? 'bg-base-100 text-primary font-medium shadow-sm' : 'text-base-content/60 hover:bg-base-200'}"
          onclick={() => activeTab = 'state'}
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
              d="M3 12l2-2m0 0l7-7 7 7M5 10v10a1 1 0 001 1h3m10-11l2 2m-2-2v10a1 1 0 01-1 1h-3m-6 0a1 1 0 001-1v-4a1 1 0 011-1h2a1 1 0 011 1v4a1 1 0 001 1m-6 0h6"
            />
          </svg>
          <span class="text-[10px] sm:text-xs mt-0.5">State Triggers</span>
        </button>
      </div>

      <!-- Tab Content -->
      {#if activeTab === 'nfc'}
        <div class="space-y-2">
          <div class="collapse collapse-arrow bg-base-200">
                <input type="checkbox" name="pixel-nfc-collapse" checked />
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
                      d="M11.48 3.499a.562.562 0 0 1 1.04 0l2.125 5.111a.563.563 0 0 0 .475.345l5.518.442c.499.04.701.663.321.988l-4.204 3.602a.563.563 0 0 0-.182.557l1.285 5.385a.562.562 0 0 1-.84.61l-4.725-2.885a.562.562 0 0 0-.586 0L6.982 20.54a.562.562 0 0 1-.84-.61l1.285-5.386a.562.562 0 0 0-.182-.557l-4.204-3.602a.562.562 0 0 1 .321-.988l5.518-.442a.563.563 0 0 0 .475-.345L11.48 3.5Z"
                    />
                  </svg>
                  Pixel
                </div>
                <div class="collapse-content">
                  <div class="grid grid-cols-1 md:grid-cols-2 gap-6 mb-6">
                    <div class="form-control">
                      <!-- svelte-ignore a11y_label_has_associated_control -->
                      <label class="label">
                        <span class="label-text text-sm">GPIO Pin</span>
                      </label>
                      <input
                        type="number"
                        bind:value={actionsConfig.nfcNeopixelPin}
                        placeholder="8"
                        class="input input-bordered w-full"
                      />
                    </div>
                    <div class="form-control">
                      <!-- svelte-ignore a11y_label_has_associated_control -->
                      <label class="label">
                        <span class="label-text text-sm">Pixel Type</span>
                      </label>
                      <select
                        bind:value={actionsConfig.neoPixelType}
                        class="select select-bordered w-full"
                      >
                        <option value={0}>RGB</option>
                        <option value={1}>RBG</option>
                        <option value={2}>BRG</option>
                        <option value={3}>BGR</option>
                        <option value={4}>GBR</option>
                        <option value={5}>GRB</option>
                      </select>
                    </div>
                  </div>

                  <div class="flex max-md:flex-col max-md:gap-8 gap-4">
                    <!-- Auth Success Group -->
                    <fieldset class="fieldset border-base-300 rounded-box flex-1 border p-4">
                      <legend class="fieldset-legend flex items-center gap-2">
                        Auth Success
                        <div
                          class="w-4 h-4 rounded border border-base-300"
                          style="background-color: rgb({actionsConfig.neopixelSuccessColor[0][1] ?? 0}, {actionsConfig.neopixelSuccessColor[1][1] ?? 0}, {actionsConfig.neopixelSuccessColor[2][1] ?? 0});"
                        ></div>
                      </legend>
                      <div class="form-control mb-4">
                        <!-- svelte-ignore a11y_label_has_associated_control -->
                        <label class="label">
                          <span class="label-text text-sm">Timeout (ms)</span>
                        </label>
                        <input
                          type="number"
                          bind:value={actionsConfig.neopixelSuccessTime}
                          placeholder="1000"
                          class="input input-bordered w-full"
                        />
                      </div>
                      <div class="form-control">
                        <!-- svelte-ignore a11y_label_has_associated_control -->
                        <label class="label">
                          <span class="label-text text-sm">Color</span>
                        </label>
                        <div class="flex gap-2">
                          <div class="form-control flex-1">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm flex items-center gap-1 text-xs">R <div class="w-2 h-2 rounded-sm" style="background-color: rgb({actionsConfig.neopixelSuccessColor[0][1] ?? 0}, 0, 0);"></div></span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.neopixelSuccessColor[0][1]}
                              placeholder="255"
                              min="0"
                              max="255"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control flex-1">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm flex items-center gap-1 text-xs">G <div class="w-2 h-2 rounded-sm" style="background-color: rgb(0, {actionsConfig.neopixelSuccessColor[1][1] ?? 0}, 0);"></div></span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.neopixelSuccessColor[1][1]}
                              placeholder="255"
                              min="0"
                              max="255"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control flex-1">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm flex items-center gap-1 text-xs">B <div class="w-2 h-2 rounded-sm" style="background-color: rgb(0, 0, {actionsConfig.neopixelSuccessColor[2][1] ?? 0});"></div></span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.neopixelSuccessColor[2][1]}
                              placeholder="255"
                              min="0"
                              max="255"
                              class="input input-bordered w-full"
                            />
                          </div>
                        </div>
                      </div>
                    </fieldset>

                    <!-- Auth Failure Group -->
                    <fieldset class="fieldset border-base-300 rounded-box flex-1 border p-4">
                      <legend class="fieldset-legend flex items-center gap-2">
                        Auth Failure
                        <div
                          class="w-4 h-4 rounded border border-base-300"
                          style="background-color: rgb({actionsConfig.neopixelFailureColor[0][1] ?? 0}, {actionsConfig.neopixelFailureColor[1][1] ?? 0}, {actionsConfig.neopixelFailureColor[2][1] ?? 0});"
                        ></div>
                      </legend>
                      <div class="form-control mb-4">
                        <!-- svelte-ignore a11y_label_has_associated_control -->
                        <label class="label">
                          <span class="label-text text-sm">Timeout (ms)</span>
                        </label>
                        <input
                          type="number"
                          bind:value={actionsConfig.neopixelFailTime}
                          placeholder="1000"
                          class="input input-bordered w-full"
                        />
                      </div>
                      <div class="form-control">
                        <!-- svelte-ignore a11y_label_has_associated_control -->
                        <label class="label">
                          <span class="label-text text-sm">Color</span>
                        </label>
                        <div class="flex gap-2">
                          <div class="form-control flex-1">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm flex items-center gap-1 text-xs">R <div class="w-2 h-2 rounded-sm" style="background-color: rgb({actionsConfig.neopixelFailureColor[0][1] ?? 0}, 0, 0);"></div></span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.neopixelFailureColor[0][1]}
                              placeholder="255"
                              min="0"
                              max="255"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control flex-1">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm flex items-center gap-1 text-xs">G <div class="w-2 h-2 rounded-sm" style="background-color: rgb(0, {actionsConfig.neopixelFailureColor[1][1] ?? 0}, 0);"></div></span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.neopixelFailureColor[1][1]}
                              placeholder="255"
                              min="0"
                              max="255"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control flex-1">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm flex items-center gap-1 text-xs">B <div class="w-2 h-2 rounded-sm" style="background-color: rgb(0, 0, {actionsConfig.neopixelFailureColor[2][1] ?? 0});"></div></span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.neopixelFailureColor[2][1]}
                              placeholder="255"
                              min="0"
                              max="255"
                              class="input input-bordered w-full"
                            />
                          </div>
                        </div>
                      </div>
                    </fieldset>

                    <!-- Tag Event Group -->
                    <fieldset class="fieldset border-base-300 rounded-box flex-1 border p-4">
                      <legend class="fieldset-legend flex items-center gap-2">
                        Tag Event
                        <div
                          class="w-4 h-4 rounded border border-base-300"
                          style="background-color: rgb({actionsConfig.neopixelTagEventColor[0][1] ?? 0}, {actionsConfig.neopixelTagEventColor[1][1] ?? 0}, {actionsConfig.neopixelTagEventColor[2][1] ?? 0});"
                        ></div>
                      </legend>
                      <div class="form-control mb-4">
                        <!-- svelte-ignore a11y_label_has_associated_control -->
                        <label class="label">
                          <span class="label-text text-sm">Timeout (ms)</span>
                        </label>
                        <input
                          type="number"
                          bind:value={actionsConfig.neopixelTagEventTime}
                          placeholder="1000"
                          class="input input-bordered w-full"
                        />
                      </div>
                      <div class="form-control">
                        <!-- svelte-ignore a11y_label_has_associated_control -->
                        <label class="label">
                          <span class="label-text text-sm">Color</span>
                        </label>
                        <div class="flex gap-2">
                          <div class="form-control flex-1">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm flex items-center gap-1 text-xs">R <div class="w-2 h-2 rounded-sm" style="background-color: rgb({actionsConfig.neopixelTagEventColor[0][1] ?? 0}, 0, 0);"></div></span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.neopixelTagEventColor[0][1]}
                              placeholder="255"
                              min="0"
                              max="255"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control flex-1">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm flex items-center gap-1 text-xs">G <div class="w-2 h-2 rounded-sm" style="background-color: rgb(0, {actionsConfig.neopixelTagEventColor[1][1] ?? 0}, 0);"></div></span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.neopixelTagEventColor[1][1]}
                              placeholder="255"
                              min="0"
                              max="255"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control flex-1">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm flex items-center gap-1 text-xs">B <div class="w-2 h-2 rounded-sm" style="background-color: rgb(0, 0, {actionsConfig.neopixelTagEventColor[2][1] ?? 0});"></div></span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.neopixelTagEventColor[2][1]}
                              placeholder="255"
                              min="0"
                              max="255"
                              class="input input-bordered w-full"
                            />
                          </div>
                        </div>
                      </div>
                    </fieldset>
                  </div>
                </div>
              </div>
              <div class="collapse collapse-arrow bg-base-200">
                <input type="checkbox" name="simple-gpio-nfc--collapse" />
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
                      d="M13.5 16.875h3.375m0 0h3.375m-3.375 0V13.5m0 3.375v3.375M6 10.5h2.25a2.25 2.25 0 0 0 2.25-2.25V6a2.25 2.25 0 0 0-2.25-2.25H6A2.25 2.25 0 0 0 3.75 6v2.25A2.25 2.25 0 0 0 6 10.5Zm0 9.75h2.25A2.25 2.25 0 0 0 10.5 18v-2.25a2.25 2.25 0 0 0-2.25-2.25H6a2.25 2.25 0 0 0-2.25 2.25V18A2.25 2.25 0 0 0 6 20.25Zm9.75-9.75H18a2.25 2.25 0 0 0 2.25-2.25V6A2.25 2.25 0 0 0 18 3.75h-2.25A2.25 2.25 0 0 0 13.5 6v2.25a2.25 2.25 0 0 0 2.25 2.25Z"
                    />
                  </svg>
                  Simple GPIO
                </div>
                <div class="collapse-content flex flex-col gap-2">
                  <div class="collapse collapse-plus bg-base-100">
                    <h3
                      class="collapse-title text-base"
                    >
                      HomeKey Authentication
                    </h3>
                    <input type="checkbox" name="hk-auth-collapse" />
                    <div class="collapse-content">
                      <div class="flex flex-wrap justify-around gap-2 mb-4">
                        <fieldset
                          class="fieldset border-base-300 rounded-box w-xs border p-4"
                        >
                          <legend class="fieldset-legend"
                            >Auth Success</legend
                          >
                          <div class="form-control mb-4">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm">GPIO Pin</span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.nfcSuccessPin}
                              placeholder="2"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control mb-4">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm">Timeout (ms)</span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.nfcSuccessTime}
                              placeholder="1000"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control mb-4">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm">GPIO State</span>
                            </label>
                            <select
                              bind:value={actionsConfig.nfcSuccessHL}
                              class="select select-bordered w-full"
                            >
                              <option value={false}>LOW</option>
                              <option value={true}>HIGH</option>
                            </select>
                          </div>
                        </fieldset>
                        <fieldset
                          class="fieldset border-base-300 rounded-box w-xs border p-4"
                        >
                          <legend class="fieldset-legend"
                            >Auth Failure</legend
                          >
                          <div class="form-control mb-4">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm">GPIO Pin</span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.nfcFailPin}
                              placeholder="2"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control mb-4">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm">Timeout (ms)</span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.nfcFailTime}
                              placeholder="1000"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control mb-4">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm">GPIO State</span>
                            </label>
                            <select
                              bind:value={actionsConfig.nfcFailHL}
                              class="select select-bordered w-full"
                            >
                              <option value={false}>LOW</option>
                              <option value={true}>HIGH</option>
                            </select>
                          </div>
                        </fieldset>
                      </div>
                      <div class="collapse collapse-plus bg-base-200">
                        <h3
                          class="collapse-title text-base"
                        >
                          Second action on success
                        </h3>
                        <input type="checkbox" name="2nd-action-collapse" />
                        <div class="collapse-content flex flex-col gap-4">
                          <div class="form-control">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm">Initiator Pin</span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.hkAltActionInitPin}
                              placeholder="255"
                              min="0"
                              max="255"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm"
                                >Initiator Timeout (ms)</span
                              >
                            </label>
                            <input
                              type="number"
                              bind:value={
                                actionsConfig.hkAltActionInitTimeout
                              }
                              placeholder="5000"
                              min="0"
                              max="10000"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm"
                                >Feedback LED Pin</span
                              >
                            </label>
                            <input
                              type="number"
                              bind:value={
                                actionsConfig.hkAltActionInitLedPin
                              }
                              placeholder="255"
                              min="0"
                              max="255"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm">GPIO Pin</span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.hkAltActionPin}
                              placeholder="2"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm">Timeout (ms)</span>
                            </label>
                            <input
                              type="number"
                              bind:value={actionsConfig.hkAltActionTimeout}
                              placeholder="1000"
                              class="input input-bordered w-full"
                            />
                          </div>
                          <div class="form-control">
                            <!-- svelte-ignore a11y_label_has_associated_control -->
                            <label class="label">
                              <span class="label-text text-sm">GPIO State</span>
                            </label>
                            <select
                              bind:value={
                                actionsConfig.hkAltActionGpioState
                              }
                              class="select select-bordered w-full"
                            >
                              <option value={0}>LOW</option>
                              <option value={1}>HIGH</option>
                            </select>
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>
                  <div class="collapse collapse-plus bg-base-100">
                    <h3
                      class="collapse-title text-base"
                    >
                      NFC Tag Event
                    </h3>
                    <input type="checkbox" name="tag-event-collapse" />
                    <div class="collapse-content">
                      <div class="form-control mb-4">
                        <!-- svelte-ignore a11y_label_has_associated_control -->
                        <label class="label">
                          <span class="label-text text-sm">GPIO Pin</span>
                        </label>
                        <input
                          type="number"
                          bind:value={actionsConfig.tagEventPin}
                          placeholder="2"
                          class="input input-bordered w-full"
                        />
                      </div>
                      <div class="form-control mb-4">
                        <!-- svelte-ignore a11y_label_has_associated_control -->
                        <label class="label">
                          <span class="label-text text-sm">Timeout (ms)</span>
                        </label>
                        <input
                          type="number"
                          bind:value={actionsConfig.tagEventTimeout}
                          placeholder="1000"
                          class="input input-bordered w-full"
                        />
                      </div>
                      <div class="form-control mb-4">
                        <!-- svelte-ignore a11y_label_has_associated_control -->
                        <label class="label">
                          <span class="label-text text-sm">GPIO State</span>
                        </label>
                        <select
                          bind:value={actionsConfig.tagEventHL}
                          class="select select-bordered w-full"
                        >
                          <option value={false}>LOW</option>
                          <option value={true}>HIGH</option>
                        </select>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
      {:else if activeTab === 'state'}
        <div class="space-y-2">
              <div class="collapse collapse-arrow bg-base-200">
                <input
                  type="checkbox"
                  name="gpio-state-trigger-collapse"
                  checked
                />
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
                      d="M13.5 16.875h3.375m0 0h3.375m-3.375 0V13.5m0 3.375v3.375M6 10.5h2.25a2.25 2.25 0 0 0 2.25-2.25V6a2.25 2.25 0 0 0-2.25-2.25H6A2.25 2.25 0 0 0 3.75 6v2.25A2.25 2.25 0 0 0 6 10.5Zm0 9.75h2.25A2.25 2.25 0 0 0 10.5 18v-2.25a2.25 2.25 0 0 0-2.25-2.25H6a2.25 2.25 0 0 0-2.25 2.25V18A2.25 2.25 0 0 0 6 20.25Zm9.75-9.75H18a2.25 2.25 0 0 0 2.25-2.25V6A2.25 2.25 0 0 0 18 3.75h-2.25A2.25 2.25 0 0 0 13.5 6v2.25a2.25 2.25 0 0 0 2.25 2.25Z"
                    />
                  </svg>
                  Simple GPIO
                </div>
                <div class="collapse-content">
                  <div class="grid grid-cols-1 md:grid-cols-2 gap-6">
                    <div class="form-control">
                      <!-- svelte-ignore a11y_label_has_associated_control -->
                      <label class="label">
                        <span class="label-text text-sm">GPIO Pin</span>
                      </label>
                      <input
                        type="number"
                        bind:value={actionsConfig.gpioActionPin}
                        placeholder="2"
                        class="input input-bordered w-full"
                      />
                    </div>
                    <div class="form-control">
                      <!-- svelte-ignore a11y_label_has_associated_control -->
                      <label class="label">
                        <span class="label-text text-sm">GPIO State - Locked</span>
                      </label>
                      <select
                        bind:value={actionsConfig.gpioActionLockState}
                        class="select select-bordered w-full"
                      >
                        <option value={false}>LOW</option>
                        <option value={true}>HIGH</option>
                      </select>
                    </div>
                    <div class="form-control">
                      <!-- svelte-ignore a11y_label_has_associated_control -->
                      <label class="label">
                        <span class="label-text text-sm">GPIO State - Unlocked</span
                        >
                      </label>
                      <select
                        bind:value={actionsConfig.gpioActionUnlockState}
                        class="select select-bordered w-full"
                      >
                        <option value={false}>LOW</option>
                        <option value={true}>HIGH</option>
                      </select>
                    </div>
                    <div class="form-control">
                      <!-- svelte-ignore a11y_label_has_associated_control -->
                      <label class="label">
                        <span class="label-text text-sm">Actionable by HomeKey</span
                        >
                      </label>
                      <select
                        bind:value={actionsConfig.hkGpioControlledState}
                        class="select select-bordered w-full"
                      >
                        <option value={false}>Disabled</option>
                        <option value={true}>Enabled</option>
                      </select>
                    </div>
                    <div class="form-control">
                      <!-- svelte-ignore a11y_label_has_associated_control -->
                      <label class="label">
                        <span class="label-text text-sm">Momentary state</span>
                      </label>
                      <select
                        bind:value={
                          actionsConfig.gpioActionMomentaryEnabled
                        }
                        class="select select-bordered w-full"
                      >
                        <option value={0}>Disabled</option>
                        <option value={1}>Home App Only</option>
                        <option value={2}>Home Key Only</option>
                        <option value={3}>Home App + Home Key</option>
                      </select>
                    </div>
                    <div class="form-control">
                      <!-- svelte-ignore a11y_label_has_associated_control -->
                      <label class="label">
                        <span class="label-text text-sm"
                          >Momentary timeout (ms)</span
                        >
                      </label>
                      <input
                        type="number"
                        bind:value={
                          actionsConfig.gpioActionMomentaryTimeout
                        }
                        placeholder="5000"
                        class="input input-bordered w-full"
                      />
                    </div>
                  </div>
                </div>
              </div>
              <div class="collapse collapse-arrow bg-base-200">
                <input type="checkbox" name="state-collapse" />
                <div class="collapse-title font-medium">Dummy</div>
                <div class="collapse-content">
                  <div class="grid grid-cols-1 md:grid-cols-2 gap-6">
                    <div class="form-control">
                      <!-- svelte-ignore a11y_label_has_associated_control -->
                      <label class="label">
                        <span class="label-text text-sm">Status</span>
                      </label>
                      <select
                        bind:value={actionsConfig.hkDumbSwitchMode}
                        class="select select-bordered w-full"
                      >
                        <option value={false}>Disabled</option>
                        <option value={true}>Enabled</option>
                      </select>
                    </div>
                    <div class="form-control">
                      <!-- svelte-ignore a11y_label_has_associated_control -->
                      <label class="label">
                        <span class="label-text text-sm">Momentary state</span>
                      </label>
                      <select
                        bind:value={
                          actionsConfig.gpioActionMomentaryEnabled
                        }
                        class="select select-bordered w-full"
                      >
                        <option value={0}>Disabled</option>
                        <option value={1}>Home App Only</option>
                        <option value={2}>Home Key Only</option>
                        <option value={3}>Home App + Home Key</option>
                      </select>
                    </div>
                    <div class="form-control">
                      <!-- svelte-ignore a11y_label_has_associated_control -->
                      <label class="label">
                        <span class="label-text text-sm"
                          >Momentary timeout (ms)</span
                        >
                      </label>
                      <input
                        type="number"
                        bind:value={
                          actionsConfig.gpioActionMomentaryTimeout
                        }
                        placeholder="5000"
                        class="input input-bordered w-full"
                      />
                    </div>
                  </div>
                </div>
              </div>
            </div>
      {/if}

      <!-- Action Buttons -->
      <div class="flex gap-3 mt-6">
        <button type="submit" class="btn btn-primary">Save & Apply changes</button>
        <button type="button" class="btn btn-ghost" onclick={resetForm}>Reset</button>
      </div>
    </form>
  {/if}
</div>
