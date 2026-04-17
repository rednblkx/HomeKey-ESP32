<script lang="ts">
	import { onDestroy, onMount } from "svelte";
	import ws, { type WebSocketEvent } from "$lib/services/ws.js";
	import type { ApiResponse, Either, OTAStatus } from "$lib/types/api";
	import { type WebSocketState } from "$lib/stores/websocket.svelte";
	import { systemInfo } from "$lib/stores/system.svelte";
	import { getChipModelString } from "$lib/utils/chipModel";

	let firmwareFile = $state<File | null>(null);
	let littlefsFile = $state<File | null>(null);
	let uploading = $state(false);
	let currentUploadType = $state("");
	let sequentialUpload = $state(false);
	let otaStatus = $state({
		in_progress: false,
		bytes_written: 0,
		total_bytes: 0,
		progress_percent: 0,
		error: "",
		current_version: "",
		compile_date: "",
		compile_time: "",
		running_partition: "",
		next_update_partition: "",
		upload_type: "",
	});
	let chipModel = $derived(systemInfo.chip_model || 0);

	let otaLogs = $state<
		{ type: string; message: string; timestamp: string }[]
	>([]);
	let firmwareInput = $state<HTMLInputElement | null>(null);
	let littlefsInput = $state<HTMLInputElement | null>(null);

	let lastLoggedPercent = $state(0);

	let progressPercent = $derived(otaStatus.progress_percent || 0);
	let currentBytes = $derived(otaStatus.bytes_written || 0);
	let totalBytes = $derived(otaStatus.total_bytes || getCurrentFileSize());

	function getCurrentFileSize() {
		if (currentUploadType === "firmware" && firmwareFile) {
			return firmwareFile.size;
		} else if (currentUploadType === "littlefs" && littlefsFile) {
			return littlefsFile.size;
		}
		return 0;
	}

	let isDisabled = $derived(otaStatus.in_progress || uploading);
	let canUploadFirmware = $derived(firmwareFile && !isDisabled);
	let canUploadLittleFS = $derived(littlefsFile && !isDisabled);
	let canUploadBoth = $derived(firmwareFile && littlefsFile && !isDisabled);

	let currentUploadStep = $derived(
		otaStatus.upload_type === "firmware"
			? "Firmware"
			: otaStatus.upload_type === "littlefs"
				? "LittleFS"
				: "Upload",
	);
	let uploadStepText = $derived(
		sequentialUpload
			? otaStatus.upload_type === "firmware"
				? "Step 1/2: Uploading Firmware..."
				: otaStatus.upload_type === "littlefs"
					? "Step 2/2: Uploading LittleFS..."
					: ""
			: "",
	);

	function formatBytes(bytes: number) {
		if (bytes === 0) return "0 Bytes";
		const k = 1024;
		const sizes = ["Bytes", "KB", "MB", "GB"];
		const i = Math.floor(Math.log(bytes) / Math.log(k));
		return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + " " + sizes[i];
	}

	function addLog(type: string, message: string) {
		const timestamp = new Date().toLocaleString();
		otaLogs.unshift({ type, message, timestamp });
		if (otaLogs.length > 20) {
			otaLogs = otaLogs.slice(0, 20);
		}
	}

	function handleWebSocketMessage(
		evt: WebSocketEvent<Either<OTAStatus, WebSocketState>>,
	) {
		if (evt.type === "message" && evt.data.type === "ota_status") {
			const previousStatus = { ...otaStatus };
			otaStatus = { ...otaStatus, ...evt.data };

			if (evt.data.error && evt.data.error !== previousStatus.error) {
				addLog("error", `OTA Error: ${evt.data.error}`);
			} else if (
				evt.data.in_progress &&
				evt.data.progress_percent !== undefined
			) {
				const percent = Math.round(evt.data.progress_percent);
				if (
					percent > 0 &&
					percent !== lastLoggedPercent &&
					percent % 10 === 0
				) {
					addLog("info", `Upload progress: ${percent}%`);
					lastLoggedPercent = percent;
				}
			} else if (!evt.data.in_progress && previousStatus.in_progress) {
				lastLoggedPercent = 0;
			}
		} else if (evt.type == "message" && evt.data.type == "ota_info") {
			otaStatus = { ...otaStatus, ...evt.data };
		} else if (evt.type == "status" && evt.data.state == "open") {
			requestOTAInfo();
		}
	}

	function onFirmwareSelected(event: Event) {
		const file = (event.target as HTMLInputElement).files?.[0];
		if (!file) return;

		if (file.name.endsWith(".bin")) {
			firmwareFile = file;
			addLog(
				"info",
				`Selected firmware: ${file.name} (${formatBytes(file.size)})`,
			);
		} else {
			addLog("error", "Please select a .bin firmware file");
			clearFirmwareInput();
		}
	}

	function onLittlefsSelected(event: Event) {
		const file = (event.target as HTMLInputElement).files?.[0];
		if (!file) return;

		if (file.name.endsWith(".bin")) {
			littlefsFile = file;
			addLog(
				"info",
				`Selected LittleFS: ${file.name} (${formatBytes(file.size)})`,
			);
		} else {
			addLog("error", "Please select a .bin LittleFS file");
			clearLittlefsInput();
		}
	}

	function clearFirmwareInput() {
		if (firmwareInput) {
			firmwareInput.value = "";
		}
	}

	function clearLittlefsInput() {
		if (littlefsInput) {
			littlefsInput.value = "";
		}
	}

	function clearFirmwareFile() {
		firmwareFile = null;
		clearFirmwareInput();
		addLog("info", "Firmware file cleared");
	}

	function clearLittlefsFile() {
		littlefsFile = null;
		clearLittlefsInput();
		addLog("info", "LittleFS file cleared");
	}

	async function uploadFirmware(skipReboot = false) {
		if (!firmwareFile) {
			addLog("error", "Please select a firmware file first");
			return false;
		}

		if (isDisabled) {
			addLog("error", "Upload already in progress");
			return false;
		}

		uploading = true;
		currentUploadType = "firmware";
		lastLoggedPercent = 0;
		addLog("info", `Starting firmware upload: ${firmwareFile.name}`);

		try {
			const url = `/ota/upload?skipReboot=${skipReboot}`;
			const response = await fetch(url, {
				method: "POST",
				body: firmwareFile,
				headers: {
					"Content-Type": "application/octet-stream",
				},
			});

			const result: ApiResponse<undefined> = await response.json();
			if (result.success) {
				addLog(
					"success",
					result.message || "Firmware uploaded successfully",
				);
				if (!skipReboot) {
					clearFirmwareFile();
				}
				return true;
			} else {
				addLog("error", result.error || "Firmware upload failed");
				return false;
			}
		} catch (error) {
			console.error("Firmware upload failed:", error);
			addLog(
				"error",
				`Firmware upload failed: ${error instanceof Error ? error.message : String(error)}`,
			);
			return false;
		} finally {
			if (!skipReboot) {
				uploading = false;
				currentUploadType = "";
			}
		}
	}

	async function uploadLittleFS(ignoreDisabled = false, skipReboot = true) {
		if (!littlefsFile) {
			addLog("error", "Please select a LittleFS file first");
			return false;
		}

		if (isDisabled && !ignoreDisabled) {
			addLog("error", "Upload already in progress");
			return false;
		}

		uploading = true;
		currentUploadType = "littlefs";
		lastLoggedPercent = 0;
		addLog("info", `Starting LittleFS upload: ${littlefsFile.name}`);

		try {
			const response = await fetch(
				`/ota/littlefs?skipReboot=${skipReboot}`,
				{
					method: "POST",
					body: littlefsFile,
					headers: {
						"Content-Type": "application/octet-stream",
					},
				},
			);

			const result: ApiResponse<undefined> = await response.json();
			if (result.success) {
				addLog(
					"success",
					result.message || "LittleFS uploaded successfully",
				);
				clearLittlefsFile();
				return true;
			} else {
				addLog("error", result.error || "LittleFS upload failed");
				return false;
			}
		} catch (error) {
			console.error("LittleFS upload failed:", error);
			addLog(
				"error",
				`LittleFS upload failed: ${error instanceof Error ? error.message : String(error)}`,
			);
			return false;
		} finally {
			uploading = false;
			currentUploadType = "";
		}
	}

	async function uploadBoth() {
		if (!firmwareFile || !littlefsFile) {
			addLog("error", "Please select both firmware and LittleFS files");
			return;
		}

		if (isDisabled) {
			addLog("error", "Upload already in progress");
			return;
		}

		sequentialUpload = true;
		addLog(
			"info",
			"Starting sequential upload: firmware first, then LittleFS",
		);

		try {
			const firmwareSuccess = await uploadFirmware(true);
			if (!firmwareSuccess) {
				throw new Error("Firmware upload failed");
			}

			await new Promise((resolve) => setTimeout(resolve, 1000));

			const littlefsSuccess = await uploadLittleFS(true, false);
			if (!littlefsSuccess) {
				throw new Error("LittleFS upload failed");
			}

			addLog(
				"success",
				"Both uploads completed successfully! Device will reboot now.",
			);

			clearFirmwareFile();
			clearLittlefsFile();
		} catch (error) {
			console.error("Sequential upload failed:", error);
			addLog(
				"error",
				`Sequential upload failed: ${error instanceof Error ? error.message : String(error)}`,
			);
		} finally {
			uploading = false;
			currentUploadType = "";
			sequentialUpload = false;
		}
	}

	function requestOTAInfo() {
		if (ws && ws.connected) {
			ws.send({ type: "ota_info" });
		}
	}

	let wsUnsubscribe = $state<(() => void) | null>(null);

	onMount(() => {
		if (ws && typeof ws.on === "function") {
			wsUnsubscribe = ws.on(handleWebSocketMessage);
		} else {
			addLog("error", "WebSocket service not available");
		}
		requestOTAInfo();
	});

	onDestroy(() => {
		if (wsUnsubscribe && typeof wsUnsubscribe === "function") {
			wsUnsubscribe();
		}
	});
