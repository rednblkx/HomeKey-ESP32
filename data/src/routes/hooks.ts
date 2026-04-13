import type { Hooks } from 'sv-router';
import type { HKInfo } from '$lib/types/api';
import { setLoadingState } from '$lib/stores/system.svelte';

declare module 'sv-router' {
  interface RouteMeta {
    homeData?: { hkInfo: HKInfo | null; error: string | null };
  }
}

export default {
  async beforeLoad({ meta, pathname }) {
    try {
      if (pathname !== '/') {
        return;
      }
      setLoadingState(true);
      const res = await fetch('/config?type=hkinfo').then(r => r.json());
      if (!res.success) throw new Error(res.error);
      meta.homeData = { hkInfo: res.data as HKInfo, error: null };
    } catch (error) {
      console.error('Failed to load hkinfo:', error);
      meta.homeData = { hkInfo: null, error: error instanceof Error ? error.message : 'Unknown error' };
    } finally {
      setLoadingState(false);
    }
  },
} satisfies Hooks;
