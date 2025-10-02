<script>
	import '../app.css';
	import { onMount, onDestroy } from 'svelte';
	import { updateSystemInfo } from '$lib/stores/system.svelte.js';
	import { getCurrentTheme, toggleTheme, initTheme, logoSrc } from '$lib/stores/theme.svelte.js';
	import { uiState, closeDrawer } from '$lib/stores/ui.svelte.js';
	import { setLoadingState } from '$lib/stores/loading.svelte.js';
	import ws from '$lib/services/ws.js';
	import Logo from '$lib/assets/favicon.png';
	import NavigationMenu from '$lib/components/NavigationMenu.svelte';
	import Notification from '$lib/components/Notification.svelte';
    import { websocketState } from '$lib/stores/websocket.svelte';

	let { children } = $props();

	/** @type {HTMLDivElement | null} */
	let sidebarTrigger = $state(null);
	/** @type {HTMLDivElement | null} */
	let mobileTrigger = $state(null);
	// WebSocket message subscription
	/**
	     * @type {() => void}
	     */
	let unsubscribeMessages;

	onMount(() => {
    	initTheme();

		// Initialize WebSocket
		if (ws) {
			ws.connect();

			// Add message listener to update stores
			unsubscribeMessages = ws.on((/** @type {{ type: string; data: any; }} */ event) => {
				if (event.type === 'message') {
					const data = event.data;
					if (data.type === 'sysinfo' || data.type === 'metrics') {
						updateSystemInfo(data);
						// Clear system info loading state when data is received
						setLoadingState('systemInfoLoading', false);
					}
				}
			    	});
		}
	});

	onDestroy(() => {
		if (unsubscribeMessages) {
			unsubscribeMessages();
		}
	});
</script>

<svelte:head>
	<link rel="icon" href={logoSrc()} type="image/svg+xml" sizes="any" />
	<link rel="shortcut icon" href={logoSrc()} type="image/svg+xml" sizes="any" />
	<link rel="apple-touch-icon" href={logoSrc()} type="image/svg+xml" sizes="any" />
	<link rel="mask-icon" href={logoSrc()} color="#000000" sizes="any" />
	<link rel="shortcut icon" href={Logo} type="image/png" />
	<link rel="apple-touch-icon" href={Logo} type="image/png" />
	<title>HomeKey-ESP32</title>
</svelte:head>

<div class="flex flex-col h-dvh">
	<!-- Mobile Navbar -->
	<div class="navbar bg-base-100 lg:hidden sticky top-[0] z-[9999]">
		<div class="navbar-start">
			<label for="main-content-drawer" class="btn btn-ghost drawer-button lg:hidden">
				<svg xmlns="http://www.w3.org/2000/svg" class="h-5 w-5" fill="none" viewBox="0 0 24 24"
					stroke="currentColor">
					<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 6h16M4 12h8m-8 6h16" />
				</svg>
			</label>
		</div>
		<div class="navbar-center">
			<img src={logoSrc()} alt="logo" class="w-8 h-8 mr-2" />
			<div class="flex items-center flex-col">
				<span class="font-bold text-lg">HomeKey-ESP32</span>
				<div class="flex items-center gap-1">
					<div class="inline-grid *:[grid-area:1/1]">
						<div class="status animate-ping" class:status-success={websocketState.connected} class:status-error={!websocketState.connected}></div>
						<div class="status" class:status-success={websocketState.connected} class:status-error={!websocketState.connected}></div>
					</div>
					<span class="text-sm" class:text-success={websocketState.connected} class:text-error={!websocketState.connected}>{websocketState.connected ? 'Online' : 'Offline'}</span>
				</div>
			</div>
		</div>
		<div class="navbar-end">
			<label class="swap swap-rotate mr-4">
				<input type="checkbox" onclick={toggleTheme} checked={getCurrentTheme() !== 'dracula'} />
				<svg class="swap-on w-6 h-6" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24"
					stroke="currentColor" stroke-width="2">
					<path stroke-linecap="round" stroke-linejoin="round"
						d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z" />
				</svg>
				<svg class="swap-off w-6 h-6" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24"
					stroke="currentColor" stroke-width="2">
					<path stroke-linecap="round" stroke-linejoin="round"
						d="M12 3v1m0 16v1m9-9h-1M4 12H3m15.364 6.364l-.707-.707M6.343 6.343l-.707-.707m12.728 0l-.707.707M6.343 17.657l-.707.707M16 12a4 4 0 11-8 0 4 4 0 018 0z" />
				</svg>
			</label>
		</div>
	</div>
	
	<!-- Drawer -->
	<div class="drawer lg:drawer-open flex w-full flex-1 overflow-hidden">
		<input id="main-content-drawer" type="checkbox" class="drawer-toggle" bind:checked={uiState.drawerOpen} />
		<!-- Sidebar -->
		<div class="drawer-side max-lg:top-auto">
			<label for="main-content-drawer" aria-label="close sidebar" class="drawer-overlay"></label>
			<NavigationMenu onClose={closeDrawer} isMobile={false} bind:triggerElement={sidebarTrigger} id="main-navigation" />
		</div>
		<div class="drawer-content flex flex-col h-full">
			<!-- Content -->
			<main id="main-content" class="px-6 w-full h-full flex-1">
				{@render children()}
			</main>
		</div>
	
	</div>
</div>
<!-- Global Notifications -->
<Notification />