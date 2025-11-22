import type { PageLoad } from './$types';
import type { HKInfo } from '$lib/types/api';
import { setLoadingState } from '$lib/stores/loading.svelte.js';

export const ssr = false;

export const load: PageLoad = async ({ fetch }) => {
  try {
    // Set page loading state
    setLoadingState('pageLoading', true);
    setLoadingState('hkInfoLoading', true);

    const hkInfo = await fetch('/config?type=hkinfo');
    const res = await hkInfo.json();
    if (!res.success) {
      throw new Error(res.error);
    }

    const result = { hkInfo: res.data as HKInfo, error: null };

    // Clear loading states on success
    setLoadingState('pageLoading', false);
    setLoadingState('hkInfoLoading', false);

    return result;
  } catch (error) {
    console.error('Failed to load hkinfo:', error);

    const result = {
      hkInfo: null,
      error: error instanceof Error ? error.message : 'Unknown error'
    };

    // Clear loading states on error
    setLoadingState('pageLoading', false);
    setLoadingState('hkInfoLoading', false);

    return result;
  }
}
