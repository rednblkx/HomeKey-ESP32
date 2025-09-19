<template>
  <div class="w-full max-w-4xl space-y-6">
    <!-- OTA Update Card -->
    <div class="card bg-base-200 shadow-xl">
      <h2 class="card-title p-4 pb-0">
        <div class="badge badge-warning badge-md">OTA</div> 
        Firmware & Filesystem Update
      </h2>
      <div class="card-body p-4">
        
        <!-- Current Firmware Info -->
        <div class="stats stats-vertical md:stats-horizontal shadow bg-base-100 w-full mb-6">
          <div class="stat">
            <div class="stat-title">Current Version</div>
            <div class="stat-value text-lg">{{ otaStatus.current_version || 'Unknown' }}</div>
            <div class="stat-desc">{{ otaStatus.compile_date }} {{ otaStatus.compile_time }}</div>
          </div>
          <div class="stat">
            <div class="stat-title">Running Partition</div>
            <div class="stat-value text-lg">{{ otaStatus.running_partition || 'Unknown' }}</div>
          </div>
          <div class="stat">
            <div class="stat-title">Update Partition</div>
            <div class="stat-value text-lg">{{ otaStatus.next_update_partition || 'Unknown' }}</div>
          </div>
        </div>

        <!-- OTA Status Alert -->
        <div v-if="otaStatus.in_progress" class="alert alert-info mb-4">
          <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" class="stroke-current shrink-0 w-6 h-6">
            <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M13 16h-1v-4h-1m1-4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z"></path>
          </svg>
          <div>
            <h3 class="font-bold">OTA Update in Progress</h3>
            <div class="text-xs">{{ formatBytes(otaStatus.bytes_written) }} / {{ formatBytes(otaStatus.total_bytes) }}</div>
          </div>
        </div>

        <div v-if="otaStatus.error" class="alert alert-error mb-4">
          <svg xmlns="http://www.w3.org/2000/svg" class="stroke-current shrink-0 h-6 w-6" fill="none" viewBox="0 0 24 24">
            <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M10 14l2-2m0 0l2-2m-2 2l-2-2m2 2l2 2m7-2a9 9 0 11-18 0 9 9 0 0118 0z" />
          </svg>
          <div>
            <h3 class="font-bold">OTA Error</h3>
            <div class="text-xs">{{ otaStatus.error }}</div>
          </div>
        </div>

        <!-- File Upload Section -->
        <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
          <!-- Firmware Upload -->
          <div class="form-control w-full">
            <label class="label">
              <span class="label-text font-semibold">Firmware File (.bin)</span>
            </label>
            <input 
              ref="firmwareInput"
              type="file" 
              accept=".bin"
              @change="onFirmwareSelected"
              :disabled="isDisabled"
              class="file-input file-input-bordered w-full" 
            />
            <label class="label">
              <span class="label-text-alt">Select compiled firmware binary</span>
            </label>
          </div>
          
          <!-- LittleFS Upload -->
          <div class="form-control w-full">
            <label class="label">
              <span class="label-text font-semibold">LittleFS File (.bin)</span>
            </label>
            <input 
              ref="littlefsInput"
              type="file" 
              accept=".bin"
              @change="onLittlefsSelected"
              :disabled="isDisabled"
              class="file-input file-input-bordered w-full" 
            />
            <label class="label">
              <span class="label-text-alt">Select filesystem binary (optional)</span>
            </label>
          </div>
        </div>

        <!-- Selected Files Info -->
        <div v-if="firmwareFile || littlefsFile" class="grid grid-cols-1 md:grid-cols-2 gap-4 mb-4">
          <!-- Firmware File Info -->
          <div v-if="firmwareFile" class="bg-base-100 rounded-lg p-4">
            <div class="flex items-center justify-between">
              <div>
                <div class="font-semibold text-primary">📱 {{ firmwareFile.name }}</div>
                <div class="text-sm opacity-70">{{ formatBytes(firmwareFile.size) }}</div>
              </div>
              <button 
                @click="clearFirmwareFile"
                :disabled="isDisabled"
                class="btn btn-ghost btn-sm"
              >
                <svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 18L18 6M6 6l12 12" />
                </svg>
              </button>
            </div>
          </div>
          
          <!-- LittleFS File Info -->
          <div v-if="littlefsFile" class="bg-base-100 rounded-lg p-4">
            <div class="flex items-center justify-between">
              <div>
                <div class="font-semibold text-secondary">🗂️ {{ littlefsFile.name }}</div>
                <div class="text-sm opacity-70">{{ formatBytes(littlefsFile.size) }}</div>
              </div>
              <button 
                @click="clearLittlefsFile"
                :disabled="isDisabled"
                class="btn btn-ghost btn-sm"
              >
                <svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 18L18 6M6 6l12 12" />
                </svg>
              </button>
            </div>
          </div>
        </div>

        <!-- Progress Bar -->
        <div v-if="isDisabled" class="mb-4">
          <div class="flex justify-between text-sm mb-2">
            <span>{{ currentUploadStep }} Progress</span>
            <span>{{ Math.round(progressPercent) }}%</span>
          </div>
          <progress 
            class="progress" 
            :class="{
              'progress-primary': otaStatus.upload_type === 'firmware',
              'progress-secondary': otaStatus.upload_type === 'littlefs'
            }"
            :value="progressPercent" 
            max="100"
          ></progress>
          <div class="text-xs text-center mt-1 opacity-70">
            {{ formatBytes(currentBytes) }} / {{ formatBytes(totalBytes) }}
          </div>
          <div v-if="sequentialUpload" class="text-xs text-center mt-1 opacity-70">
            {{ uploadStepText }}
          </div>
        </div>

        <!-- Upload Buttons -->
        <div class="card-actions justify-end gap-2">
          <button 
            @click="uploadFirmware"
            :disabled="!canUploadFirmware"
            class="btn btn-primary"
          >
            <span v-if="uploading && currentUploadType === 'firmware'" class="loading loading-spinner loading-sm"></span>
            <svg v-else xmlns="http://www.w3.org/2000/svg" class="h-5 w-5 mr-2" fill="none" viewBox="0 0 24 24" stroke="currentColor">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M7 16a4 4 0 01-.88-7.903A5 5 0 1115.9 6L16 6a5 5 0 011 9.9M15 13l-3-3m0 0l-3 3m3-3v12" />
            </svg>
            {{ (uploading && currentUploadType === 'firmware') ? 'Uploading...' : 'Upload Firmware' }}
          </button>
          
          <button 
            @click="uploadLittleFS"
            :disabled="!canUploadLittleFS"
            class="btn btn-secondary"
          >
            <span v-if="uploading && currentUploadType === 'littlefs'" class="loading loading-spinner loading-sm"></span>
            <svg v-else xmlns="http://www.w3.org/2000/svg" class="h-5 w-5 mr-2" fill="none" viewBox="0 0 24 24" stroke="currentColor">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M3 7v10a2 2 0 002 2h14a2 2 0 002-2V9a2 2 0 00-2-2H5a2 2 0 00-2-2z" />
            </svg>
            {{ (uploading && currentUploadType === 'littlefs') ? 'Uploading...' : 'Upload LittleFS' }}
          </button>
          
          <button 
            @click="uploadBoth"
            :disabled="!canUploadBoth"
            class="btn btn-accent"
          >
            <span v-if="uploading && sequentialUpload" class="loading loading-spinner loading-sm"></span>
            <svg v-else xmlns="http://www.w3.org/2000/svg" class="h-5 w-5 mr-2" fill="none" viewBox="0 0 24 24" stroke="currentColor">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 16v1a3 3 0 003 3h10a3 3 0 003-3v-1m-4-8l-4-4m0 0L8 8m4-4v12" />
            </svg>
            {{ (uploading && sequentialUpload) ? uploadStepText : 'Upload Both' }}
          </button>
        </div>

        <!-- Warning Notice -->
        <div class="alert alert-warning mt-4">
          <svg xmlns="http://www.w3.org/2000/svg" class="stroke-current shrink-0 h-6 w-6" fill="none" viewBox="0 0 24 24">
            <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-2.5L13.732 4c-.77-.833-1.964-.833-2.732 0L3.732 16.5c-.77.833.192 2.5 1.732 2.5z" />
          </svg>
          <div>
            <h3 class="font-bold">Important Notice</h3>
            <div class="text-xs">
              • Only upload firmware files compiled for this device<br>
              • LittleFS files contain the web interface and configuration<br>
              • "Upload Both" will upload firmware first, then LittleFS without rebooting in between<br>
              • The device will automatically reboot after firmware updates<br>
              • Do not power off the device during the update process<br>
              • Ensure stable power supply during the update
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- OTA Logs Card -->
    <div class="card bg-base-200 shadow-xl">
      <h2 class="card-title p-4 pb-0">
        <div class="badge badge-info badge-md">Logs</div>
        Update History
      </h2>
      <div class="card-body p-4">
        <div v-if="otaLogs.length === 0" class="text-center py-8 opacity-70">
          No update history available
        </div>
        <div v-else class="space-y-2">
          <div 
            v-for="(log, index) in otaLogs" 
            :key="index"
            class="bg-base-100 rounded-lg p-3 text-sm"
          >
            <div class="flex justify-between items-start">
              <div>
                <div class="font-semibold" :class="{
                  'text-success': log.type === 'success',
                  'text-error': log.type === 'error',
                  'text-info': log.type === 'info'
                }">
                  {{ log.message }}
                </div>
                <div class="text-xs opacity-70">{{ log.timestamp }}</div>
              </div>
              <div class="badge badge-sm" :class="{
                'badge-success': log.type === 'success',
                'badge-error': log.type === 'error',
                'badge-info': log.type === 'info'
              }">
                {{ log.type }}
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted, computed, nextTick } from 'vue'
import ws from '@/services/ws.js'

