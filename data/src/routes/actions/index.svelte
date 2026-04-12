<script lang="ts">
	import AppActions from '$lib/components/AppActions.svelte';
	import { route } from 'sv-router/generated';
</script>

<AppActions actions={route.meta.actionsData?.actions} error={route.meta.actionsData?.error} />