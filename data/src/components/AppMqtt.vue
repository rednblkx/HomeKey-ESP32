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
              <div class="text-xs uppercase font-semibold opacity-60 mb-4" v-if="!mqttConfig.useSSL">TCP - Without TLS</div>
              <div class="text-xs uppercase font-semibold opacity-60 mb-4" v-else>TCP - With TLS/SSL</div>
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
                <div class="form-control md:col-span-2 mb-4">
                  <label class="label cursor-pointer">
                    <span class="label-text">Enable SSL/TLS</span>
                    <input type="checkbox" v-model="mqttConfig.useSSL" :true-value="1" :false-value="0" class="toggle toggle-primary" @change="onSSLToggleChange" />
                  </label>
                </div>
              </div>
            </div>
          </div>

          <!-- SSL/TLS Configuration Section -->
          <div v-if="mqttConfig.useSSL" class="collapse collapse-arrow bg-base-100">
            <input type="checkbox" name="mqtt-accordion" />
            <div class="collapse-title font-medium flex items-center">
              <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-5 mr-2">
                <path stroke-linecap="round" stroke-linejoin="round" d="M16.5 10.5V6.75a4.5 4.5 0 10-9 0v3.75m-.75 11.25h10.5a2.25 2.25 0 002.25-2.25v-6.75a2.25 2.25 0 00-2.25-2.25H6.75a2.25 2.25 0 00-2.25 2.25v6.75a2.25 2.25 0 002.25 2.25z" />
              </svg>
              SSL/TLS Settings
            </div>
            <div class="collapse-content">
              <div class="space-y-6">
                <div class="form-control">
                  <label class="label cursor-pointer">
                    <span class="label-text">Allow Insecure Connections (skip certificate validation)</span>
                    <input type="checkbox" v-model="mqttConfig.allowInsecure" :true-value="1" :false-value="0" class="toggle toggle-warning" />
                  </label>
                </div>

                <!-- Certificate Status Display -->
                <div class="bg-base-200 rounded-lg p-4">
                  <h4 class="font-medium mb-3">Certificate Status</h4>
                  
                  <!-- Reconnection Status Alert -->
                  <div v-if="reconnectionStatus.isReconnecting" class="alert alert-info mb-4">
                    <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-6 h-6">
                      <path stroke-linecap="round" stroke-linejoin="round" d="M16.023 9.348h4.992v-.001M2.985 19.644v-4.992m0 0h4.992m-4.993 0l3.181 3.183a8.25 8.25 0 0011.664 0l3.181-3.183m-3.181-4.991v4.99" />
                    </svg>
                    <span>{{ reconnectionStatus.message }}</span>
                  </div>
                  
                  <div class="grid grid-cols-1 md:grid-cols-3 gap-4">
                    <!-- CA Certificate -->
                    <div class="flex flex-col p-3 bg-base-100 rounded-lg">
                      <div class="flex items-center justify-between mb-2">
                        <div class="flex items-center">
                          <svg v-if="certificateStatus.ca" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-5 h-5 text-success mr-2">
                            <path stroke-linecap="round" stroke-linejoin="round" d="M9 12.75L11.25 15 15 9.75M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
                          </svg>
                          <svg v-else xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-5 h-5 text-error mr-2">
                            <path stroke-linecap="round" stroke-linejoin="round" d="M9.75 9.75l4.5 4.5m0-4.5l-4.5 4.5M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
                          </svg>
                          <span class="text-sm font-medium">CA Certificate</span>
                        </div>
                        <button v-if="certificateStatus.ca" @click="deleteCertificateHandler('ca')" class="btn btn-ghost btn-xs btn-error">Delete</button>
                      </div>
                      <div v-if="detailedCertificateStatus.ca" class="text-xs space-y-1">
                        <div v-if="detailedCertificateStatus.ca.issuer" class="flex justify-between">
                          <span class="opacity-60">Issuer:</span>
                          <span class="truncate">{{ detailedCertificateStatus.ca.issuer }}</span>
                        </div>
                        <div v-if="detailedCertificateStatus.ca.expiration" class="flex justify-between">
                          <span class="opacity-60">Expires:</span>
                          <span>{{ new Date(detailedCertificateStatus.ca.expiration).toLocaleDateString() }}</span>
                        </div>
                        <div v-if="detailedCertificateStatus.ca.daysUntilExpiry !== undefined" class="flex justify-between">
                          <span class="opacity-60">Days left:</span>
                          <span :class="detailedCertificateStatus.ca.daysUntilExpiry < 30 ? 'text-warning' : 'text-success'">
                            {{ detailedCertificateStatus.ca.daysUntilExpiry }}
                          </span>
                        </div>
                      </div>
                    </div>
                    
                    <!-- Client Certificate -->
                    <div class="flex flex-col p-3 bg-base-100 rounded-lg">
                      <div class="flex items-center justify-between mb-2">
                        <div class="flex items-center">
                          <svg v-if="certificateStatus.client" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-5 h-5 text-success mr-2">
                            <path stroke-linecap="round" stroke-linejoin="round" d="M9 12.75L11.25 15 15 9.75M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
                          </svg>
                          <svg v-else xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-5 h-5 text-error mr-2">
                            <path stroke-linecap="round" stroke-linejoin="round" d="M9.75 9.75l4.5 4.5m0-4.5l-4.5 4.5M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
                          </svg>
                          <span class="text-sm font-medium">Client Certificate</span>
                        </div>
                        <button v-if="certificateStatus.client" @click="deleteCertificateHandler('client')" class="btn btn-ghost btn-xs btn-error">Delete</button>
                      </div>
                      <div v-if="detailedCertificateStatus.client" class="text-xs space-y-1">
                        <div v-if="detailedCertificateStatus.client.issuer" class="flex justify-between">
                          <span class="opacity-60">Issuer:</span>
                          <span class="truncate">{{ detailedCertificateStatus.client.issuer }}</span>
                        </div>
                        <div v-if="detailedCertificateStatus.client.expiration" class="flex justify-between">
                          <span class="opacity-60">Expires:</span>
                          <span>{{ new Date(detailedCertificateStatus.client.expiration).toLocaleDateString() }}</span>
                        </div>
                        <div v-if="detailedCertificateStatus.client.daysUntilExpiry !== undefined" class="flex justify-between">
                          <span class="opacity-60">Days left:</span>
                          <span :class="detailedCertificateStatus.client.daysUntilExpiry < 30 ? 'text-warning' : 'text-success'">
                            {{ detailedCertificateStatus.client.daysUntilExpiry }}
                          </span>
                        </div>
                        <div v-if="detailedCertificateStatus.client.subject" class="flex justify-between">
                          <span class="opacity-60">Subject:</span>
                          <span class="truncate">{{ detailedCertificateStatus.client.subject }}</span>
                        </div>
                      </div>
                    </div>
                    
                    <!-- Private Key -->
                    <div class="flex flex-col p-3 bg-base-100 rounded-lg">
                      <div class="flex items-center justify-between mb-2">
                        <div class="flex items-center">
                          <svg v-if="certificateStatus.privateKey" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-5 h-5 text-success mr-2">
                            <path stroke-linecap="round" stroke-linejoin="round" d="M9 12.75L11.25 15 15 9.75M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
                          </svg>
                          <svg v-else xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-5 h-5 text-error mr-2">
                            <path stroke-linecap="round" stroke-linejoin="round" d="M9.75 9.75l4.5 4.5m0-4.5l-4.5 4.5M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
                          </svg>
                          <span class="text-sm font-medium">Private Key</span>
                        </div>
                        <button v-if="certificateStatus.privateKey" @click="deleteCertificateHandler('privateKey')" class="btn btn-ghost btn-xs btn-error">Delete</button>
                      </div>
                      <div v-if="detailedCertificateStatus.privateKey" class="text-xs space-y-1">
                        <div v-if="detailedCertificateStatus.privateKey.keyType" class="flex justify-between">
                          <span class="opacity-60">Type:</span>
                          <span>{{ detailedCertificateStatus.privateKey.keyType }}</span>
                        </div>
                        <div v-if="detailedCertificateStatus.privateKey.keySize" class="flex justify-between">
                          <span class="opacity-60">Key Size:</span>
                          <span>{{ detailedCertificateStatus.privateKey.keySize }} bits</span>
                        </div>
                        <div v-if="detailedCertificateStatus.privateKey.matchesCertificate !== undefined" class="flex justify-between">
                          <span class="opacity-60">Matches Cert:</span>
                          <span :class="detailedCertificateStatus.privateKey.matchesCertificate ? 'text-success' : 'text-error'">
                            {{ detailedCertificateStatus.privateKey.matchesCertificate ? 'Yes' : 'No' }}
                          </span>
                        </div>
                      </div>
                    </div>
                  </div>
                  
                  <!-- Certificate Validation Status -->
                  <div v-if="detailedCertificateStatus.ca || detailedCertificateStatus.client || detailedCertificateStatus.privateKey" class="mt-4 p-3 bg-base-100 rounded-lg">
                    <h5 class="font-medium mb-2">Validation Status</h5>
                    <div class="space-y-1 text-xs">
                      <div v-if="detailedCertificateStatus.ca" class="flex items-center">
                        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-4 h-4 mr-2" :class="detailedCertificateStatus.ca.valid ? 'text-success' : 'text-error'">
                          <path stroke-linecap="round" stroke-linejoin="round" d="M9 12.75L11.25 15 15 9.75M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
                        </svg>
                        <span>CA Certificate: {{ detailedCertificateStatus.ca.valid ? 'Valid' : 'Invalid' }}</span>
                      </div>
                      <div v-if="detailedCertificateStatus.client" class="flex items-center">
                        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-4 h-4 mr-2" :class="detailedCertificateStatus.client.valid ? 'text-success' : 'text-error'">
                          <path stroke-linecap="round" stroke-linejoin="round" d="M9 12.75L11.25 15 15 9.75M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
                        </svg>
                        <span>Client Certificate: {{ detailedCertificateStatus.client.valid ? 'Valid' : 'Invalid' }}</span>
                      </div>
                      <div v-if="detailedCertificateStatus.privateKey" class="flex items-center">
                        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor" class="w-4 h-4 mr-2" :class="detailedCertificateStatus.privateKey.valid ? 'text-success' : 'text-error'">
                          <path stroke-linecap="round" stroke-linejoin="round" d="M9 12.75L11.25 15 15 9.75M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
                        </svg>
                        <span>Private Key: {{ detailedCertificateStatus.privateKey.valid ? 'Valid' : 'Invalid' }}</span>
                      </div>
                    </div>
                  </div>
                </div>

                <!-- Certificate Upload Sections -->
                <div class="grid grid-cols-1 md:grid-cols-3 gap-4">
                  <div class="form-control">
                    <label class="label">
                      <span class="label-text">CA Certificate</span>
                    </label>
                    <input
                      type="file"
                      @change="handleCertificateUpload('ca', $event)"
                      accept=".pem,.crt,.cer,.der"
                      class="file-input file-input-bordered w-full"
                      :disabled="uploadProgress.ca > 0 && uploadProgress.ca < 100"
                    />
                    <div v-if="uploadProgress.ca > 0 && uploadProgress.ca < 100" class="progress progress-info mt-2">
                      <div class="progress-bar" :style="{ width: uploadProgress.ca + '%' }"></div>
                    </div>
                    <div v-if="uploadErrors.ca" class="text-error text-sm mt-1">{{ uploadErrors.ca }}</div>
                  </div>
                  <div class="form-control">
                    <label class="label">
                      <span class="label-text">Client Certificate</span>
                    </label>
                    <input
                      type="file"
                      @change="handleCertificateUpload('client', $event)"
                      accept=".pem,.crt,.cer,.der"
                      class="file-input file-input-bordered w-full"
                      :disabled="uploadProgress.client > 0 && uploadProgress.client < 100"
                    />
                    <div v-if="uploadProgress.client > 0 && uploadProgress.client < 100" class="progress progress-info mt-2">
                      <div class="progress-bar" :style="{ width: uploadProgress.client + '%' }"></div>
                    </div>
                    <div v-if="uploadErrors.client" class="text-error text-sm mt-1">{{ uploadErrors.client }}</div>
                  </div>
                  <div class="form-control">
                    <label class="label">
                      <span class="label-text">Private Key</span>
                    </label>
                    <input
                      type="file"
                      @change="handleCertificateUpload('privateKey', $event)"
                      accept=".pem,.key,.der"
                      class="file-input file-input-bordered w-full"
                      :disabled="uploadProgress.privateKey > 0 && uploadProgress.privateKey < 100"
                    />
                    <div v-if="uploadProgress.privateKey > 0 && uploadProgress.privateKey < 100" class="progress progress-info mt-2">
                      <div class="progress-bar" :style="{ width: uploadProgress.privateKey + '%' }"></div>
                    </div>
                    <div v-if="uploadErrors.privateKey" class="text-error text-sm mt-1">{{ uploadErrors.privateKey }}</div>
                  </div>
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
import { fetchConfig, saveConfig, uploadCertificate, getCertificateStatus, getDetailedCertificateStatus, deleteCertificate } from '../services/api';

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
      useSSL: 0,
      allowInsecure: 0,
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
    const certificateStatus = ref({
      ca: false,
      client: false,
      privateKey: false,
    });
    const detailedCertificateStatus = ref({
      ca: null,
      client: null,
      privateKey: null,
    });
    const reconnectionStatus = ref({
      isReconnecting: false,
      message: '',
      lastReconnection: null,
    });
    const uploadProgress = ref({
      ca: 0,
      client: 0,
      privateKey: 0,
    });
    const uploadErrors = ref({
      ca: '',
      client: '',
      privateKey: '',
    });
    let originalMqttConfig = {}; // To store the original fetched config for reset

    const fetchMqttConfig = async () => {
      try {
        const data = await fetchConfig('mqtt');
        // Flatten the customLockActions and customLockStates for v-model compatibility
        mqttConfig.value = {
          ...data,
          useSSL: data.useSSL || 0,
          allowInsecure: data.allowInsecure || 0,
          customLockActions: data.customLockActions || { UNLOCK: 255, LOCK: 255 },
          customLockStates: data.customLockStates || { C_UNLOCKING: 255, C_LOCKING: 255, C_UNLOCKED: 255, C_LOCKED: 255, C_JAMMED: 255, C_UNKNOWN: 255 },
        };
        originalMqttConfig = JSON.parse(JSON.stringify(mqttConfig.value)); // Deep copy for reset
        await fetchCertificateStatus();
      } catch (e) {
        error.value = e.message;
      } finally {
        loading.value = false;
      }
    };

    const fetchCertificateStatus = async () => {
      try {
        const status = await getCertificateStatus();
        certificateStatus.value = status;
        
        // Also fetch detailed status
        try {
          const detailedStatus = await getDetailedCertificateStatus();
          detailedCertificateStatus.value = detailedStatus;
        } catch (e) {
          console.warn('Detailed certificate status not available:', e);
          // Fallback to basic status
          detailedCertificateStatus.value = {
            ca: status.ca ? { valid: true, type: 'ca' } : null,
            client: status.client ? { valid: true, type: 'client' } : null,
            privateKey: status.privateKey ? { valid: true, type: 'privateKey' } : null,
          };
        }
      } catch (e) {
        console.error('Error fetching certificate status:', e);
      }
    };

    const handleCertificateUpload = async (type, event) => {
      const file = event.target.files[0];
      if (!file) return;

      // Reset error for this type
      uploadErrors.value[type] = '';
      
      // Validate file type
      const validExtensions = ['.pem', '.crt', '.cer', '.der', '.key'];
      const fileExtension = '.' + file.name.split('.').pop().toLowerCase();
      
      if (type === 'privateKey' && !['.pem', '.key', '.der'].includes(fileExtension)) {
        uploadErrors.value[type] = 'Invalid file type for private key';
        return;
      } else if (type !== 'privateKey' && !validExtensions.slice(0, -1).includes(fileExtension)) {
        uploadErrors.value[type] = 'Invalid certificate file type';
        return;
      }

      // Simulate upload progress
      uploadProgress.value[type] = 0;
      
      const reader = new FileReader();
      reader.onloadstart = () => {
        uploadProgress.value[type] = 10;
      };
      reader.onprogress = (e) => {
        if (e.lengthComputable) {
          uploadProgress.value[type] = Math.min(90, (e.loaded / e.total) * 100);
        }
      };
      reader.onload = async (e) => {
        try {
          uploadProgress.value[type] = 95;
          const content = e.target.result;
          const result = await uploadCertificate(type, content);
          uploadProgress.value[type] = 100;
          
          // Handle reconnection status from response
          if (result.reconnectionStatus) {
            reconnectionStatus.value = {
              isReconnecting: result.reconnectionStatus.isReconnecting,
              message: result.reconnectionStatus.message,
              lastReconnection: new Date().toISOString(),
            };
            
            // Show reconnection notification
            if (result.reconnectionStatus.isReconnecting) {
              alert(`Certificate uploaded successfully. ${result.reconnectionStatus.message}`);
            } else if (result.reconnectionStatus.message) {
              alert(`Certificate uploaded successfully. ${result.reconnectionStatus.message}`);
            }
          }
          
          // Refresh certificate status
          await fetchCertificateStatus();
          
          // Clear file input
          event.target.value = '';
          
          // Reset progress after a delay
          setTimeout(() => {
            uploadProgress.value[type] = 0;
          }, 1000);
        } catch (error) {
          uploadErrors.value[type] = `Upload failed: ${error.message}`;
          uploadProgress.value[type] = 0;
        }
      };
      reader.onerror = () => {
        uploadErrors.value[type] = 'Failed to read file';
        uploadProgress.value[type] = 0;
      };
      
      reader.readAsText(file);
    };

    const deleteCertificateHandler = async (type) => {
      if (!confirm(`Are you sure you want to delete the ${type} certificate?`)) {
        return;
      }
      
      try {
        await deleteCertificate(type);
        await fetchCertificateStatus();
      } catch (e) {
        alert(`Error deleting certificate: ${e.message}`);
      }
    };

    const onSSLToggleChange = () => {
      // If SSL is disabled, reset allowInsecure to 0
      if (mqttConfig.value.useSSL === 0) {
        mqttConfig.value.allowInsecure = 0;
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
      certificateStatus,
      detailedCertificateStatus,
      reconnectionStatus,
      uploadProgress,
      uploadErrors,
      saveMqttConfig,
      resetForm,
      handleCertificateUpload,
      deleteCertificateHandler,
      onSSLToggleChange,
    };
  },
};
</script>