// Reactive state
const firmwareFile = ref(null)
const littlefsFile = ref(null)
const uploading = ref(false)
const currentUploadType = ref('')
const sequentialUpload = ref(false)
const otaStatus = ref({
  in_progress: false,
  bytes_written: 0,
  total_bytes: 0,
  progress_percent: 0,
  error: '',
  current_version: '',
  compile_date: '',
  compile_time: '',
  running_partition: '',
  next_update_partition: ''
})
const otaLogs = ref([])
const firmwareInput = ref(null)
const littlefsInput = ref(null)

// Internal state for cleanup
let statusInterval = null
let lastLoggedPercent = 0

// Computed properties for progress tracking
const progressPercent = computed(() => {
  return otaStatus.value.progress_percent || 0
})

const currentBytes = computed(() => {
  return otaStatus.value.bytes_written || 0
})

const totalBytes = computed(() => {
  return otaStatus.value.total_bytes || getCurrentFileSize()
})

const getCurrentFileSize = () => {
  if (currentUploadType.value === 'firmware' && firmwareFile.value) {
    return firmwareFile.value.size
  } else if (currentUploadType.value === 'littlefs' && littlefsFile.value) {
    return littlefsFile.value.size
  }
  return 0
}

const isDisabled = computed(() => {
  return otaStatus.value.in_progress || uploading.value
})

