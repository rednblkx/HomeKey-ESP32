<script lang="ts">
	import { onDestroy, onMount } from 'svelte';
  import ws, { type WebSocketEvent } from '$lib/services/ws.js';
  import type { ApiResponse, Either, OTAStatus } from '$lib/types/api';
  import { type WebSocketState } from '$lib/stores/websocket.svelte';

	let firmwareFile = $state<File | null>(null);
	let littlefsFile = $state<File | null>(null);
	let uploading = $state(false);
	let currentUploadType = $state('');
	let sequentialUpload = $state(false);
	let otaStatus = $state({
		in_progress: false,
		bytes_written: 0,
		total_bytes: 0,
		progress_percent: 0,
		error: '',
		current_version: '',
		compile_date: '',
		compile_time: '',
		running_partition: '',
		next_update_partition: '',
		upload_type: ''
	});

	let otaLogs = $state<{ type: string; message: string; timestamp: string }[]>([]);
	let firmwareInput = $state<HTMLInputElement | null>(null);
	let littlefsInput = $state<HTMLInputElement | null>(null);

	let lastLoggedPercent = $state(0);

	let progressPercent = $derived(otaStatus.progress_percent || 0);

	let currentBytes = $derived(otaStatus.bytes_written || 0);

	let totalBytes = $derived(() => {
		return otaStatus.total_bytes || getCurrentFileSize();
	});

	function getCurrentFileSize() {
		if (currentUploadType === 'firmware' && firmwareFile) {
			return firmwareFile.size;
		} else if (currentUploadType === 'littlefs' && littlefsFile) {
			return littlefsFile.size;
		}
		return 0;
	}

	let isDisabled = $derived(otaStatus.in_progress || uploading);

	let canUploadFirmware = $derived(firmwareFile && !isDisabled);

	let canUploadLittleFS = $derived(littlefsFile && !isDisabled);

	let canUploadBoth = $derived(firmwareFile && littlefsFile && !isDisabled);

	let currentUploadStep = $derived(() => {
		if (otaStatus.upload_type === 'firmware') {
			return 'Firmware';
		} else if (otaStatus.upload_type === 'littlefs') {
			return 'LittleFS';
		}
		return 'Upload';
	});

	let uploadStepText = $derived(() => {
		if (sequentialUpload) {
			if (otaStatus.upload_type === 'firmware') {
				return 'Step 1/2: Uploading Firmware...';
			} else if (otaStatus.upload_type === 'littlefs') {
				return 'Step 2/2: Uploading LittleFS...';
			}
		}
		return '';
	});

	function formatBytes(bytes: number) {
		if (bytes === 0) return '0 Bytes';
		const k = 1024;
		const sizes = ['Bytes', 'KB', 'MB', 'GB'];
		const i = Math.floor(Math.log(bytes) / Math.log(k));
		return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
	}

	function addLog(type: string, message: string) {
		const timestamp = new Date().toLocaleString();
		otaLogs.unshift({ type, message, timestamp });

		if (otaLogs.length > 20) {
			otaLogs = otaLogs.slice(0, 20);
		}
	}
	function handleWebSocketMessage(evt: WebSocketEvent<Either<OTAStatus, WebSocketState>>) {
		if (evt.type === 'message' && evt.data.type === 'ota_status') {
			const previousStatus = { ...otaStatus };
			otaStatus = { ...otaStatus, ...evt.data };

			if (evt.data.error && evt.data.error !== previousStatus.error) {
				addLog('error', `OTA Error: ${evt.data.error}`);
			} else if (evt.data.in_progress && evt.data.progress_percent !== undefined) {
				const percent = Math.round(evt.data.progress_percent);
				if (percent > 0 && percent !== lastLoggedPercent && percent % 10 === 0) {
					addLog('info', `Upload progress: ${percent}%`);
					lastLoggedPercent = percent;
				}
			} else if (!evt.data.in_progress && previousStatus.in_progress) {
				if (!evt.data.error) {
					addLog('success', 'OTA update completed successfully');
				}
				lastLoggedPercent = 0;
			}
		} else if(evt.type == "status" && evt.data.state == "open") {
      requestOTAStatus();
    }
	}

	function onFirmwareSelected(event: Event) {
		const file = (event.target as HTMLInputElement).files?.[0];
		if (!file) return;

		if (file.name.endsWith('.bin')) {
			firmwareFile = file;
			addLog('info', `Selected firmware: ${file.name} (${formatBytes(file.size)})`);
		} else {
			addLog('error', 'Please select a .bin firmware file');
			clearFirmwareInput();
		}
	}

	function onLittlefsSelected(event: Event) {
		const file = (event.target as HTMLInputElement).files?.[0];
		if (!file) return;

		if (file.name.endsWith('.bin')) {
			littlefsFile = file;
			addLog('info', `Selected LittleFS: ${file.name} (${formatBytes(file.size)})`);
		} else {
			addLog('error', 'Please select a .bin LittleFS file');
			clearLittlefsInput();
		}
	}

	function clearFirmwareInput() {
		if (firmwareInput) {
			firmwareInput.value = '';
		}
	}

	function clearLittlefsInput() {
		if (littlefsInput) {
			littlefsInput.value = '';
		}
	}

	function clearFirmwareFile() {
		firmwareFile = null;
		clearFirmwareInput();
		addLog('info', 'Firmware file cleared');
	}

	function clearLittlefsFile() {
		littlefsFile = null;
		clearLittlefsInput();
		addLog('info', 'LittleFS file cleared');
	}

	// OTA operations
	async function uploadFirmware(skipReboot = false) {
		if (!firmwareFile) {
			addLog('error', 'Please select a firmware file first');
			return false;
		}

		if (isDisabled) {
			addLog('error', 'Upload already in progress');
			return false;
		}

		uploading = true;
		currentUploadType = 'firmware';
		lastLoggedPercent = 0;
		addLog('info', `Starting firmware upload: ${firmwareFile.name}`);

		try {
			const url = `/ota/upload?skipReboot=${skipReboot}`;
			const response = await fetch(url, {
				method: 'POST',
				body: firmwareFile,
				headers: {
					'Content-Type': 'application/octet-stream'
				}
			});

      const result : ApiResponse<undefined> = await response.json();
      if(result.success) {
        addLog('success', result.message || 'Firmware uploaded successfully');
        if (!skipReboot) {
          clearFirmwareFile();
        }
        return true;
      } else {
        addLog('error', result.error || 'Firmware upload failed');
        return false;
      }
		} catch (error) {
			console.error('Firmware upload failed:', error);
			addLog('error', `Firmware upload failed: ${error instanceof Error ? error.message : String(error)}`);
			return false;
		} finally {
			if (!skipReboot) {
				uploading = false;
				currentUploadType = '';
			}
		}
	}

	async function uploadLittleFS(ignoreDisabled = false, skipReboot = true) {
		if (!littlefsFile) {
			addLog('error', 'Please select a LittleFS file first');
			return false;
		}

		if (isDisabled && !ignoreDisabled) {
			addLog('error', 'Upload already in progress');
			return false;
		}

		uploading = true;
		currentUploadType = 'littlefs';
		lastLoggedPercent = 0;
		addLog('info', `Starting LittleFS upload: ${littlefsFile.name}`);

		try {
			const response = await fetch(`/ota/littlefs?skipReboot=${skipReboot}`, {
				method: 'POST',
				body: littlefsFile,
				headers: {
					'Content-Type': 'application/octet-stream'
				}
			});

      const result : ApiResponse<undefined> = await response.json();
      if(result.success) {
        addLog('success', result.message || 'LittleFS uploaded successfully');
        clearLittlefsFile();
        return true;
      } else {
        addLog('error', result.error || 'LittleFS upload failed');
        return false;
      }
		} catch (error) {
			console.error('LittleFS upload failed:', error);
			addLog('error', `LittleFS upload failed: ${error instanceof Error ? error.message : String(error)}`);
			return false;
		} finally {
			uploading = false;
			currentUploadType = '';
		}
	}

	async function uploadBoth() {
		if (!firmwareFile || !littlefsFile) {
			addLog('error', 'Please select both firmware and LittleFS files');
			return;
		}

		if (isDisabled) {
			addLog('error', 'Upload already in progress');
			return;
		}

		sequentialUpload = true;
		addLog('info', 'Starting sequential upload: firmware first, then LittleFS');

		try {
			const firmwareSuccess = await uploadFirmware(true);
			if (!firmwareSuccess) {
				throw new Error('Firmware upload failed');
			}

			await new Promise(resolve => setTimeout(resolve, 1000));

			const littlefsSuccess = await uploadLittleFS(true, false);
			if (!littlefsSuccess) {
				throw new Error('LittleFS upload failed');
			}

			addLog('success', 'Both uploads completed successfully! Device will reboot now.');

			clearFirmwareFile();
			clearLittlefsFile();
		} catch (error) {
			console.error('Sequential upload failed:', error);
			addLog('error', `Sequential upload failed: ${error instanceof Error ? error.message : String(error)}`);
		} finally {
			uploading = false;
			currentUploadType = '';
			sequentialUpload = false;
		}
	}

	function requestOTAStatus() {
		if (ws && ws.connected) {
			ws.send({ type: 'ota_status' });
		}
	}

	let wsUnsubscribe = $state<(() => void) | null>(null);

	onMount(() => {
		if (ws && typeof ws.on === 'function') {
			wsUnsubscribe = ws.on(handleWebSocketMessage);
		} else {
			addLog('error', 'WebSocket service not available');
		}

		requestOTAStatus();
	});

	onDestroy(() => {
		if (wsUnsubscribe && typeof wsUnsubscribe === 'function') {
			wsUnsubscribe();
		}
	});
