<template>
  <div class="w-full max-w-4xl py-6">
    <h1 class="md:text-3xl text-2xl font-bold mb-8">
      MQTT Settings
      <div class="tooltip tooltip-bottom tooltip-info" data-tip="Reboot required to apply!">
        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor"
          class="size-6">
          <path stroke-linecap="round" stroke-linejoin="round"
            d="m3.75 13.5 10.5-11.25L12 10.5h8.25L9.75 21.75 12 13.5H3.75Z" />
        </svg>
      </div>
    </h1>
    <div v-if="loading" class="text-center">
      <span class="loading loading-spinner loading-lg"></span>
    </div>
    <div v-else-if="error" class="text-center text-error">
      <p>Error: {{ error }}</p>
    </div>
    <form v-else @submit.prevent="saveMqttConfig">
      <div class="card bg-base-200 shadow-xl">
        <div class="card-body p-4">
          <div class="collapse collapse-arrow bg-base-100">
            <input type="checkbox" name="mqtt-accordion" checked />
            <div class="collapse-title font-medium flex items-center">
              <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5"
                stroke="currentColor" class="w-5 mr-2">
                <path stroke-linecap="round" stroke-linejoin="round"
                  d="M21.75 17.25v-.228a4.5 4.5 0 0 0-.12-1.03l-2.268-9.64a3.375 3.375 0 0 0-3.285-2.602H7.923a3.375 3.375 0 0 0-3.285 2.602l-2.268 9.64a4.5 4.5 0 0 0-.12 1.03v.228m19.5 0a3 3 0 0 1-3 3H5.25a3 3 0 0 1-3-3m19.5 0a3 3 0 0 0-3-3H5.25a3 3 0 0 0-3 3m16.5 0h.008v.008h-.008v-.008Zm-3 0h.008v.008h-.008v-.008Z" />
              </svg>
              Broker
            </div>
            <div class="collapse-content">
              <div class="text-xs uppercase font-semibold opacity-60 mb-4">TCP - Without TLS</div>
              <div class="grid grid-cols-1 md:grid-cols-2 gap-6">
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Address</span>
                  </label>
                  <input type="text" v-model="mqttConfig.mqttBroker" placeholder="0.0.0.0"
                    class="input input-bordered w-full" required />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Port</span>
                  </label>
                  <input type="number" v-model.number="mqttConfig.mqttPort" placeholder="1883"
                    class="input input-bordered w-full" required inputmode="numeric" min="0" max="65535" />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Client ID</span>
                  </label>
                  <input type="text" v-model="mqttConfig.mqttClientId" placeholder="homekey_mqtt"
                    class="input input-bordered w-full" required />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">LWT Topic</span>
                  </label>
                  <input type="text" v-model="mqttConfig.lwtTopic" placeholder="homekey_mqtt/status"
                    class="input input-bordered w-full" required />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Username</span>
                  </label>
                  <input type="text" v-model="mqttConfig.mqttUsername" placeholder="username"
                    class="input input-bordered w-full" />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Password</span>
                  </label>
                  <input type="password" v-model="mqttConfig.mqttPassword" placeholder="password"
                    class="input input-bordered w-full" />
                </div>
                <div class="form-control md:col-span-2 mb-4">
                  <label class="label cursor-pointer">
                    <span class="label-text">HASS MQTT Discovery</span>
                    <input type="checkbox" v-model="mqttConfig.hassMqttDiscoveryEnabled" :true-value="1"
                      :false-value="0" class="toggle toggle-primary" />
                  </label>
                </div>
              </div>
            </div>
          </div>

          <div class="collapse collapse-arrow bg-base-100">
            <input type="checkbox" name="mqtt-accordion" />
            <div class="collapse-title font-medium flex items-center">
              <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5"
                stroke="currentColor" class="w-5 mr-2">
                <path stroke-linecap="round" stroke-linejoin="round"
                  d="M21.75 6.75a4.5 4.5 0 0 1-4.884 4.484c-1.076-.091-2.264.071-2.95.904l-7.152 8.684a2.548 2.548 0 1 1-3.586-3.586l8.684-7.152c.833-.686.995-1.874.904-2.95a4.5 4.5 0 0 1 6.336-4.486l-3.276 3.276a3.004 3.004 0 0 0 2.25 2.25l3.276-3.276c.256.565.398 1.192.398 1.852Z" />
                <path stroke-linecap="round" stroke-linejoin="round" d="M4.867 19.125h.008v.008h-.008v-.008Z" />
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
                        <input type="text" v-model="mqttConfig.hkTopic" placeholder="topic/auth"
                          class="input input-bordered w-full" required />
                      </div>
                      <div class="form-control">
                        <label class="label cursor-pointer">
                          <span class="label-text">Ignore NFC Tags</span>
                          <input type="checkbox" v-model="mqttConfig.nfcTagNoPublish" :true-value="1" :false-value="0"
                            class="toggle toggle-primary" />
                        </label>
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Secondary action Topic</span>
                        </label>
                        <input type="text" v-model="mqttConfig.hkAltActionTopic" placeholder="topic/alt_action"
                          class="input input-bordered w-full" required />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Lock State Topic</span>
                        </label>
                        <input type="text" v-model="mqttConfig.lockStateTopic" placeholder="topic/state"
                          class="input input-bordered w-full" required />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Lock State Cmd Topic</span>
                        </label>
                        <input type="text" v-model="mqttConfig.lockStateCmd" placeholder="topic/set_state"
                          class="input input-bordered w-full" required />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Lock Current State Cmd Topic</span>
                        </label>
                        <input type="text" v-model="mqttConfig.lockCStateCmd" placeholder="topic/set_current_state"
                          class="input input-bordered w-full" required />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Lock Target State Cmd Topic</span>
                        </label>
                        <input type="text" v-model="mqttConfig.lockTStateCmd" placeholder="topic/set_target_state"
                          class="input input-bordered w-full" required />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">SmartLock battery level Cmd Topic</span>
                        </label>
                        <input type="text" v-model="mqttConfig.btrLvlCmdTopic" placeholder="topic/set_battery_level"
                          class="input input-bordered w-full" required />
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
                        <input type="checkbox" v-model="mqttConfig.lockEnableCustomState" :true-value="1"
                          :false-value="0" class="toggle toggle-primary" />
                      </label>
                    </div>
                    <div v-if="mqttConfig.lockEnableCustomState">
                      <div class="form-control mb-4">
                        <label class="label">
                          <span class="label-text">Custom State Topic</span>
                        </label>
                        <input type="text" v-model="mqttConfig.lockCustomStateTopic"
                          placeholder="topic/set_target_state" class="input input-bordered w-full" />
                      </div>
                      <div class="form-control mb-4">
                        <label class="label">
                          <span class="label-text">Custom State Cmd Topic</span>
                        </label>
                        <input type="text" v-model="mqttConfig.lockCustomStateCmd" placeholder="topic/set_target_state"
                          class="input input-bordered w-full" />
                      </div>
                      <div class="divider">Custom Lock Actions</div>
                      <div class="grid grid-cols-2 gap-6">
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Unlock</span>
                          </label>
                          <input type="number" v-model.number="mqttConfig.customLockActions.UNLOCK" placeholder="255"
                            class="input input-bordered w-full" min="0" max="255" />
                        </div>
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Lock</span>
                          </label>
                          <input type="number" v-model.number="mqttConfig.customLockActions.LOCK" placeholder="255"
                            class="input input-bordered w-full" min="0" max="255" />
                        </div>
                      </div>
                      <div class="divider">Custom Lock States</div>
                      <div class="grid grid-cols-2 md:grid-cols-3 gap-6">
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Unlocking</span>
                          </label>
                          <input type="number" v-model.number="mqttConfig.customLockStates.C_UNLOCKING"
                            placeholder="255" class="input input-bordered w-full" min="0" max="255" />
                        </div>
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Locking</span>
                          </label>
                          <input type="number" v-model.number="mqttConfig.customLockStates.C_LOCKING" placeholder="255"
                            class="input input-bordered w-full" min="0" max="255" />
                        </div>
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Unlocked</span>
                          </label>
                          <input type="number" v-model.number="mqttConfig.customLockStates.C_UNLOCKED" placeholder="255"
                            class="input input-bordered w-full" min="0" max="255" />
                        </div>
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Locked</span>
                          </label>
                          <input type="number" v-model.number="mqttConfig.customLockStates.C_LOCKED" placeholder="255"
                            class="input input-bordered w-full" min="0" max="255" />
                        </div>
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Jammed</span>
                          </label>
                          <input type="number" v-model.number="mqttConfig.customLockStates.C_JAMMED" placeholder="255"
                            class="input input-bordered w-full" min="0" max="255" />
                        </div>
                        <div class="form-control">
                          <label class="label">
                            <span class="label-text">Unknown</span>
                          </label>
                          <input type="number" v-model.number="mqttConfig.customLockStates.C_UNKNOWN" placeholder="255"
                            class="input input-bordered w-full" min="0" max="255" />
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
      <div class="card-actions justify-end px-2 pt-4">
        <button type="submit" class="btn btn-primary">Save & Apply</button>
        <button type="button" class="btn btn-ghost" @click="resetForm">Reset Form</button>
      </div>
    </form>
  </div>
