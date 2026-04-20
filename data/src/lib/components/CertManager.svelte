<script lang="ts">
	import type { CertificatesStatus } from '$lib/types/api';
	import { CertificateType } from "$lib/types/api"
	import { deleteCertificate, getCertificateStatus, uploadCertificate } from '$lib/services/api';
	import { notifications } from '$lib/stores/notifications.svelte.js';
	import { SvelteDate } from "svelte/reactivity";
    import { onMount } from 'svelte';

	interface Props {
		/** Certificate type for the main certificate */
		certType: CertificateType;
		/** Certificate type for the private key */
		keyType: CertificateType;
		/** Certificate type for the CA certificate (optional) */
		caType?: CertificateType;
		/** Mode determines certificate requirements - mqtt (CA required) or https (cert+key required) */
		mode?: 'mqtt' | 'https';
	}

	let {
		certType,
		keyType,
		caType,
		mode = 'https',
	}: Props = $props();

	let certLabel = $derived(mode === 'mqtt' ? 'Client Certificate' : 'Certificate File');
	let keyLabel = $derived(mode === 'mqtt' ? 'Client Private Key' : 'Private Key');
	let caLabel = $derived(mode === 'mqtt' ? 'CA Certificate (Required)' : 'CA Certificate File (Optional)');

	let selectedCertFile: File | null = $state(null);
	let selectedKeyFile: File | null = $state(null);
	let selectedCaFile: File | null = $state(null);
	let isUploading = $state(false);
	let isDeleting = $state(false);
	let certInputRef: HTMLInputElement | null = $state(null);
	let keyInputRef: HTMLInputElement | null = $state(null);
	let caInputRef: HTMLInputElement | null = $state(null);

	let certStatus = $state<CertificatesStatus>();
	let hasCustomCert = $derived(certStatus?.[certType as keyof CertificatesStatus]);
	let certInfo = $derived(certStatus?.[certType as keyof CertificatesStatus] as any);
	let caCertInfo = $derived(certStatus?.[caType as keyof CertificatesStatus] as any);
	let isKeyMatch = $derived((certStatus?.[certType as keyof CertificatesStatus] as any)?.keyMatchesCert ?? false);
	let keyInfo = $derived(certStatus?.[keyType as keyof CertificatesStatus] as any);
	const fetchCertificateStatus = async () => {
		try {
		const response = await getCertificateStatus();
		if (response.success && response.data) {
			certStatus = response.data;
		}
		} catch (e) {
		console.error('Error fetching certificate status:', e);
		}
	};
	onMount(() => {
		fetchCertificateStatus();
	});
	let isExpired = $derived(() => {
		if (!certInfo?.expiration?.to) return false;
		const expiryDate = new Date(certInfo.expiration.to);
		return expiryDate < new Date();
	});

	let isExpiringSoon = $derived(() => {
		if (!certInfo?.expiration?.to) return false;
		const expiryDate = new Date(certInfo.expiration.to);
		const thirtyDaysFromNow = new SvelteDate();
		thirtyDaysFromNow.setDate(thirtyDaysFromNow.getDate() + 30);
		return expiryDate <= thirtyDaysFromNow && expiryDate > new Date();
	});

	let isCaExpired = $derived(() => {
		if (!caCertInfo?.expiration?.to) return false;
		const expiryDate = new Date(caCertInfo.expiration.to);
		return expiryDate < new Date();
	});

	let isCaExpiringSoon = $derived(() => {
		if (!caCertInfo?.expiration?.to) return false;
		const expiryDate = new Date(caCertInfo.expiration.to);
		const thirtyDaysFromNow = new SvelteDate();
		thirtyDaysFromNow.setDate(thirtyDaysFromNow.getDate() + 30);
		return expiryDate <= thirtyDaysFromNow && expiryDate > new Date();
	});

	const VALID_CERT_EXTENSIONS = ['.pem', '.crt', '.cer'];
	const VALID_KEY_EXTENSIONS = ['.pem', '.key'];

	function isValidCertFile(file: File): boolean {
		const ext = '.' + file.name.split('.').pop()?.toLowerCase();
		return VALID_CERT_EXTENSIONS.includes(ext);
	}

	function isValidKeyFile(file: File): boolean {
		const ext = '.' + file.name.split('.').pop()?.toLowerCase();
		return VALID_KEY_EXTENSIONS.includes(ext);
	}

	function handleCertFileChange(event: Event) {
		const input = event.target as HTMLInputElement;
		const file = input.files?.[0];
		if (file && !isValidCertFile(file)) {
			notifications.addWarning('Please select a valid certificate file (.pem, .crt, .cer)');
			input.value = '';
			selectedCertFile = null;
			return;
		}
		selectedCertFile = file || null;
	}

	function handleKeyFileChange(event: Event) {
		const input = event.target as HTMLInputElement;
		const file = input.files?.[0];
		if (file && !isValidKeyFile(file)) {
			notifications.addWarning('Please select a valid private key file (.pem, .key)');
			input.value = '';
			selectedKeyFile = null;
			return;
		}
		selectedKeyFile = file || null;
	}

	function isValidCaFile(file: File): boolean {
		const ext = '.' + file.name.split('.').pop()?.toLowerCase();
		return VALID_CERT_EXTENSIONS.includes(ext);
	}

	function handleCaFileChange(event: Event) {
		const input = event.target as HTMLInputElement;
		const file = input.files?.[0];
		if (file && !isValidCaFile(file)) {
			notifications.addWarning('Please select a valid CA certificate file (.pem, .crt, .cer)');
			input.value = '';
			selectedCaFile = null;
			return;
		}
		selectedCaFile = file || null;
	}

	let uploadProgress = $state<{ [key: string]: number }>({
		cert: 0,
		key: 0,
		ca: 0
	});

	async function handleUpload() {
		if (mode === 'mqtt') {
			if (!selectedCaFile) {
				notifications.addError('Please select a CA certificate file for server verification');
				return;
			}
			if (selectedCertFile && !selectedKeyFile) {
				notifications.addError('Please select a private key file to match the client certificate');
				return;
			}
			if (selectedKeyFile && !selectedCertFile) {
				notifications.addError('Please select a client certificate file to match the private key');
				return;
			}
		} else {
			if (!selectedCertFile || !selectedKeyFile) {
				notifications.addError('Please select both certificate and private key files');
				return;
			}
		}

		isUploading = true;
		uploadProgress.cert = selectedCertFile ? 10 : 0;
		uploadProgress.key = selectedKeyFile ? 10 : 0;
		uploadProgress.ca = selectedCaFile ? 10 : 0;

		try {
			let certResult = { success: true };
			let keyResult = { success: true };
			let caResult = { success: true };

			if (selectedCaFile && caType !== undefined) {
				uploadProgress.ca = 50;
				const caContent = await selectedCaFile.text();
				caResult = await uploadCertificate(caType, caContent);
				uploadProgress.ca = 90;
			}

			if (selectedCertFile) {
				uploadProgress.cert = 50;
				const certContent = await selectedCertFile.text();
				certResult = await uploadCertificate(certType, certContent);
				uploadProgress.cert = 90;
			}

			if (selectedKeyFile) {
				uploadProgress.key = 50;
				const keyContent = await selectedKeyFile.text();
				keyResult = await uploadCertificate(keyType, keyContent);
				uploadProgress.key = 90;
			}

			if (certResult.success && keyResult.success && caResult.success) {
				uploadProgress.cert = 100;
				uploadProgress.key = 100;
				uploadProgress.ca = selectedCaFile && caType !== undefined ? 100 : 0;

				selectedCertFile = null;
				selectedKeyFile = null;
				selectedCaFile = null;
				if (certInputRef) certInputRef.value = '';
				if (keyInputRef) keyInputRef.value = '';
				if (caInputRef) caInputRef.value = '';

				await fetchCertificateStatus();

				setTimeout(() => {
					uploadProgress = { cert: 0, key: 0, ca: 0 };
				}, 1000);
			}
		} catch (error) {
			const message = error instanceof Error ? error.message : String(error);
			notifications.addError(`Upload failed: ${message}`);
		} finally {
			isUploading = false;
		}
	}

	async function handleDelete() {
		if (!confirm('This will delete your custom certificate, private key, and CA certificate (if present). Continue?')) {
			return;
		}

		isDeleting = true;
		try {
			const certResult = await deleteCertificate(certType);
			if (!certResult.success) {
				return;
			}

			const keyResult = await deleteCertificate(keyType);
			if (!keyResult.success) {
				return;
			}

			if (caType !== undefined) {
				await deleteCertificate(caType);
			}

			notifications.addSuccess('Custom certificate deleted.');

			await fetchCertificateStatus();
		} catch (error) {
			const message = error instanceof Error ? error.message : String(error);
			notifications.addError(`Delete failed: ${message}`);
		} finally {
			isDeleting = false;
		}
	}

	function formatDate(dateStr: string): string {
		try {
			return new Date(dateStr).toLocaleDateString();
		} catch {
			return dateStr;
		}
	}
