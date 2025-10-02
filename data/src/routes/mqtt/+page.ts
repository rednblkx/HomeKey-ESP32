import type { PageLoad } from './$types';
import type { MqttConfig } from '$lib/types/api';

export const ssr = false;

export const load: PageLoad = async ({ fetch }) => {
  try {
    const mqtt = await fetch('/config?type=mqtt');
    if (!mqtt.ok) {
      throw new Error(`HTTP error! status: ${mqtt.status}`);
    }
    return { mqtt: await mqtt.json() as MqttConfig, error: null };
  } catch (error) {
    console.error('Failed to load mqtt:', error);
    return {
      mqtt: null,
      error: error instanceof Error ? error.message : 'Unknown error'
    };
  }
}