</script>

<div class="flex flex-col pt-6 gap-6 max-w-6xl mx-auto">
	<!-- Page Header -->
	<div>
		<h1 class="text-2xl font-bold text-base-content">OTA Update</h1>
		<p class="text-base-content/60 text-sm">
			Update firmware and filesystem over-the-air.
		</p>
	</div>

	<!-- Device Information -->
	<div class="card bg-base-200">
		<div class="card-body">
			<div class="flex items-center gap-2 mb-4">
				<svg
					viewBox="0 0 24 24"
					xmlns="http://www.w3.org/2000/svg"
					width="24"
					height="24"
					fill="currentColor"
					class="size-5 text-primary"
				>
					<path
						d="M6 18H18V6H6V18ZM14 20H10V22H8V20H5C4.44772 20 4 19.5523 4 19V16H2V14H4V10H2V8H4V5C4 4.44772 4.44772 4 5 4H8V2H10V4H14V2H16V4H19C19.5523 4 20 4.44772 20 5V8H22V10H20V14H22V16H20V19C20 19.5523 19.5523 20 19 20H16V22H14V20ZM8 8H16V16H8V8Z"
					>
					</path>
				</svg>
				<div>
					<h2 class="card-title text-base">Device Information</h2>
					<p class="text-xs text-base-content/60">
						Current firmware and partition status
					</p>
				</div>
			</div>

			<div class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-3">
				<div class="bg-base-300/50 rounded-lg p-3">
					<div class="text-xs text-base-content/60 mb-1">
						Current Version
					</div>
					<div class="text-sm font-medium">
						{otaStatus.current_version || "Unknown"}
					</div>
				</div>

				<div class="bg-base-300/50 rounded-lg p-3">
					<div class="text-xs text-base-content/60 mb-1">Chip Model</div>
					<div class="text-sm font-medium">
						{getChipModelString(chipModel)}
					</div>
				</div>

				<div class="bg-base-300/50 rounded-lg p-3">
					<div class="text-xs text-base-content/60 mb-1">
						Running Partition
					</div>
					<div class="text-sm font-medium">
						{otaStatus.running_partition || "Unknown"}
					</div>
				</div>

				<div class="bg-base-300/50 rounded-lg p-3">
					<div class="text-xs text-base-content/60 mb-1">
						Update Partition
					</div>
					<div class="text-sm font-medium">
						{otaStatus.next_update_partition || "Unknown"}
					</div>
				</div>
			</div>
		</div>
	</div>

	<!-- Upload Section -->
	<div class="grid grid-cols-1 lg:grid-cols-2 gap-6">
		<!-- Firmware Upload -->
		<div class="card bg-base-200">
			<div class="card-body">
				<div class="flex items-center gap-2 mb-4">
					<svg
						viewBox="0 0 24 24"
						xmlns="http://www.w3.org/2000/svg"
						width="24"
						height="24"
						fill="currentColor"
						class="size-5 text-primary"
					>
						<path
							d="M3 19H21V21H3V19ZM13 13.1716L19.0711 7.1005L20.4853 8.51472L12 17L3.51472 8.51472L4.92893 7.1005L11 13.1716V2H13V13.1716Z"
						></path>
					</svg>
					<div>
						<h2 class="card-title text-base">Firmware</h2>
						<p class="text-xs text-base-content/60">
							Upload compiled firmware binary
						</p>
					</div>
				</div>

				<div class="form-control">
					<label class="label py-1" for="firmware-input">
						<span class="label-text text-xs">Firmware File (.bin)</span>
					</label>
					<input
						id="firmware-input"
						bind:this={firmwareInput}
						type="file"
						accept=".bin"
						onchange={onFirmwareSelected}
						disabled={isDisabled}
						class="file-input file-input-bordered file-input-sm w-full bg-base-300/50"
					/>
					<label class="label py-1" for="firmware-input">
						<span class="label-text-alt text-xs">
							Select compiled firmware binary (.bin)
						</span>
					</label>
				</div>

				{#if firmwareFile}
					<div class="bg-base-300/30 rounded-lg p-3 mt-3">
						<div class="flex items-center justify-between">
							<div class="flex items-center gap-2">
								<svg
									xmlns="http://www.w3.org/2000/svg"
									class="h-4 w-4 text-primary"
									viewBox="0 0 24 24"
									fill="none"
									stroke="currentColor"
									stroke-width="2"
								>
									<path d="M14.5 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V7.5L14.5 2z" />
									<polyline points="14 2 14 8 20 8" />
								</svg>
								<div>
									<div class="text-sm font-medium">
										{firmwareFile.name}
									</div>
									<div class="text-xs text-base-content/60">
										{formatBytes(firmwareFile.size)}
									</div>
								</div>
							</div>
							<button
								onclick={clearFirmwareFile}
								disabled={isDisabled}
								class="btn btn-ghost btn-xs btn-circle"
								aria-label="Clear firmware file"
							>
								<svg
									xmlns="http://www.w3.org/2000/svg"
									class="h-4 w-4"
									fill="none"
									viewBox="0 0 24 24"
									stroke="currentColor"
								>
									<path
										stroke-linecap="round"
										stroke-linejoin="round"
										stroke-width="2"
										d="M6 18L18 6M6 6l12 12"
									/>
								</svg>
							</button>
						</div>
					</div>
				{/if}

				<button
					onclick={() => uploadFirmware()}
					disabled={!canUploadFirmware}
					class="btn btn-sm w-full mt-4 btn-primary text-white border-0"
				>
					{#if uploading && currentUploadType === "firmware"}
						<span class="loading loading-spinner loading-xs"></span>
					{:else}
						<svg
							xmlns="http://www.w3.org/2000/svg"
							class="h-4 w-4"
							viewBox="0 0 24 24"
							fill="none"
							stroke="currentColor"
							stroke-width="2"
						>
							<path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4" />
							<polyline points="17 8 12 3 7 8" />
							<line x1="12" y1="3" x2="12" y2="15" />
						</svg>
					{/if}
					{uploading && currentUploadType === "firmware"
						? "Uploading..."
						: "Upload Firmware"}
				</button>
			</div>
		</div>

		<!-- LittleFS Upload -->
		<div class="card bg-base-200">
			<div class="card-body">
				<div class="flex items-center gap-2 mb-4">
					<svg
						viewBox="0 0 24 24"
						xmlns="http://www.w3.org/2000/svg"
						width="24"
						height="24"
						fill="currentColor"
						class="remixicon size-5 text-primary"
					>
						<path
							d="M5 10.9381C8.61872 10.4869 11.4869 7.61872 11.9381 4H5V10.9381ZM5 12.9506V20H19V4H13.9506C13.4816 8.72442 9.72442 12.4816 5 12.9506ZM4 2H20C20.5523 2 21 2.44772 21 3V21C21 21.5523 20.5523 22 20 22H4C3.44772 22 3 21.5523 3 21V3C3 2.44772 3.44772 2 4 2ZM15 16H17V18H15V16Z"
						>
						</path>
					</svg>
					<div>
						<h2 class="card-title text-base">LittleFS</h2>
						<p class="text-xs text-base-content/60">
							Upload filesystem binary
						</p>
					</div>
				</div>

				<div class="form-control">
					<label class="label py-1" for="littlefs-input">
						<span class="label-text text-xs">LittleFS File (.bin)</span>
					</label>
					<input
						id="littlefs-input"
						bind:this={littlefsInput}
						type="file"
						accept=".bin"
						onchange={onLittlefsSelected}
						disabled={isDisabled}
						class="file-input file-input-bordered file-input-sm w-full bg-base-300/50"
					/>
					<label class="label py-1" for="littlefs-input">
						<span class="label-text-alt text-xs">
							Select filesystem binary (.bin)
						</span>
					</label>
				</div>

				{#if littlefsFile}
					<div class="bg-base-300/30 rounded-lg p-3 mt-3">
						<div class="flex items-center justify-between">
							<div class="flex items-center gap-2">
								<svg
									xmlns="http://www.w3.org/2000/svg"
									class="h-4 w-4 text-primary"
									viewBox="0 0 24 24"
									fill="none"
									stroke="currentColor"
									stroke-width="2"
								>
									<path d="M14.5 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V7.5L14.5 2z" />
									<polyline points="14 2 14 8 20 8" />
								</svg>
								<div>
									<div class="text-sm font-medium">
										{littlefsFile.name}
									</div>
									<div class="text-xs text-base-content/60">
										{formatBytes(littlefsFile.size)}
									</div>
								</div>
							</div>
							<button
								onclick={clearLittlefsFile}
								disabled={isDisabled}
								class="btn btn-ghost btn-xs btn-circle"
								aria-label="Clear LittleFS file"
							>
								<svg
									xmlns="http://www.w3.org/2000/svg"
									class="h-4 w-4"
									fill="none"
									viewBox="0 0 24 24"
									stroke="currentColor"
								>
									<path
										stroke-linecap="round"
										stroke-linejoin="round"
										stroke-width="2"
										d="M6 18L18 6M6 6l12 12"
									/>
								</svg>
							</button>
						</div>
					</div>
				{/if}

				<button
					onclick={() => uploadLittleFS()}
					disabled={!canUploadLittleFS}
					class="btn btn-sm w-full mt-4 btn-primary text-white border-0"
				>
					{#if uploading && currentUploadType === "littlefs"}
						<span class="loading loading-spinner loading-xs"></span>
					{:else}
						<svg
							xmlns="http://www.w3.org/2000/svg"
							class="h-4 w-4"
							viewBox="0 0 24 24"
							fill="none"
							stroke="currentColor"
							stroke-width="2"
						>
							<path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4" />
							<polyline points="17 8 12 3 7 8" />
							<line x1="12" y1="3" x2="12" y2="15" />
						</svg>
					{/if}
					{uploading && currentUploadType === "littlefs"
						? "Uploading..."
						: "Upload LittleFS"}
				</button>
			</div>
		</div>
	</div>

	<!-- Upload Both Button (Full Width) -->
	{#if firmwareFile && littlefsFile}
		<button
			onclick={uploadBoth}
			disabled={!canUploadBoth}
			class="btn btn-md w-full btn-primary text-white border-0"
		>
			{#if uploading && sequentialUpload}
				<span class="loading loading-spinner loading-sm"></span>
			{:else}
				<svg
					xmlns="http://www.w3.org/2000/svg"
					class="h-5 w-5"
					viewBox="0 0 24 24"
					fill="none"
					stroke="currentColor"
					stroke-width="2"
				>
					<path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4" />
					<polyline points="17 8 12 3 7 8" />
					<line x1="12" y1="3" x2="12" y2="15" />
				</svg>
			{/if}
			{uploading && sequentialUpload ? uploadStepText : "Upload Both"}
		</button>
	{/if}

	<!-- Progress Bar -->
	{#if isDisabled}
		<div class="card bg-base-200">
			<div class="card-body py-4">
				<div class="flex justify-between text-sm mb-2">
					<span>{currentUploadStep} Progress</span>
					<span>{Math.round(progressPercent)}%</span>
				</div>
				<progress
					class="progress progress-primary w-full"
					value={progressPercent}
					max="100"
				></progress>
				<div class="text-xs text-center mt-2 text-base-content/60">
					{formatBytes(currentBytes)} / {formatBytes(totalBytes)}
				</div>
			</div>
		</div>
	{/if}

	<!-- Error Alert -->
	{#if otaStatus.error}
		<div class="alert alert-error">
			<svg
				xmlns="http://www.w3.org/2000/svg"
				class="h-5 w-5 shrink-0"
				fill="none"
				viewBox="0 0 24 24"
				stroke="currentColor"
			>
				<path
					stroke-linecap="round"
					stroke-linejoin="round"
					stroke-width="2"
					d="M10 14l2-2m0 0l2-2m-2 2l-2-2m2 2l2 2m7-2a9 9 0 11-18 0 9 9 0 0118 0z"
				/>
			</svg>
			<div>
				<h3 class="font-bold">Upload Error</h3>
				<div class="text-xs">{otaStatus.error}</div>
			</div>
		</div>
	{/if}

	<!-- Upload Log -->
	<div class="card bg-base-200">
		<div class="card-body">
			<h2 class="card-title text-base mb-4">Upload Log</h2>
			{#if otaLogs.length === 0}
				<div class="text-center py-8 text-base-content/50 text-sm">
					No log entries yet
				</div>
			{:else}
				<div class="space-y-2 max-h-60 overflow-y-auto">
					{#each otaLogs as log}
						<div class="bg-base-300/30 rounded-lg p-3 text-sm">
							<div class="flex justify-between items-start gap-2">
								<div class="flex-1">
									<div
										class="font-medium"
										class:text-success={log.type === "success"}
										class:text-error={log.type === "error"}
										class:text-info={log.type === "info"}
									>
										{log.message}
									</div>
									<div class="text-xs text-base-content/50 mt-1">
										{log.timestamp}
									</div>
								</div>
								<div
									class="badge badge-xs shrink-0"
									class:badge-success={log.type === "success"}
									class:badge-error={log.type === "error"}
									class:badge-info={log.type === "info"}
								>
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
