import type { Hooks } from 'sv-router';
import type { ActionsConfig } from '$lib/types/api';

declare module 'sv-router' {
  interface RouteMeta {
    actionsData?: { actions: ActionsConfig | null; error: string | null };
  }
}

export default {
  async beforeLoad({ meta }) {
    try {
      const res = await fetch('/config?type=actions').then(r => r.json());
      if (!res.success) throw new Error(res.error);
      meta.actionsData = { actions: res.data as ActionsConfig, error: null };
    } catch (error) {
      console.error('Failed to load actions:', error);
      meta.actionsData = { actions: null, error: error instanceof Error ? error.message : 'Unknown error' };
    }
  },
} satisfies Hooks;