</script>

<div class="space-y-4">
	<!-- Certificate Status Display -->
	<div class="py-3 px-3 bg-base-100 rounded-lg">
		<div class="flex items-center justify-between mb-3">
			<h4 class="text-sm font-medium">Certificates</h4>
			{#if hasCustomCert}
			<button
				type="button"
				class="btn btn-sm btn-outline btn-error"
				disabled={isDeleting}
				onclick={handleDelete}
			>
				{#if isDeleting}
					<span class="loading loading-spinner loading-xs"></span>
				{/if}
				<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor">
					<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M19 7l-.867 12.142A2 2 0 0116.138 21H7.862a2 2 0 01-1.995-1.858L5 7m5 4v6m4-6v6m1-10V4a1 1 0 00-1-1h-4a1 1 0 00-1 1v3M4 7h16" />
				</svg>
			</button>
			{/if}
		</div>

		{#if certInfo}
			<div class="space-y-2 text-xs">
				<div class="flex items-center gap-2">
					<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 text-info" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
						<path stroke-linecap="round" stroke-linejoin="round" d="M9 12l2 2 4-4m5.618-4.016A11.955 11.955 0 0112 2.944a11.955 11.955 0 01-8.618 3.04A12.02 12.02 0 003 9c0 5.591 3.824 10.29 9 11.622 5.176-1.332 9-6.03 9-11.622 0-1.042-.133-2.052-.382-3.016z" />
					</svg>
					<span class="font-medium text-info">{mode === "https" ? "Server" : "Client"} Certificate Present</span>
				</div>
				{#if certInfo.issuer}
					<div class="flex flex-col">
						<span class="text-base-content/60">Issuer</span>
						<span class="font-mono truncate" title={certInfo.issuer}>{certInfo.issuer}</span>
					</div>
				{/if}

				{#if certInfo.subject}
					<div class="flex flex-col">
						<span class="text-base-content/60">Subject</span>
						<span class="font-mono truncate" title={certInfo.subject}>{certInfo.subject}</span>
					</div>
				{/if}

				{#if certInfo.serial}
					<div class="flex flex-col">
						<span class="text-base-content/60">Serial number</span>
						<span class="font-mono break-words" title={certInfo.serial}>{certInfo.serial}</span>
					</div>
				{/if}

				{#if certInfo.fingerprint}
					<div class="flex flex-col">
						<span class="text-base-content/60">SHA256 Fingerprint</span>
						<span class="font-mono break-words" title={certInfo.fingerprint}>{certInfo.fingerprint}</span>
					</div>
				{/if}

				{#if certInfo.expiration?.from && certInfo.expiration?.to}
					<div class="flex flex-col">
						<span class="text-base-content/60">Valid Period</span>
						<span class="font-mono">
							{formatDate(certInfo.expiration.from)} - {formatDate(certInfo.expiration.to)}
						</span>
					</div>
				{/if}

				{#if keyInfo?.keyType}
					<div class="flex flex-col">
						<span class="text-base-content/60">Key Type</span>
						<span class="font-mono truncate">
							{keyInfo.keyType}
						</span>
						{#if keyInfo.keyType === "RSA"}
							<div class="flex items-center justify-between py-2 px-3 bg-warning/10 rounded-lg">
								<div class="flex items-start gap-2">
									<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 text-warning mt-0.5 shrink-0" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
										<path stroke-linecap="round" stroke-linejoin="round" d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z" />
									</svg>
									<div>
										<p class="text-sm font-medium text-warning">Switch to EC if possible</p>
										<p class="text-xs text-base-content/60">RSA is computationally intensive and significantly slows down requests processing times</p>
									</div>
								</div>
							</div>
						{/if}
					</div>
				{/if}

				<!-- Key Match Status -->
				<div class="flex items-center gap-2 mt-2">
					{#if isKeyMatch}
						<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 text-success" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
							<path stroke-linecap="round" stroke-linejoin="round" d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
						</svg>
						<span class="text-success">Private key matches certificate</span>
					{:else if hasCustomCert}
						<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 text-error" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
							<path stroke-linecap="round" stroke-linejoin="round" d="M12 8v4m0 4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
						</svg>
						<span class="text-error">Private key does not match certificate</span>
					{/if}
				</div>
			</div>

			<!-- CA Certificate Section -->
			{#if caCertInfo}
				<div class="divider my-2"></div>
				<div class="space-y-2 text-xs">
					<div class="flex items-center gap-2">
						<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 text-info" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
							<path stroke-linecap="round" stroke-linejoin="round" d="M9 12l2 2 4-4m5.618-4.016A11.955 11.955 0 0112 2.944a11.955 11.955 0 01-8.618 3.04A12.02 12.02 0 003 9c0 5.591 3.824 10.29 9 11.622 5.176-1.332 9-6.03 9-11.622 0-1.042-.133-2.052-.382-3.016z" />
						</svg>
						<span class="font-medium text-info">CA Certificate Present</span>
					</div>
					{#if caCertInfo.issuer}
						<div class="flex flex-col">
							<span class="text-base-content/60">Issuer</span>
							<span class="font-mono truncate" title={caCertInfo.issuer}>{caCertInfo.issuer}</span>
						</div>
					{/if}
					{#if caCertInfo.subject}
						<div class="flex flex-col">
							<span class="text-base-content/60">Subject</span>
							<span class="font-mono truncate" title={caCertInfo.subject}>{caCertInfo.subject}</span>
						</div>
					{/if}
					{#if caCertInfo.serial}
						<div class="flex flex-col">
							<span class="text-base-content/60">Serial number</span>
							<span class="font-mono break-words" title={caCertInfo.serial}>{caCertInfo.serial}</span>
						</div>
					{/if}

					{#if caCertInfo.fingerprint}
						<div class="flex flex-col">
							<span class="text-base-content/60">SHA256 Fingerprint</span>
							<span class="font-mono break-words" title={caCertInfo.fingerprint}>{caCertInfo.fingerprint}</span>
						</div>
					{/if}
					{#if caCertInfo.expiration?.from && caCertInfo.expiration?.to}
						<div class="flex flex-col">
							<span class="text-base-content/60">Valid Period</span>
							<span class="font-mono">
								{formatDate(caCertInfo.expiration.from)} - {formatDate(caCertInfo.expiration.to)}
							</span>
						</div>
					{/if}
				</div>
			{/if}
		{:else}
			<p class="text-xs text-base-content/60 italic">No certificate information available</p>
		{/if}
	</div>

	<!-- Warning Messages -->
	{#if isExpired()}
		<div class="alert alert-error alert-sm py-2">
			<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 shrink-0" fill="none" viewBox="0 0 24 24" stroke="currentColor">
				<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z" />
			</svg>
			<span class="text-xs">Certificate has expired! Please upload a new certificate.</span>
		</div>
	{:else if isExpiringSoon()}
		<div class="alert alert-warning alert-sm py-2">
			<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 shrink-0" fill="none" viewBox="0 0 24 24" stroke="currentColor">
				<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z" />
			</svg>
			<span class="text-xs">Certificate expires soon. Consider renewing.</span>
		</div>
	{/if}

	<!-- CA Warning Messages -->
	{#if isCaExpired()}
		<div class="alert alert-error alert-sm py-2">
			<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 shrink-0" fill="none" viewBox="0 0 24 24" stroke="currentColor">
				<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z" />
			</svg>
			<span class="text-xs">CA certificate has expired! Please upload a new CA certificate.</span>
		</div>
	{:else if isCaExpiringSoon()}
		<div class="alert alert-warning alert-sm py-2">
			<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 shrink-0" fill="none" viewBox="0 0 24 24" stroke="currentColor">
				<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z" />
			</svg>
			<span class="text-xs">CA certificate expires soon. Consider renewing.</span>
		</div>
	{/if}

	<!-- File Upload Section -->
	<div class="space-y-3 py-2 px-3 bg-base-100 rounded-lg">
		<h4 class="text-sm font-medium">Upload Certificate</h4>
		<!-- Help Text -->
		<div class="border-l-4 border-info bg-gradient-to-r from-info/10 to-transparent rounded-r-lg p-3 shadow-sm">
			<div class="flex items-start gap-2">
				<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 text-info mt-0.5 shrink-0" fill="none" viewBox="0 0 24 24" stroke="currentColor" stroke-width="2">
					<path stroke-linecap="round" stroke-linejoin="round" d="M13 16h-1v-4h-1m1-4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
				</svg>
				<div class="space-y-1.5">
					<p class="text-xs font-semibold text-base-content">Certificate Requirements</p>
					<ul class="text-xs text-base-content/75 list-disc list-inside space-y-0.5">
						<li>Must be in PEM format</li>
						<li>Supported keys: RSA (2048+ bits) and SECP256R1 (P-256)</li>
						{#if mode === 'mqtt'}
							<li><strong>CA certificate</strong> is required for verifying the MQTT broker's identity</li>
							<li><strong>Client certificate + key</strong> are only needed for mutual TLS (mTLS) authentication</li>
						{:else}
							<li><strong>Server certificate + private key</strong> are required for HTTPS operation</li>
							<li><strong>CA certificate</strong> is only needed if requiring client certificate validation (mTLS)</li>
						{/if}
					</ul>
				</div>
			</div>
		</div>
		<div class="grid grid-cols-1 md:grid-cols-2 gap-3">
			<!-- Certificate File Input -->
			<div class="form-control">
				<label class="label" for="cert-file-input">
					<span class="label-text text-xs">{certLabel} (.pem, .crt, .cer){mode === 'mqtt' ? ' - Optional' : ''}</span>
				</label>
				<input
					id="cert-file-input"
					type="file"
					accept={VALID_CERT_EXTENSIONS.join(",")}
					onchange={handleCertFileChange}
					bind:this={certInputRef}
					class="file-input file-input-sm file-input-bordered w-full text-xs"
					disabled={isUploading}
				/>
				{#if selectedCertFile}
					<p class="text-xs text-success mt-1">Selected: {selectedCertFile.name}</p>
				{/if}
				{#if uploadProgress.cert > 0}
					<progress class="progress progress-primary w-full mt-2" value={uploadProgress.cert} max="100"></progress>
				{/if}
			</div>

			<!-- Private Key File Input -->
			<div class="form-control">
				<label class="label" for="key-file-input">
					<span class="label-text text-xs">{keyLabel} (.pem, .key){mode === 'mqtt' ? ' - Optional' : ''}</span>
				</label>
				<input
					id="key-file-input"
					type="file"
					accept={VALID_KEY_EXTENSIONS.join(",")}
					onchange={handleKeyFileChange}
					bind:this={keyInputRef}
					class="file-input file-input-sm file-input-bordered w-full text-xs"
					disabled={isUploading}
				/>
				{#if selectedKeyFile}
					<p class="text-xs text-success mt-1">Selected: {selectedKeyFile.name}</p>
				{/if}
				{#if uploadProgress.key > 0}
					<progress class="progress progress-primary w-full mt-2" value={uploadProgress.key} max="100"></progress>
				{/if}
			</div>
		</div>

		<!-- CA Certificate File Input - Full width for both modes -->
		<div class="form-control mt-3">
			<label class="label" for="ca-file-input">
				<span class="label-text text-xs">{caLabel} (.pem, .crt, .cer)</span>
			</label>
			<input
				id="ca-file-input"
				type="file"
				accept={VALID_CERT_EXTENSIONS.join(",")}
				onchange={handleCaFileChange}
				bind:this={caInputRef}
				class="file-input file-input-sm file-input-bordered w-full text-xs"
				disabled={isUploading}
			/>
			{#if selectedCaFile}
				<p class="text-xs text-success mt-1">Selected: {selectedCaFile.name}</p>
			{/if}
			{#if uploadProgress.ca > 0}
				<progress class="progress progress-primary w-full mt-2" value={uploadProgress.ca} max="100"></progress>
			{/if}
		</div>

		<!-- Upload Button -->
		<button
			type="button"
			class="btn btn-sm btn-primary"
			disabled={isUploading || (mode === 'mqtt' ? !selectedCaFile : (!selectedCertFile || !selectedKeyFile))}
			onclick={handleUpload}
		>
			{#if isUploading}
				<span class="loading loading-spinner loading-xs"></span>
				Uploading...
			{:else}
				<svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4 mr-1" fill="none" viewBox="0 0 24 24" stroke="currentColor">
					<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 16v1a3 3 0 003 3h10a3 3 0 003-3v-1m-4-8l-4-4m0 0L8 8m4-4v12" />
				</svg>
				Upload
			{/if}
		</button>
	</div>
</div>
