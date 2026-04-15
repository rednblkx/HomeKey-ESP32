import type { Hooks } from 'sv-router';
import type { CaptivePortalConfig, EthConfig, NfcGpioPinsPreset } from '$lib/types/api';
import { setLoadingState } from '$lib/stores/system.svelte';

declare module 'sv-router' {
  interface RouteMeta {
    captivePortalData?: {
      config: CaptivePortalConfig | null;
      nfcPresets: NfcGpioPinsPreset | null;
      ethConfig: EthConfig | null;
      error: string | null;
    };
  }
}

export default {
  async beforeLoad({ meta }) {
    try {
      setLoadingState(true);

      // Fetch all config data in parallel
      const [configRes, nfcRes, ethRes] = await Promise.all([
        fetch('/captive_portal_config').then(r => r.json()),
        fetch('/nfc_get_presets').then(r => r.json()),
        fetch('/eth_get_config').then(r => r.json()),
      ]);

      if (!configRes.success) throw new Error(configRes.error);

      const data = configRes.data;

      meta.captivePortalData = {
        config: {
          wifiSsid: '',
          wifiPassword: '',
          setupCode: data.setupCode || '46637726',
          hk_key_color: data.hk_key_color ?? 0,
          nfcPinsPreset: data.nfcPinsPreset ?? 0,
          nfcGpioPins: data.nfcGpioPins ?? [5, 18, 19, 23],
          ethernetEnabled: data.ethernetEnabled ?? false,
          ethActivePreset: data.ethActivePreset ?? 255,
          ethPhyType: data.ethPhyType ?? 0,
          ethSpiBus: data.ethSpiBus ?? 1,
          ethRmiiConfig: data.ethRmiiConfig ?? [0, -1, -1, -1, 0],
          ethSpiConfig: data.ethSpiConfig ?? [20, -1, -1, -1, -1, -1, -1],
        },
        nfcPresets: nfcRes.success ? nfcRes.data : null,
        ethConfig: ethRes.success ? ethRes.data : null,
        error: null,
      };
    } catch (error) {
      console.error('Failed to load captive portal config:', error);
      meta.captivePortalData = {
        config: {
          wifiSsid: '',
          wifiPassword: '',
          setupCode: '46637726',
          hk_key_color: 0,
          nfcPinsPreset: 0,
          nfcGpioPins: [5, 18, 19, 23],
          ethernetEnabled: false,
          ethActivePreset: 255,
          ethPhyType: 0,
          ethSpiBus: 1,
          ethRmiiConfig: [0, -1, -1, -1, 0],
          ethSpiConfig: [20, -1, -1, -1, -1, -1, -1],
        },
        nfcPresets: null,
        ethConfig: null,
        error: error instanceof Error ? error.message : 'Unknown error',
      };
    } finally {
      setLoadingState(false);
    }
  },
} satisfies Hooks;