const canUploadFirmware = computed(() => {
  return firmwareFile.value && !isDisabled.value
})

const canUploadLittleFS = computed(() => {
  return littlefsFile.value && !isDisabled.value
})

const canUploadBoth = computed(() => {
  return firmwareFile.value && littlefsFile.value && !isDisabled.value
})

const currentUploadStep = computed(() => {
  if (otaStatus.value.upload_type === 'firmware') {
    return 'Firmware'
  } else if (otaStatus.value.upload_type === 'littlefs') {
    return 'LittleFS'
  }
  return 'Upload'
})

const uploadStepText = computed(() => {
  if (sequentialUpload.value) {
    if (otaStatus.value.upload_type === 'firmware') {
      return 'Step 1/2: Uploading Firmware...'
    } else if (otaStatus.value.upload_type === 'littlefs') {
      return 'Step 2/2: Uploading LittleFS...'
    }
  }
  return ''
})

// Utility functions
const formatBytes = (bytes) => {
  if (bytes === 0) return '0 Bytes'
  const k = 1024
  const sizes = ['Bytes', 'KB', 'MB', 'GB']
  const i = Math.floor(Math.log(bytes) / Math.log(k))
  return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i]
}

const addLog = (type, message) => {
  const timestamp = new Date().toLocaleString()
  otaLogs.value.unshift({ type, message, timestamp })
  
  // Keep only last 20 logs
  if (otaLogs.value.length > 20) {
    otaLogs.value = otaLogs.value.slice(0, 20)
  }
}

