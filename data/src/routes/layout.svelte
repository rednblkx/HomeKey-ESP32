<script lang="ts">
	import '@/app.css';
	import { onMount, onDestroy } from 'svelte';
	import { getLoadingState, updateSystemInfo } from '$lib/stores/system.svelte.js';
	import { getCurrentTheme, toggleTheme, initTheme, logoSrc } from '$lib/stores/theme.svelte.js';
	import ws, { type WebSocketEvent } from '$lib/services/ws.js';
	import Logo from '$lib/assets/favicon.png';
	import NavigationMenu from '$lib/components/NavigationMenu.svelte';
	import Notification from '$lib/components/Notification.svelte';
  import { websocketState } from '$lib/stores/websocket.svelte';
  import type { LogEntry } from '$lib/types/api';
  import { logIdIncrement, logs } from '$lib/stores/logs.svelte';

	let { children } = $props();
  let drawerOpen = $state(false);
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
	<div class="navbar bg-base-100 lg:hidden sticky top-0 z-9999">
		<div class="navbar-start w-full">
			<label for="main-content-drawer" class="btn btn-ghost drawer-button lg:hidden" aria-label="Menu">
				<svg xmlns="http://www.w3.org/2000/svg" class="h-5 w-5" fill="none" viewBox="0 0 24 24"
					stroke="currentColor">
					<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 6h16M4 12h8m-8 6h16" />
				</svg>
			</label>
			<div class="flex justify-between w-full">
				<span class="font-bold text-lg">HomeKey-ESP32</span>
				<div class="gap-1 pr-2">
					<div class="inline-grid *:[grid-area:1/1]">
						<div class="status animate-ping" class:status-success={websocketState.connected} class:status-error={!websocketState.connected} class:status-warning={websocketState.state == "reconnecting"}></div>
						<div class="status" class:status-success={websocketState.connected} class:status-error={!websocketState.connected} class:status-warning={websocketState.state == "reconnecting"}></div>
					</div>
					<span class="text-sm" class:text-success={websocketState.connected} class:text-error={!websocketState.connected} class:text-warning={websocketState.state == "reconnecting"}>{websocketState.state == "open" ? 'Online' : websocketState.state == "reconnecting" ? "Reconnecting" : 'Offline'}</span>
				</div>
			</div>
		</div>
	</div>
	
	<!-- Drawer -->
	<div class="drawer lg:drawer-open flex w-full flex-1 overflow-hidden">
		<input id="main-content-drawer" type="checkbox" class="drawer-toggle" bind:checked={drawerOpen} />
		<!-- Sidebar -->
		<div class="drawer-side max-lg:fixed max-lg:top-16 max-lg:bottom-0 max-lg:left-0 max-lg:z-50 max-lg:h-[calc(100dvh-4rem)] h-screen flex flex-col overflow-hidden">
			<label for="main-content-drawer" aria-label="close sidebar" class="drawer-overlay w-full h-full absolute"></label>
			<NavigationMenu onClose={() => drawerOpen = false} id="main-navigation" />
		</div>
		<div class="drawer-content h-full w-full">
			<!-- Content -->
			<main id="main-content" class="px-6 h-full overflow-y-auto bg-base-100">
			{#if getLoadingState()}
				<div class="flex items-center justify-center h-full">
					<div class="loading loading-spinner loading-lg"></div>
				</div>
			{:else}
				{@render children()}
			{/if}
			</main>
		</div>
	
	</div>
</div>
<!-- Global Notifications -->
<Notification />