</script>

<div class="w-full max-w-4xl space-y-6 py-6">
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
					<div class="stat-value text-lg">{otaStatus.current_version || 'Unknown'}</div>
					<div class="stat-desc">{otaStatus.compile_date} {otaStatus.compile_time}</div>
				</div>
				<div class="stat">
					<div class="stat-title">Running Partition</div>
					<div class="stat-value text-lg">{otaStatus.running_partition || 'Unknown'}</div>
				</div>
				<div class="stat">
					<div class="stat-title">Update Partition</div>
					<div class="stat-value text-lg">{otaStatus.next_update_partition || 'Unknown'}</div>
				</div>
			</div>

			<!-- OTA Status Alert -->
			{#if otaStatus.in_progress}
				<div class="alert alert-info mb-4">
					<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" class="stroke-current shrink-0 w-6 h-6">
						<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M13 16h-1v-4h-1m1-4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z"></path>
					</svg>
					<div>
						<h3 class="font-bold">OTA Update in Progress</h3>
						<div class="text-xs">{formatBytes(otaStatus.bytes_written)} / {formatBytes(otaStatus.total_bytes)}</div>
					</div>
				</div>
			{/if}

			{#if otaStatus.error}
				<div class="alert alert-error mb-4">
					<svg xmlns="http://www.w3.org/2000/svg" class="stroke-current shrink-0 h-6 w-6" fill="none" viewBox="0 0 24 24">
						<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M10 14l2-2m0 0l2-2m-2 2l-2-2m2 2l2 2m7-2a9 9 0 11-18 0 9 9 0 0118 0z" />
					</svg>
					<div>
						<h3 class="font-bold">OTA Error</h3>
						<div class="text-xs">{otaStatus.error}</div>
					</div>
				</div>
			{/if}

			<!-- File Upload Section -->
			<div class="grid grid-cols-1 md:grid-cols-2 gap-4">
				<!-- Firmware Upload -->
				<div class="form-control w-full">
					<label class="label" for="firmware-input">
						<span class="label-text font-semibold">Firmware File (.bin)</span>
					</label>
					<input
						id="firmware-input"
						bind:this={firmwareInput}
						type="file"
						accept=".bin"
						onchange={onFirmwareSelected}
						disabled={isDisabled}
						class="file-input file-input-bordered w-full"
					/>
					<label class="label" for="firmware-input">
						<span class="label-text-alt">Select compiled firmware binary</span>
					</label>
				</div>

				<!-- LittleFS Upload -->
				<div class="form-control w-full">
					<label class="label" for="littlefs-input">
						<span class="label-text font-semibold">LittleFS File (.bin)</span>
					</label>
					<input
						id="littlefs-input"
						bind:this={littlefsInput}
						type="file"
						accept=".bin"
						onchange={onLittlefsSelected}
						disabled={isDisabled}
						class="file-input file-input-bordered w-full"
					/>
					<label class="label" for="littlefs-input">
						<span class="label-text-alt">Select filesystem binary (optional)</span>
					</label>
				</div>
			</div>

			<!-- Selected Files Info -->
			{#if firmwareFile || littlefsFile}
				<div class="grid grid-cols-1 md:grid-cols-2 gap-4 mb-4">
					<!-- Firmware File Info -->
					{#if firmwareFile}
						<div class="bg-base-100 rounded-lg p-4">
							<div class="flex items-center justify-between">
								<div>
									<div class="font-semibold text-primary">📱 {firmwareFile.name}</div>
									<div class="text-sm opacity-70">{formatBytes(firmwareFile.size)}</div>
								</div>
								<button
									onclick={clearFirmwareFile}
									disabled={isDisabled}
									class="btn btn-ghost btn-sm"
									aria-label="Clear firmware file"
								>
									<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor">
										<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 18L18 6M6 6l12 12" />
									</svg>
								</button>
							</div>
						</div>
					{/if}

					<!-- LittleFS File Info -->
					{#if littlefsFile}
						<div class="bg-base-100 rounded-lg p-4">
							<div class="flex items-center justify-between">
								<div>
									<div class="font-semibold text-secondary">🗂️ {littlefsFile.name}</div>
									<div class="text-sm opacity-70">{formatBytes(littlefsFile.size)}</div>
								</div>
								<button
									onclick={clearLittlefsFile}
									disabled={isDisabled}
									class="btn btn-ghost btn-sm"
									aria-label="Clear LittleFS file"
								>
									<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor">
										<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 18L18 6M6 6l12 12" />
									</svg>
								</button>
							</div>
						</div>
					{/if}
				</div>
			{/if}

			<!-- Progress Bar -->
			{#if isDisabled}
				<div class="mb-4">
					<div class="flex justify-between text-sm mb-2">
						<span>{currentUploadStep()} Progress</span>
						<span>{Math.round(progressPercent)}%</span>
					</div>
					<progress
						class="progress"
						class:progress-primary={otaStatus.upload_type === 'firmware'}
						class:progress-secondary={otaStatus.upload_type === 'littlefs'}
						value={progressPercent}
						max="100"
					></progress>
					<div class="text-xs text-center mt-1 opacity-70">
						{formatBytes(currentBytes)} / {formatBytes(totalBytes())}
					</div>
					{#if sequentialUpload}
						<div class="text-xs text-center mt-1 opacity-70">
							{uploadStepText()}
						</div>
					{/if}
				</div>
			{/if}

			<!-- Upload Buttons -->
			<div class="card-actions justify-end gap-2">
				<button
					onclick={() => uploadFirmware()}
					disabled={!canUploadFirmware}
					class="btn btn-primary"
				>
					{#if uploading && currentUploadType === 'firmware'}
						<span class="loading loading-spinner loading-sm"></span>
					{/if}
					{(uploading && currentUploadType === 'firmware') ? 'Uploading...' : 'Upload Firmware'}
				</button>

				<button
					onclick={() => uploadLittleFS()}
					disabled={!canUploadLittleFS}
					class="btn btn-secondary"
				>
					{#if uploading && currentUploadType === 'littlefs'}
						<span class="loading loading-spinner loading-sm"></span>
					{/if}
					{(uploading && currentUploadType === 'littlefs') ? 'Uploading...' : 'Upload LittleFS'}
				</button>

				<button
					onclick={uploadBoth}
					disabled={!canUploadBoth}
					class="btn btn-accent"
				>
					{#if uploading && sequentialUpload}
						<span class="loading loading-spinner loading-sm"></span>
					{/if}
					{(uploading && sequentialUpload) ? uploadStepText() : 'Upload Both'}
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
			{#if otaLogs.length === 0}
				<div class="text-center py-8 opacity-70">
					No update history available
				</div>
			{:else}
				<div class="space-y-2">
					{#each otaLogs as log}
						<div class="bg-base-100 rounded-lg p-3 text-sm">
							<div class="flex justify-between items-start">
								<div>
									<div class="font-semibold" class:text-success={log.type === 'success'} class:text-error={log.type === 'error'} class:text-info={log.type === 'info'}>
										{log.message}
									</div>
									<div class="text-xs opacity-70">{log.timestamp}</div>
								</div>
								<div class="badge badge-sm" class:badge-success={log.type === 'success'} class:badge-error={log.type === 'error'} class:badge-info={log.type === 'info'}>
									{log.type}
								</div>
							</div>
						</div>
					{/each}
				</div>
			{/if}
		</div>
	</div>
</div>