// WebSocket message handler
const handleWebSocketMessage = (evt) => {
  if (evt.type === 'message' && evt.data.type === 'ota_status') {
    const previousStatus = { ...otaStatus.value }
    otaStatus.value = { ...otaStatus.value, ...evt.data }
    
    // Log status changes
    if (evt.data.error && evt.data.error !== previousStatus.error) {
      addLog('error', `OTA Error: ${evt.data.error}`)
    } else if (evt.data.in_progress && evt.data.progress_percent !== undefined) {
      // Log progress milestones (every 10%)
      const percent = Math.round(evt.data.progress_percent)
      if (percent > 0 && percent !== lastLoggedPercent && percent % 10 === 0) {
        addLog('info', `Upload progress: ${percent}%`)
        lastLoggedPercent = percent
      }
    } else if (!evt.data.in_progress && previousStatus.in_progress) {
      // OTA completed or stopped
      if (!evt.data.error) {
        addLog('success', 'OTA update completed successfully')
      }
      lastLoggedPercent = 0
    }
  }
}

// File handling
const onFirmwareSelected = (event) => {
  const file = event.target.files[0]
  if (!file) return

  if (file.name.endsWith('.bin')) {
    firmwareFile.value = file
    addLog('info', `Selected firmware: ${file.name} (${formatBytes(file.size)})`)
  } else {
    addLog('error', 'Please select a .bin firmware file')
    clearFirmwareInput()
  }
}

const onLittlefsSelected = (event) => {
  const file = event.target.files[0]
  if (!file) return

  if (file.name.endsWith('.bin')) {
    littlefsFile.value = file
    addLog('info', `Selected LittleFS: ${file.name} (${formatBytes(file.size)})`)
  } else {
    addLog('error', 'Please select a .bin LittleFS file')
    clearLittlefsInput()
  }
}

const clearFirmwareInput = () => {
  if (firmwareInput.value) {
    firmwareInput.value.value = ''
  }
}

const clearLittlefsInput = () => {
  if (littlefsInput.value) {
    littlefsInput.value.value = ''
  }
}

const clearFirmwareFile = () => {
  firmwareFile.value = null
  clearFirmwareInput()
  addLog('info', 'Firmware file cleared')
}

const clearLittlefsFile = () => {
  littlefsFile.value = null
  clearLittlefsInput()
  addLog('info', 'LittleFS file cleared')
}

// OTA operations
const uploadFirmware = async (skipReboot = false) => {
  if (!firmwareFile.value) {
    addLog('error', 'Please select a firmware file first')
    return false
  }

  if (isDisabled.value) {
    addLog('error', 'Upload already in progress')
    return false
  }

  uploading.value = true
  currentUploadType.value = 'firmware'
  lastLoggedPercent = 0
  addLog('info', `Starting firmware upload: ${firmwareFile.value.name}`)

  try {
    const url = skipReboot ? '/ota/upload?skipReboot=true' : '/ota/upload'
    const response = await fetch(url, {
      method: 'POST',
      body: firmwareFile.value,
      headers: {
        'Content-Type': 'application/octet-stream'
      }
    })

    if (response.ok) {
      const result = await response.json()
      addLog('success', result.message || 'Firmware uploaded successfully')
      
      if (!skipReboot) {
        // Clear file after successful upload
        await nextTick()
        clearFirmwareFile()
      }
      return true
    } else {
      const errorText = await response.text()
      throw new Error(errorText || `HTTP ${response.status}: ${response.statusText}`)
    }
  } catch (error) {
    console.error('Firmware upload failed:', error)
    addLog('error', `Firmware upload failed: ${error.message}`)
    return false
  } finally {
    if (!skipReboot) {
      uploading.value = false
      currentUploadType.value = ''
    }
  }
}

