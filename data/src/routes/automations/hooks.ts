import type { Hooks } from 'sv-router';
import type { AutomationConfig, AutomationSchema } from '$lib/types/api';
import { setLoadingState } from '$lib/stores/system.svelte';

declare module 'sv-router' {
  interface RouteMeta {
    automationsData?: {
      config: AutomationConfig | null;
      schema: AutomationSchema | null;
      error: string | null;
    };
  }
}

export default {
  async beforeLoad({ meta }) {
    try {
      setLoadingState(true);
      const [configRes, schemaRes] = await Promise.all([
        fetch('/config?type=automation').then(r => r.json()),
        fetch('/automation/schema').then(r => r.json()),
      ]);
      if (!configRes.success) throw new Error(configRes.error);
      const raw = configRes.data as { rulesJson?: string } | null;
      const config: AutomationConfig = raw?.rulesJson
        ? JSON.parse(raw.rulesJson)
        : { rules: [] };
      const schema = schemaRes.success ? (schemaRes.data as AutomationSchema) : null;
      meta.automationsData = {
        config,
        schema,
        error: schema ? null : 'Failed to load automation schema',
      };
    } catch (error) {
      console.error('Failed to load automations:', error);
      meta.automationsData = {
        config: null,
        schema: null,
        error: error instanceof Error ? error.message : 'Unknown error',
      };
    } finally {
      setLoadingState(false);
    }
  },
} satisfies Hooks;
