import type { PageLoad } from './$types';
import type { MiscConfig, EthConfig, NfcGpioPinsPreset } from '$lib/types/api';

export const ssr = false;

export const load: PageLoad = async ({ fetch }) => {
  try {
    const [miscResponse, ethResponse, nfcPresetsResponse] = await Promise.all([
      fetch('/config?type=misc'),
      fetch('/eth_get_config'),
      fetch('/nfc_get_presets')
    ]);

    const res = await Promise.all([miscResponse.json(), ethResponse.json(), nfcPresetsResponse.json()]);
    if (!res[0].success || !res[1].success || !res[2].success) {
      throw new Error(res[0].error || res[1].error || res[2].error);
    }

    const misc = res[0].data as MiscConfig;
    const eth = res[1].data as EthConfig;
    const nfcPresets = res[2].data as NfcGpioPinsPreset;

    return { misc, eth, nfcPresets };
  } catch (error) {
    console.error('Failed to load misc config:', error);
    return {
      misc: null,
      eth: null,
      error: error instanceof Error ? error.message : 'Unknown error'
    };
  }
}