const uploadLittleFS = async (ignoreDisabled = false) => {
  if (!littlefsFile.value) {
    addLog('error', 'Please select a LittleFS file first')
    return false
  }

  if (isDisabled.value && !ignoreDisabled) {
    addLog('error', 'Upload already in progress')
    return false
  }

  uploading.value = true
  currentUploadType.value = 'littlefs'
  lastLoggedPercent = 0
  addLog('info', `Starting LittleFS upload: ${littlefsFile.value.name}`)

  try {
    const response = await fetch('/ota/littlefs', {
      method: 'POST',
      body: littlefsFile.value,
      headers: {
        'Content-Type': 'application/octet-stream'
      }
    })

    if (response.ok) {
      const result = await response.json()
      addLog('success', result.message || 'LittleFS uploaded successfully')
      
      // Clear file after successful upload
      await nextTick()
      clearLittlefsFile()
      return true
    } else {
      const errorText = await response.text()
      throw new Error(errorText || `HTTP ${response.status}: ${response.statusText}`)
    }
  } catch (error) {
    console.error('LittleFS upload failed:', error)
    addLog('error', `LittleFS upload failed: ${error.message}`)
    return false
  } finally {
    uploading.value = false
    currentUploadType.value = ''
  }
}

const uploadBoth = async () => {
  if (!firmwareFile.value || !littlefsFile.value) {
    addLog('error', 'Please select both firmware and LittleFS files')
    return
  }

  if (isDisabled.value) {
    addLog('error', 'Upload already in progress')
    return
  }

  sequentialUpload.value = true
  addLog('info', 'Starting sequential upload: firmware first, then LittleFS')

  try {
    // Step 1: Upload firmware with skipReboot=true
    const firmwareSuccess = await uploadFirmware(true)
    if (!firmwareSuccess) {
      throw new Error('Firmware upload failed')
    }

    // Wait a moment between uploads
    await new Promise(resolve => setTimeout(resolve, 1000))

    // Step 2: Upload LittleFS (bypass disabled guard since we're in a controlled sequential flow)
    const littlefsSuccess = await uploadLittleFS(true)
    if (!littlefsSuccess) {
      throw new Error('LittleFS upload failed')
    }

    addLog('success', 'Both uploads completed successfully! Device will reboot now.')
    
    // Clear both files after successful upload
    await nextTick()
    clearFirmwareFile()
    clearLittlefsFile()
    
    // Trigger reboot manually after both uploads
    setTimeout(async () => {
      addLog('info', 'Rebooting device...')
      try {
        await fetch('/ota/reboot', { method: 'POST' })
      } catch (error) {
        // Expected - device will reboot and connection will be lost
        console.log('Reboot triggered, connection lost as expected')
      }
    }, 2000)
    
  } catch (error) {
    console.error('Sequential upload failed:', error)
    addLog('error', `Sequential upload failed: ${error.message}`)
  } finally {
    uploading.value = false
    currentUploadType.value = ''
    sequentialUpload.value = false
  }
}

const requestOTAStatus = () => {
  if (ws && ws.connected) {
    ws.send({ type: 'ota_status' })
  }
}

// const startStatusPolling = () => {
//   if (statusInterval) return // Already polling
  
//   statusInterval = setInterval(() => {
//     if (otaStatus.value.in_progress || uploading.value) {
//       requestOTAStatus()
//     }
//   }, 2000)
// }

// const stopStatusPolling = () => {
//   if (statusInterval) {
//     clearInterval(statusInterval)
//     statusInterval = null
//   }
// }

let wsOff = null

// Lifecycle management
onMounted(() => {
  // Register WebSocket handler
  if (ws && typeof ws.on === 'function') {
    wsOff = ws.on(handleWebSocketMessage)
  } else {
    addLog('error', 'WebSocket service not available')
  }
  
  // Request initial OTA status
  requestOTAStatus()
  
//   // Start polling for status updates
//   startStatusPolling()
  
//   addLog('info', 'OTA component initialized')
})

onUnmounted(() => {
  // Clean up WebSocket handler
  if (ws && typeof wsOff === 'function') {
    wsOff()
  }
  
  // Clean up polling interval
  // stopStatusPolling()
})
</script>

<style scoped>
.list-row + .list-row {
  margin-top: 1rem;
}
</style>