</template>

<script>
import { ref, onMounted } from 'vue';
import { fetchConfig, saveConfig } from '../services/api';

export default {
  name: 'AppMqtt',
  setup() {
    const mqttConfig = ref({
      mqttBroker: '',
      mqttPort: 1883,
      mqttClientId: '',
      lwtTopic: '',
      mqttUsername: '',
      mqttPassword: '',
      hassMqttDiscoveryEnabled: 0,
      hkTopic: '',
      nfcTagNoPublish: 0,
      hkAltActionTopic: '',
      lockStateTopic: '',
      lockStateCmd: '',
      lockCStateCmd: '',
      lockTStateCmd: '',
      btrLvlCmdTopic: '',
      lockEnableCustomState: 0,
      lockCustomStateTopic: '',
      lockCustomStateCmd: '',
      customLockActions: {
        UNLOCK: 255,
        LOCK: 255,
      },
      customLockStates: {
        C_UNLOCKING: 255,
        C_LOCKING: 255,
        C_UNLOCKED: 255,
        C_LOCKED: 255,
        C_JAMMED: 255,
        C_UNKNOWN: 255,
      },
    });
    const loading = ref(true);
    const error = ref(null);
    let originalMqttConfig = {}; // To store the original fetched config for reset

    const fetchMqttConfig = async () => {
      try {
        const data = await fetchConfig('mqtt');
        // Flatten the customLockActions and customLockStates for v-model compatibility
        mqttConfig.value = {
          ...data,
          customLockActions: data.customLockActions || { UNLOCK: 255, LOCK: 255 },
          customLockStates: data.customLockStates || { C_UNLOCKING: 255, C_LOCKING: 255, C_UNLOCKED: 255, C_LOCKED: 255, C_JAMMED: 255, C_UNKNOWN: 255 },
        };
        originalMqttConfig = JSON.parse(JSON.stringify(mqttConfig.value)); // Deep copy for reset
      } catch (e) {
        error.value = e.message;
      } finally {
        loading.value = false;
      }
    };

    const saveMqttConfig = async () => {
      try {
        // Reconstruct the data to match the ESP32's expected nested structure
        const dataToSend = {
          ...mqttConfig.value,
          customLockActions: {
            UNLOCK: mqttConfig.value.customLockActions.UNLOCK,
            LOCK: mqttConfig.value.customLockActions.LOCK,
          },
          customLockStates: {
            C_UNLOCKING: mqttConfig.value.customLockStates.C_UNLOCKING,
            C_LOCKING: mqttConfig.value.customLockStates.C_LOCKING,
            C_UNLOCKED: mqttConfig.value.customLockStates.C_UNLOCKED,
            C_LOCKED: mqttConfig.value.customLockStates.C_LOCKED,
            C_JAMMED: mqttConfig.value.customLockStates.C_JAMMED,
            C_UNKNOWN: mqttConfig.value.customLockStates.C_UNKNOWN,
          },
        };

        const result = await saveConfig('mqtt', dataToSend);
        alert(result); // Replace with a more user-friendly notification
        originalMqttConfig = JSON.parse(JSON.stringify(mqttConfig.value)); // Update original after successful save
      } catch (e) {
        error.value = e.message;
        alert(`Error saving config: ${e.message}`); // Replace with a more user-friendly notification
      }
    };

    const resetForm = () => {
      mqttConfig.value = JSON.parse(JSON.stringify(originalMqttConfig));
    };

    onMounted(fetchMqttConfig);

    return {
      mqttConfig,
      loading,
      error,
      saveMqttConfig,
      resetForm,
    };
  },
};
</script>
