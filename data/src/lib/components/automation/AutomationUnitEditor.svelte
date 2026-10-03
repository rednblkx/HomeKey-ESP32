<script lang="ts">
  import type {
    AutomationUnitDesc,
    AutomationParamDesc,
  } from '$lib/types/api';
  import AutomationParamEditor from './AutomationParamEditor.svelte';

  /**
   * Generic editor for one trigger / condition / action unit. The unit's
   * descriptor comes from the device schema endpoint; params are edited as a
   * flat Record<string, string|number|boolean> that is serialized verbatim
   * into the rule JSON blob. Params with a `showIfParam`/`showIfValue`
   * dependency are hidden while the dependency doesn't hold, and their stale
   * values are dropped from the saved blob.
   */
  let {
    units,
    selected = $bindable(''),
    params = $bindable({}),
    label,
  }: {
    units: AutomationUnitDesc[];
    selected?: string;
    params?: Record<string, string | number | boolean>;
    label: string;
  } = $props();

  const unit = $derived(units.find((u) => u.type === selected));

  function visible(p: AutomationParamDesc): boolean {
    if (!p.showIfParam) return true;
    return String(params[p.showIfParam] ?? '') === p.showIfValue;
  }

  function onPick(e: Event) {
    const next = (e.currentTarget as HTMLSelectElement).value;
    selected = next;
    params = {};
    const u = units.find((x) => x.type === next);
    for (const p of u?.params ?? []) {
      if (p.name) {
        params[p.name] = p.kind === 'select' ? (p.options?.[0] ?? '') : p.kind === 'bool' ? false : p.kind === 'number' ? 0 : '';
      }
    }
  }

  function setParam(name: string, v: string | number | boolean) {
    params = { ...params, [name]: v };
  }
</script>

<div class="border border-base-300 rounded-xl p-3 space-y-2 bg-base-200/40">
  <div class="text-xs font-semibold uppercase tracking-wide text-base-content/60">{label}</div>
  <select class="select select-bordered select-sm w-full" value={selected} onchange={onPick}>
    <option value="" disabled>Select…</option>
    {#each units as u (u.type)}
      <option value={u.type}>{u.label}</option>
    {/each}
  </select>
  {#if unit && unit.params.some((p) => visible(p))}
    <div class="grid grid-cols-2 gap-2">
      {#each unit.params as p (p.name)}
        {#if visible(p)}
          <AutomationParamEditor desc={p} value={params[p.name] ?? ''} onchange={(v: string | number | boolean) => setParam(p.name, v)} />
        {/if}
      {/each}
    </div>
  {/if}
</div>
