import type { PageLoad } from './$types';
import type { MiscConfig, EthConfig } from '$lib/types/api';

export const ssr = false;

export const load: PageLoad = async ({ fetch }) => {
  try {
    const [miscResponse, ethResponse] = await Promise.all([
      fetch('/config?type=misc'),
      fetch('/eth_get_config')
    ]);

    if (!miscResponse.ok) {
      throw new Error(`HTTP error loading misc! status: ${miscResponse.status}`);
    }
    if (!ethResponse.ok) {
      throw new Error(`HTTP error loading eth config! status: ${ethResponse.status}`);
    }

    const misc = await miscResponse.json() as MiscConfig;
    const eth = await ethResponse.json() as EthConfig;

    return { misc, eth, error: null };
  } catch (error) {
    console.error('Failed to load misc config:', error);
    return {
      misc: null,
      eth: null,
      error: error instanceof Error ? error.message : 'Unknown error'
    };
  }
}