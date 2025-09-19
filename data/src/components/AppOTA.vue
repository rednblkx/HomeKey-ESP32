<template>
  <div class="w-full max-w-4xl space-y-6">
    <!-- OTA Update Card -->
    <div class="card bg-base-200 shadow-xl">
      <h2 class="card-title p-4 pb-0">
        <div class="badge badge-warning badge-md">OTA</div> 
        Firmware Update
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
        <div class="form-control w-full">
          <label class="label">
            <span class="label-text font-semibold">Select Firmware File (.bin)</span>
          </label>
          <input 
            ref="fileInput"
            type="file" 
            accept=".bin"
            @change="onFileSelected"
            :disabled="isDisabled"
            class="file-input file-input-bordered w-full" 
          />
          <label class="label">
            <span class="label-text-alt">Select a compiled firmware binary file</span>
          </label>
        </div>

        <!-- Selected File Info -->
        <div v-if="selectedFile" class="bg-base-100 rounded-lg p-4 mb-4">
          <div class="flex items-center justify-between">
            <div>
              <div class="font-semibold">{{ selectedFile.name }}</div>
              <div class="text-sm opacity-70">{{ formatBytes(selectedFile.size) }}</div>
            </div>
            <button 
              @click="clearFile"
              :disabled="isDisabled"
              class="btn btn-ghost btn-sm"
            >
              <svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 18L18 6M6 6l12 12" />
              </svg>
            </button>
          </div>
        </div>

        <!-- Progress Bar -->
        <div v-if="isDisabled" class="mb-4">
          <div class="flex justify-between text-sm mb-2">
            <span>Upload Progress</span>
            <span>{{ Math.round(progressPercent) }}%</span>
          </div>
          <progress 
            class="progress progress-primary w-full" 
            :value="progressPercent" 
            max="100"
          ></progress>
          <div class="text-xs text-center mt-1 opacity-70">
            {{ formatBytes(currentBytes) }} / {{ formatBytes(totalBytes) }}
          </div>
        </div>

        <!-- Upload Button -->
        <div class="card-actions justify-end">
          <button 
            @click="uploadFirmware"
            :disabled="!canUpload"
            class="btn btn-primary"
          >
            <span v-if="uploading" class="loading loading-spinner loading-sm"></span>
            <svg v-else xmlns="http://www.w3.org/2000/svg" class="h-5 w-5 mr-2" fill="none" viewBox="0 0 24 24" stroke="currentColor">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M7 16a4 4 0 01-.88-7.903A5 5 0 1115.9 6L16 6a5 5 0 011 9.9M15 13l-3-3m0 0l-3 3m3-3v12" />
            </svg>
            {{ uploading ? 'Uploading...' : 'Upload Firmware' }}
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
              • The device will automatically reboot after successful update<br>
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
const selectedFile = ref(null)
const uploading = ref(false)
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
const fileInput = ref(null)

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
  return otaStatus.value.total_bytes || (selectedFile.value ? selectedFile.value.size : 0)
})

const isDisabled = computed(() => {
  return otaStatus.value.in_progress || uploading.value
})

const canUpload = computed(() => {
  return selectedFile.value && !isDisabled.value
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
const onFileSelected = (event) => {
  const file = event.target.files[0]
  if (!file) return

  if (file.name.endsWith('.bin')) {
    selectedFile.value = file
    addLog('info', `Selected file: ${file.name} (${formatBytes(file.size)})`)
  } else {
    addLog('error', 'Please select a .bin firmware file')
    clearFileInput()
  }
}

const clearFileInput = () => {
  if (fileInput.value) {
    fileInput.value.value = ''
  }
}

const clearFile = () => {
  selectedFile.value = null
  clearFileInput()
  addLog('info', 'File selection cleared')
}

// OTA operations
const uploadFirmware = async () => {
  if (!selectedFile.value) {
    addLog('error', 'Please select a firmware file first')
    return
  }

  if (isDisabled.value) {
    addLog('error', 'Upload already in progress')
    return
  }

  uploading.value = true
  lastLoggedPercent = 0
  addLog('info', `Starting firmware upload: ${selectedFile.value.name}`)

  try {
    const response = await fetch('/ota/upload', {
      method: 'POST',
      body: selectedFile.value,
      headers: {
        'Content-Type': 'application/octet-stream'
      }
    })

    if (response.ok) {
      const result = await response.json()
      addLog('success', result.message || 'Firmware uploaded successfully')
      
      // Clear file after successful upload
      await nextTick()
      clearFile()
    } else {
      const errorText = await response.text()
      throw new Error(errorText || `HTTP ${response.status}: ${response.statusText}`)
    }
  } catch (error) {
    console.error('Upload failed:', error)
    addLog('error', `Upload failed: ${error.message}`)
  } finally {
    uploading.value = false
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
