<script lang="ts">
  import type {
    AutomationRule,
    AutomationSchema,
    AutomationUnitDesc,
  } from '$lib/types/api';
  import AutomationUnitEditor from './AutomationUnitEditor.svelte';

  let {
    rule = $bindable(),
    schema,
    ondelete,
  }: {
    rule: AutomationRule;
    schema: AutomationSchema;
    ondelete?: () => void;
  } = $props();

  const triggers: AutomationUnitDesc[] = $derived(schema?.triggers ?? []);
  const conditions: AutomationUnitDesc[] = $derived(schema?.conditions ?? []);
  const actions: AutomationUnitDesc[] = $derived(schema?.actions ?? []);

  function addCondition() {
    rule.conditions = [...rule.conditions, { type: '', params: {} }];
  }
  function addAction() {
    rule.actions = [...rule.actions, { type: '', params: {} }];
  }
</script>

<div class="card bg-base-100 border border-base-300 shadow-sm">
  <div class="card-body p-4 gap-3">
    <div class="flex items-center gap-3">
      <input
        type="checkbox"
        class="toggle toggle-primary"
        checked={rule.enabled}
        onchange={(e) => (rule.enabled = e.currentTarget.checked)}
      />
      <input
        type="text"
        class="input input-ghost input-sm font-semibold flex-1"
        bind:value={rule.name}
        placeholder="Rule name"
      />
      <button type="button" class="btn btn-ghost btn-sm text-error" onclick={ondelete}>✕</button>
    </div>

    <AutomationUnitEditor
      units={triggers}
      label="When"
      bind:selected={rule.trigger.type}
      bind:params={rule.trigger.filter}
    />

    {#each rule.conditions as _cond, i}
      <AutomationUnitEditor
        units={conditions}
        label="And if"
        bind:selected={rule.conditions[i].type}
        bind:params={rule.conditions[i].params}
      />
      <button
        type="button"
        class="btn btn-ghost btn-xs self-end text-error"
        onclick={() => rule.conditions.splice(i, 1)}>remove condition</button>
    {/each}
    <button type="button" class="btn btn-outline btn-xs self-start" onclick={addCondition}>
      + condition
    </button>

    {#each rule.actions as _act, i}
      <AutomationUnitEditor
        units={actions}
        label="Then"
        bind:selected={rule.actions[i].type}
        bind:params={rule.actions[i].params}
      />
      <button
        type="button"
        class="btn btn-ghost btn-xs self-end text-error"
        onclick={() => rule.actions.splice(i, 1)}>remove action</button>
    {/each}
    <button type="button" class="btn btn-outline btn-xs self-start" onclick={addAction}>
      + action
    </button>
  </div>
</div>
