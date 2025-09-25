<template>
  <div class="flex flex-col py-6">
    <h1 class="md:text-3xl text-3xl font-bold mb-8 self-start">
      System Settings
      <span class="max-sm:before:transform-[translateX(-80%)!important] tooltip tooltip-bottom sm:tooltip-right tooltip-info sm:ml-2" data-tip="Reboot required to apply!">
        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor"
          class="size-6">
          <path stroke-linecap="round" stroke-linejoin="round"
            d="m3.75 13.5 10.5-11.25L12 10.5h8.25L9.75 21.75 12 13.5H3.75Z" />
        </svg>
      </span>
    </h1>
    <div v-if="loading" class="text-center">
      <span class="loading loading-spinner loading-lg"></span>
    </div>
    <div v-else-if="error" class="text-center text-error">
      <p>Error: {{ error }}</p>
    </div>
    <form v-else @submit.prevent="saveMiscConfig" class="self-center">
      <div class="card bg-base-200 shadow-xl">
        <div class="card-body p-4">
          <div class="space-y-2">
            <div class="collapse collapse-arrow bg-base-100">
              <input type="checkbox" name="misc-collapse" />
              <div class="collapse-title font-medium">HomeKit</div>
              <div class="collapse-content flex flex-col gap-4">
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Device Name</span>
                  </label>
                  <input type="text" v-model="miscConfig.deviceName" placeholder="HK"
                    class="input input-bordered w-full" required />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Setup Code</span>
                  </label>
                  <input type="number" v-model.number="miscConfig.setupCode" placeholder="46637726" maxlength="8"
                    class="input input-bordered w-full" required />
                </div>
                <div class="form-control">
                  <label class="label cursor-pointer">
                    <span class="label-text">Always Lock on HomeKey</span>
                    <input type="checkbox" v-model="miscConfig.lockAlwaysLock" :true-value="1" :false-value="0"
                      class="toggle toggle-primary" />
                  </label>
                </div>
                <div class="form-control">
                  <label class="label cursor-pointer">
                    <span class="label-text">Always Unlock on HomeKey</span>
                    <input type="checkbox" v-model="miscConfig.lockAlwaysUnlock" :true-value="1" :false-value="0"
                      class="toggle toggle-primary" />
                  </label>
                </div>
                <div class="form-control">
                  <label class="label cursor-pointer">
                    <span class="label-text">SmartLock battery reporting</span>
                    <input type="checkbox" v-model="miscConfig.proxBatEnabled" :true-value="1" :false-value="0"
                      class="toggle toggle-primary" />
                  </label>
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Battery low status Threshold</span>
                  </label>
                  <input type="number" v-model.number="miscConfig.btrLowStatusThreshold" placeholder="10" min="0"
                    max="100" class="input input-bordered w-full" />
                </div>
              </div>
            </div>

            <div class="collapse collapse-arrow bg-base-100">
              <input type="checkbox" name="misc-collapse" />
              <div class="collapse-title font-medium">HomeKey</div>
              <div class="collapse-content flex flex-col gap-4">
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Alt action Initiator Pin</span>
                  </label>
                  <input type="number" v-model.number="miscConfig.hkAltActionInitPin" placeholder="255" min="0"
                    max="255" class="input input-bordered w-full" />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Alt action Initiator Timeout (ms)</span>
                  </label>
                  <input type="number" v-model.number="miscConfig.hkAltActionInitTimeout" placeholder="5000" min="0"
                    max="10000" class="input input-bordered w-full" />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Feedback LED Pin</span>
                  </label>
                  <input type="number" v-model.number="miscConfig.hkAltActionInitLedPin" placeholder="255" min="0"
                    max="255" class="input input-bordered w-full" />
                </div>
                <div class="divider">HomeKey Card Finish</div>
                <div class="flex flex-col items-center">
                  <img :src="hkFinishImage" alt="HomeKey Finish" class="w-36 h-auto" />
                  <div class="grid grid-cols-2 gap-2">
                    <label v-for="(color, index) in hkFinishColors" :key="index" class="label cursor-pointer">
                      <span class="label-text">{{ color.name }}</span>
                      <input type="radio" name="hk_key_color" :value="color.value"
                        v-model.number="miscConfig.hk_key_color" class="radio radio-primary" />
                    </label>
                  </div>
                </div>
              </div>
            </div>

            <div class="collapse collapse-arrow bg-base-100">
              <input type="checkbox" name="misc-collapse" />
              <div class="collapse-title font-medium">PN532</div>
              <div class="collapse-content flex flex-col gap-4">
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">SS Pin</span>
                  </label>
                  <input type="number" v-model.number="miscConfig.nfcGpioPins[0]" placeholder="5" min="0" max="255"
                    class="input input-bordered w-full" />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">SCK Pin</span>
                  </label>
                  <input type="number" v-model.number="miscConfig.nfcGpioPins[1]" placeholder="18" min="0" max="255"
                    class="input input-bordered w-full" />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">MISO Pin</span>
                  </label>
                  <input type="number" v-model.number="miscConfig.nfcGpioPins[2]" placeholder="19" min="0" max="255"
                    class="input input-bordered w-full" />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">MOSI Pin</span>
                  </label>
                  <input type="number" v-model.number="miscConfig.nfcGpioPins[3]" placeholder="23" min="0" max="255"
                    class="input input-bordered w-full" />
                </div>
              </div>
            </div>

            <div class="collapse collapse-arrow bg-base-100">
              <input type="checkbox" name="misc-collapse" />
              <div class="collapse-title font-medium">HomeSpan</div>
              <div class="collapse-content flex flex-col gap-4">
                <a href="https://github.com/HomeSpan/HomeSpan/blob/master/docs/GettingStarted.md#adding-a-control-button-and-status-led-optional"
                  class="link link-primary block" target="_blank">HomeSpan Documentation</a>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">OTA Password</span>
                  </label>
                  <input type="password" v-model="miscConfig.otaPasswd" placeholder="homespan-ota"
                    class="input input-bordered w-full" required />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Control GPIO Pin</span>
                  </label>
                  <input type="number" v-model.number="miscConfig.controlPin" placeholder="26"
                    class="input input-bordered w-full" />
                </div>
                <div class="form-control">
                  <label class="label">
                    <span class="label-text">Status LED GPIO Pin</span>
                  </label>
                  <input type="number" v-model.number="miscConfig.hsStatusPin" placeholder="2"
                    class="input input-bordered w-full" />
                </div>
              </div>
            </div>

            <div class="collapse collapse-arrow bg-base-100">
              <input type="checkbox" name="misc-collapse" />
              <div class="collapse-title font-medium">Ethernet</div>
              <div class="collapse-content flex flex-col gap-4">
                <div class="form-control">
                  <label class="label cursor-pointer">
                    <span class="label-text">Ethernet Enabled</span>
                    <input type="checkbox" v-model="miscConfig.ethernetEnabled" :true-value="1" :false-value="0"
                      class="toggle toggle-primary" />
                  </label>
                </div>

                <div v-if="miscConfig.ethernetEnabled" class="flex flex-col gap-4">
                  <div class="form-control">
                    <label class="label">
                      <span class="label-text">Active Ethernet Preset</span>
                    </label>
                    <select v-model.number="miscConfig.ethActivePreset" @change="handleEthPresetChange"
                      class="select select-bordered w-full">
                      <option v-for="(preset, key) in ethConfig.boardPresets" :key="key" :value="parseInt(key)">{{
                        preset.name }}</option>
                      <option :value="255">Custom</option>
                    </select>
                  </div>

                  <div class="form-control">
                    <label class="label">
                      <span class="label-text">Ethernet PHY Type</span>
                    </label>
                    <select v-model.number="miscConfig.ethPhyType" :disabled="miscConfig.ethActivePreset !== 255"
                      class="select select-bordered w-full">
                      <option v-for="(chip, key) in ethConfig.supportedChips" :key="key" :value="parseInt(key)">{{
                        chip.name }}</option>
                    </select>
                  </div>

                  <div v-if="currentEthChip && currentEthChip.emac" class="flex flex-col gap-4">
                    <h3 class="text-lg font-bold">RMII Configuration</h3>
                    <div class="flex flex-col gap-4">
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">PHY Address</span>
                        </label>
                        <input type="number" v-model.number="miscConfig.ethRmiiConfig.phy_addr"
                          :disabled="miscConfig.ethActivePreset !== 255" class="input input-bordered w-full" />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Pin MCD</span>
                        </label>
                        <input type="number" v-model.number="miscConfig.ethRmiiConfig.pin_mcd"
                          :disabled="miscConfig.ethActivePreset !== 255" class="input input-bordered w-full" />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Pin MDIO</span>
                        </label>
                        <input type="number" v-model.number="miscConfig.ethRmiiConfig.pin_mdio"
                          :disabled="miscConfig.ethActivePreset !== 255" class="input input-bordered w-full" />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Pin Power</span>
                        </label>
                        <input type="number" v-model.number="miscConfig.ethRmiiConfig.pin_power"
                          :disabled="miscConfig.ethActivePreset !== 255" class="input input-bordered w-full" />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Pin RMII Clock</span>
                        </label>
                        <select v-model.number="miscConfig.ethRmiiConfig.pin_rmii_clock"
                          :disabled="miscConfig.ethActivePreset !== 255" class="select select-bordered w-full">
                          <option value="0">GPIO0_IN</option>
                          <option value="1">GPIO0_OUT</option>
                          <option value="2">GPIO16_OUT</option>
                          <option value="3">GPIO17_OUT</option>
                        </select>
                      </div>
                    </div>
                  </div>

                  <div v-else-if="currentEthChip && !currentEthChip.emac" class="flex flex-col gap-4">
                    <h3 class="text-lg font-bold">SPI Configuration</h3>
                    <div class="flex flex-col gap-4">
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">SPI Freq MHz</span>
                        </label>
                        <input type="number" v-model.number="miscConfig.ethSpiConfig.spi_freq_mhz"
                          :disabled="miscConfig.ethActivePreset !== 255" class="input input-bordered w-full" />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Pin CS</span>
                        </label>
                        <input type="number" v-model.number="miscConfig.ethSpiConfig.pin_cs"
                          :disabled="miscConfig.ethActivePreset !== 255" class="input input-bordered w-full" />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Pin IRQ</span>
                        </label>
                        <input type="number" v-model.number="miscConfig.ethSpiConfig.pin_irq"
                          :disabled="miscConfig.ethActivePreset !== 255" class="input input-bordered w-full" />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Pin RST</span>
                        </label>
                        <input type="number" v-model.number="miscConfig.ethSpiConfig.pin_rst"
                          :disabled="miscConfig.ethActivePreset !== 255" class="input input-bordered w-full" />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Pin SCK</span>
                        </label>
                        <input type="number" v-model.number="miscConfig.ethSpiConfig.pin_sck"
                          :disabled="miscConfig.ethActivePreset !== 255" class="input input-bordered w-full" />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Pin MISO</span>
                        </label>
                        <input type="number" v-model.number="miscConfig.ethSpiConfig.pin_miso"
                          :disabled="miscConfig.ethActivePreset !== 255" class="input input-bordered w-full" />
                      </div>
                      <div class="form-control">
                        <label class="label">
                          <span class="label-text">Pin MOSI</span>
                        </label>
                        <input type="number" v-model.number="miscConfig.ethSpiConfig.pin_mosi"
                          :disabled="miscConfig.ethActivePreset !== 255" class="input input-bordered w-full" />
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>

            <div class="collapse collapse-arrow bg-base-100">
              <input type="checkbox" name="misc-collapse" />
              <div class="collapse-title font-medium">WebUI</div>
              <div class="collapse-content">
                <div class="form-control my-4">
                  <label class="label cursor-pointer">
                    <span class="label-text">Authentication Enabled</span>
                    <input type="checkbox" v-model="miscConfig.webAuthEnabled" :true-value="1" :false-value="0"
                      class="toggle toggle-primary" />
                  </label>
                </div>
                <div class="form-control mb-4">
                  <label class="label">
                    <span class="label-text">Username</span>
                  </label>
                  <input type="text" v-model="miscConfig.webUsername" placeholder="admin"
                    class="input input-bordered w-full" required />
                </div>
                <div class="form-control mb-2">
                  <label class="label">
                    <span class="label-text">Password</span>
                  </label>
                  <input type="password" v-model="miscConfig.webPassword" placeholder="password"
                    class="input input-bordered w-full" required />
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>

      <div class="card-actions justify-end mt-6">
        <button type="submit" class="btn btn-primary">Save & Apply</button>
        <button type="button" class="btn btn-ghost" @click="resetForm">Reset Form</button>
      </div>
    </form>
  </div>
