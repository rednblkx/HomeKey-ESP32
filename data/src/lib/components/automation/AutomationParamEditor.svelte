<script lang="ts">
  import type { AutomationParamDesc } from '$lib/types/api';

  let {
    desc,
    value = $bindable(''),
    onchange,
  }: {
    desc: AutomationParamDesc;
    value?: string | number | boolean;
    onchange?: (v: string | number | boolean) => void;
  } = $props();

  function report(v: string | number | boolean) {
    value = v;
    onchange?.(v);
  }
</script>

<label class="form-control">
  <div class="label py-0.5"><span class="label-text text-xs">{desc.name}</span></div>
  {#if desc.kind === 'select'}
    <select class="select select-bordered select-sm" value={value as string}
      onchange={(e) => report(e.currentTarget.value)}>
      {#each desc.options ?? [] as opt}
        <option value={opt}>{opt}</option>
      {/each}
    </select>
  {:else if desc.kind === 'bool'}
    <input type="checkbox" class="toggle toggle-sm" checked={value === true || value === 'true'}
      onchange={(e) => report(e.currentTarget.checked)} />
  {:else if desc.kind === 'number'}
    <input type="number" class="input input-bordered input-sm" value={value as number}
      onchange={(e) => report(e.currentTarget.valueAsNumber)} />
  {:else}
    <input type="text" class="input input-bordered input-sm" value={value as string}
      onchange={(e) => report(e.currentTarget.value)} />
  {/if}
</label>
