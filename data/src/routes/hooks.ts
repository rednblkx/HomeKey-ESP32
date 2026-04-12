import type { Hooks } from 'sv-router';
import type { HKInfo } from '$lib/types/api';

declare module 'sv-router' {
  interface RouteMeta {
    homeData?: { hkInfo: HKInfo | null; error: string | null };
  }
}

export default {
  async beforeLoad({ meta }) {
    try {
      const res = await fetch('/config?type=hkinfo').then(r => r.json());
      if (!res.success) throw new Error(res.error);
      meta.homeData = { hkInfo: res.data as HKInfo, error: null };
    } catch (error) {
      console.error('Failed to load hkinfo:', error);
      meta.homeData = { hkInfo: null, error: error instanceof Error ? error.message : 'Unknown error' };
    }
  },
} satisfies Hooks;