</template>
<script>
import { ref, onMounted, computed } from 'vue';
import { fetchConfig, saveConfig, fetchEthConfig } from '../services/api';

export default {
  name: 'AppMisc',
  setup() {
    const miscConfig = ref({
      deviceName: '',
      setupCode: 0,
      lockAlwaysLock: 0,
      lockAlwaysUnlock: 0,
      proxBatEnabled: 0,
      btrLowStatusThreshold: 10,
      hkAltActionInitPin: 255,
      hkAltActionInitTimeout: 5000,
      hkAltActionInitLedPin: 255,
      hk_key_color: 0,
      nfcGpioPins: [5, 18, 19, 23],
      otaPasswd: '',
      controlPin: 26,
      hsStatusPin: 2,
      ethernetEnabled: 0,
      ethActivePreset: 0,
      ethPhyType: 0,
      ethSpiConfig: {
        spi_freq_mhz: 0,
        pin_cs: 0,
        pin_irq: 0,
        pin_rst: 0,
        pin_sck: 0,
        pin_miso: 0,
        pin_mosi: 0,
      },
      ethRmiiConfig: {
        phy_addr: 0,
        pin_mcd: 0,
        pin_mdio: 0,
        pin_power: 0,
        pin_rmii_clock: 0,
      },
      webAuthEnabled: 0,
      webUsername: '',
      webPassword: '',
    });
    const ethConfig = ref({
      boardPresets: {},
      supportedChips: {},
    });
    const loading = ref(true);
    const error = ref(null);
    let originalMiscConfig = {};

    const hkFinishColors = [
      { name: 'Tan', value: 0, image: '/static/hk-finish-0.webp' },
      { name: 'Gold', value: 1, image: '/static/hk-finish-1.webp' },
      { name: 'Silver', value: 2, image: '/static/hk-finish-2.webp' },
      { name: 'Black', value: 3, image: '/static/hk-finish-3.webp' },
    ];

    const hkFinishImage = computed(() => {
      const color = hkFinishColors.find(c => c.value === miscConfig.value.hk_key_color);
      return color ? color.image : '/static/hk-finish-0.webp';
    });

    const currentEthChip = computed(() => {
      return ethConfig.value.supportedChips[miscConfig.value.ethPhyType];
    });

    const _fetchEthConfig = async () => {
      try {
        ethConfig.value = await fetchEthConfig();
      } catch (e) {
        console.error("Error fetching eth config:", e);
        error.value = e.message;
      }
    };

    const _fetchMiscConfig = async () => {
      try {
        const data = await fetchConfig('misc');
        miscConfig.value = {
          ...miscConfig.value, // Keep defaults for properties not in fetched data
          ...data,
          nfcGpioPins: data.nfcGpioPins || [5, 18, 19, 23],
          ethSpiConfig: data.ethSpiConfig || miscConfig.value.ethSpiConfig,
          ethRmiiConfig: data.ethRmiiConfig || miscConfig.value.ethRmiiConfig,
        };
        originalMiscConfig = JSON.parse(JSON.stringify(miscConfig.value));
      } catch (e) {
        console.error("Error fetching misc config:", e);
        error.value = e.message;
      } finally {
        loading.value = false;
      }
    };

    const saveMiscConfig = async () => {
      try {
        const dataToSend = {
          ...miscConfig.value,
        };

        const result = await saveConfig('misc', dataToSend);
        alert(result);
        originalMiscConfig = JSON.parse(JSON.stringify(miscConfig.value));
      } catch (e) {
        error.value = e.message;
        alert(`Error saving config: ${e.message}`);
      }
    };

    const handleEthPresetChange = () => {
      if (miscConfig.value.ethActivePreset !== 255) {
        const preset = ethConfig.value.boardPresets[miscConfig.value.ethActivePreset];
        if (preset) {
          miscConfig.value.ethPhyType = preset.ethChip.phy_type;
          if (preset.spi_conf) {
            miscConfig.value.ethSpiConfig = { ...preset.spi_conf };
          } else {
            miscConfig.value.ethSpiConfig = {
              spi_freq_mhz: 0,
              pin_cs: 0,
              pin_irq: 0,
              pin_rst: 0,
              pin_sck: 0,
              pin_miso: 0,
              pin_mosi: 0,
            };
          }
          if (preset.rmii_conf) {
            miscConfig.value.ethRmiiConfig = { ...preset.rmii_conf };
          } else {
            miscConfig.value.ethRmiiConfig = {
              phy_addr: 0,
              pin_mcd: 0,
              pin_mdio: 0,
              pin_power: 0,
              pin_rmii_clock: 0,
            };
          }
        }
      }
    };

    const resetForm = () => {
      miscConfig.value = JSON.parse(JSON.stringify(originalMiscConfig));
    };

    onMounted(async () => {
      await _fetchEthConfig();
      await _fetchMiscConfig();
      // After fetching both, apply initial preset if not custom
      if (miscConfig.value.ethActivePreset !== 255) {
        handleEthPresetChange();
      }
    });

    return {
      miscConfig,
      ethConfig,
      loading,
      error,
      hkFinishColors,
      hkFinishImage,
      currentEthChip,
      saveMiscConfig,
      resetForm,
      handleEthPresetChange,
    };
  },
};
</script>