<script lang="ts">
	import AppActions from '$lib/components/AppActions.svelte';

	let { data } = $props();
</script>

<AppActions actions={data.actions} error={data.error} />