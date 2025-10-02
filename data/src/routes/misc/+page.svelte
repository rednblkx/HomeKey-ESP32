<script lang="ts">
	import AppMisc from '$lib/components/AppMisc.svelte';

	let { data } = $props();
</script>

<AppMisc misc={data.misc} eth={data.eth} error={data.error} />