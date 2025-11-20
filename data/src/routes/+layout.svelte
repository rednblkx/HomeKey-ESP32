<script lang="ts">
	import '@/app.css';
	import { onMount, onDestroy } from 'svelte';
	import { updateSystemInfo } from '$lib/stores/system.svelte.js';
	import { getCurrentTheme, toggleTheme, initTheme, logoSrc } from '$lib/stores/theme.svelte.js';
	import { uiState, closeDrawer } from '$lib/stores/ui.svelte.js';
	import { setLoadingState } from '$lib/stores/loading.svelte.js';
	import ws, { type WebSocketEvent } from '$lib/services/ws.js';
	import Logo from '$lib/assets/favicon.png';
	import NavigationMenu from '$lib/components/NavigationMenu.svelte';
	import Notification from '$lib/components/Notification.svelte';
  import { websocketState } from '$lib/stores/websocket.svelte';
  import { navigating } from '$app/state';
  import type { LogEntry } from '$lib/types/api';
  import { logIdIncrement, logs } from '$lib/stores/logs.svelte';

	let { children } = $props();

	let sidebarTrigger = $state(null);
	let unsubscribeMessages : (() => void) | null = null;

	onMount(() => {
    initTheme();

		if (ws) {
			ws.connect();

			unsubscribeMessages = ws.on((event : WebSocketEvent<any>) => {
				if (event.type === 'message') {
					const data = event.data;
					if (data.type === 'sysinfo' || data.type === 'metrics') {
						updateSystemInfo(data);
						setLoadingState('systemInfoLoading', false);
					}
          if (data.type === 'log') {
						const log : LogEntry = {
							id: Date.now() + logIdIncrement(),
							localts: new Date().toISOString(),
							expanded: false,
							...data
						};

						if (log.msg && log.level) {
							logs.push(log);
						} else {
							console.warn('Invalid log message structure:', data);
						}
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
	<link rel="icon" href={logoSrc()} type="image/svg+xml" />
	<link rel="apple-touch-icon" href={Logo} />
	<title>HomeKey-ESP32</title>
</svelte:head>

<div class="flex flex-col h-dvh">
	<!-- Mobile Navbar -->
	<div class="navbar bg-base-100 lg:hidden sticky top-[0] z-[9999]">
		<div class="navbar-start">
			<label for="main-content-drawer" class="btn btn-ghost drawer-button lg:hidden" aria-label="Menu">
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
						<div class="status animate-ping" class:status-success={websocketState.connected} class:status-error={!websocketState.connected} class:status-warning={websocketState.state == "reconnecting"}></div>
						<div class="status" class:status-success={websocketState.connected} class:status-error={!websocketState.connected} class:status-warning={websocketState.state == "reconnecting"}></div>
					</div>
					<span class="text-sm" class:text-success={websocketState.connected} class:text-error={!websocketState.connected} class:text-warning={websocketState.state == "reconnecting"}>{websocketState.state == "open" ? 'Online' : websocketState.state == "reconnecting" ? "Reconnecting" : 'Offline'}</span>
				</div>
			</div>
		</div>
		<div class="navbar-end">
			<label class="swap swap-rotate mr-4">
				<input type="checkbox" onclick={toggleTheme} checked={getCurrentTheme() !== 'dracula'} aria-label="Toggle theme" />
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
			<NavigationMenu onClose={closeDrawer} bind:triggerElement={sidebarTrigger} id="main-navigation" />
		</div>
		<div class="drawer-content h-full w-full">
			<!-- Content -->
			<main id="main-content" class="px-6 h-full overflow-y-auto">
        {#await navigating.complete}
          <div class="flex justify-center items-center min-h-screen" aria-live="polite" aria-label="Loading page">
            <div class="text-center">
              <span class="loading loading-spinner loading-lg" aria-label="Loading page content"></span>
              <p class="mt-4 text-lg">Loading...</p>
            </div>
          </div>
        {:then}
          {@render children()}
        {:catch error}
          <div class="card bg-base-200 shadow-xl" aria-live="assertive" aria-label="Error loading WebSocket Test component">
            <div class="card-body p-4">
              <div class="text-center text-error">
                <p>Failed to load WebSocket Test component: {error.message || 'Unknown error'}</p>
              </div>
            </div>
          </div>
        {/await}
			</main>
		</div>
	
	</div>
</div>
<!-- Global Notifications -->
<Notification />
