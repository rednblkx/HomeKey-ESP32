import type { Hooks } from 'sv-router';
import type { MiscConfig, EthConfig, NfcGpioPinsPreset } from '$lib/types/api';

declare module 'sv-router' {
  interface RouteMeta {
    miscData?: { misc: MiscConfig | null; eth: EthConfig | null; nfcPresets: NfcGpioPinsPreset | null; error: string | null };
  }
}

export default {
  async beforeLoad({ meta }) {
    try {
      const [miscRes, ethRes, nfcRes] = await Promise.all([
        fetch('/config?type=misc').then(r => r.json()),
        fetch('/eth_get_config').then(r => r.json()),
        fetch('/nfc_get_presets').then(r => r.json()),
      ]);
      if (!miscRes.success || !ethRes.success || !nfcRes.success) {
        throw new Error(miscRes.error || ethRes.error || nfcRes.error);
      }
      meta.miscData = {
        misc: miscRes.data as MiscConfig,
        eth: ethRes.data as EthConfig,
        nfcPresets: nfcRes.data as NfcGpioPinsPreset,
        error: null,
      };
    } catch (error) {
      console.error('Failed to load misc config:', error);
      meta.miscData = { misc: null, eth: null, nfcPresets: null, error: error instanceof Error ? error.message : 'Unknown error' };
    }
  },
} satisfies Hooks;
