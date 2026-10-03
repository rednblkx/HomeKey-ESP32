<script lang="ts">
  import { saveConfig } from '$lib/services/api';
  import type { AutomationConfig, AutomationRule, AutomationSchema, AutomationUnitDesc } from '$lib/types/api';
  import AutomationRuleEditor from './automation/AutomationRuleEditor.svelte';

  let {
    config,
    schema,
    error,
  }: {
    config: AutomationConfig | undefined;
    schema: AutomationSchema | null;
    error?: string | null;
  } = $props();

  // svelte-ignore state_referenced_locally
  let rules = $state<AutomationRule[]>(normalizeRules(config?.rules ?? []));
  let saving = $state(false);

  function normalizeRules(list: AutomationRule[]): AutomationRule[] {
    return list.map((r) => ({
      ...r,
      trigger: { type: r.trigger?.type ?? '', filter: r.trigger?.filter ?? {} },
      conditions: (r.conditions ?? []).map((c) => ({ type: c.type ?? '', params: c.params ?? {} })),
      actions: (r.actions ?? []).map((a) => ({ type: a.type ?? '', params: a.params ?? {} })),
    }));
  }

  function addRule() {
    rules.push({
      id: `r${Date.now().toString(36)}`,
      name: 'New rule',
      enabled: true,
      trigger: { type: '', filter: {} },
      conditions: [],
      actions: [],
    });
  }

  function pruneParams(
    list: { type: string; params?: Record<string, string | number | boolean> }[],
    units: AutomationUnitDesc[] | undefined,
  ) {
    for (const item of list) {
      const desc = units?.find((u) => u.type === item.type);
      if (!desc) continue;
      const keep = desc.params.filter(
        (p) => !p.showIfParam || item.params?.[p.showIfParam] === p.showIfValue,
      );
      item.params = Object.fromEntries(
        Object.entries(item.params ?? {}).filter(([k]) => keep.some((p) => p.name === k)),
      );
    }
  }

  function serializeRules(): string {
    const snapshot: AutomationRule[] = $state.snapshot(rules).map((r) => ({
      ...r,
      trigger: { ...r.trigger, filter: { ...(r.trigger.filter ?? {}) } },
      conditions: r.conditions.map((c) => ({ ...c, params: { ...(c.params ?? {}) } })),
      actions: r.actions.map((a) => ({ ...a, params: { ...(a.params ?? {}) } })),
    }));
    pruneParams(
      snapshot.flatMap((r) => [r.trigger, ...r.conditions, ...r.actions]),
      schema?.triggers,
    );
    return JSON.stringify({ rules: snapshot });
  }

  async function save() {
    saving = true;
    try {
      const result = await saveConfig('automation', { rulesJson: serializeRules() });
      if (result.success && result.data) {
        const parsed = JSON.parse((result.data as { rulesJson?: string }).rulesJson ?? '{"rules":[]}');
        rules = normalizeRules(parsed.rules ?? []);
      }
    } catch (e) {
      alert(`Error saving automations: ${e instanceof Error ? e.message : String(e)}`);
    } finally {
      saving = false;
    }
  }

  function removeRule(index: number) {
    rules.splice(index, 1);
  }

  const incomplete = $derived(
    rules.some((r) => !r.trigger.type || r.actions.some((a) => !a.type) || r.conditions.some((c) => !c.type)),
  );
</script>

<div class="w-full py-6">
  <div class="mb-6">
    <h1 class="text-2xl font-bold text-base-content">Automations</h1>
    <p class="text-sm text-base-content/60">
      Advanced trigger–condition–action rules. The simple fixed triggers live on the
      <a class="link link-primary" href="/actions">Actions</a> page.
    </p>
  </div>

  {#if !config && error}
    <div class="text-center text-error"><p>Error: {error}</p></div>
  {:else}
    <div class="max-w-5xl space-y-3">
      {#each rules as _rule, i (rules[i].id)}
        <AutomationRuleEditor bind:rule={rules[i]} schema={schema ?? { triggers: [], conditions: [], actions: [] }} ondelete={() => removeRule(i)} />
      {/each}

      <div class="flex gap-2">
        <button type="button" class="btn btn-outline btn-sm" onclick={addRule}>+ Add rule</button>
        <button type="button" class="btn btn-primary btn-sm" onclick={save} disabled={saving || incomplete}>
          {saving ? 'Saving…' : 'Save'}
        </button>
      </div>
      {#if incomplete}
        <p class="text-xs text-warning">Some rules are missing a trigger or action and won't be saved until completed.</p>
      {/if}
    </div>
  {/if}
</div>
