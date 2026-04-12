import type { Hooks } from 'sv-router';
import type { MqttConfig } from '$lib/types/api';

declare module 'sv-router' {
  interface RouteMeta {
    mqttData?: { mqtt: MqttConfig | null; error: string | null };
  }
}

export default {
  async beforeLoad({ meta }) {
    try {
      const res = await fetch('/config?type=mqtt').then(r => r.json());
      if (!res.success) throw new Error(res.error);
      meta.mqttData = { mqtt: res.data as MqttConfig, error: null };
    } catch (error) {
      console.error('Failed to load mqtt:', error);
      meta.mqttData = { mqtt: null, error: error instanceof Error ? error.message : 'Unknown error' };
    }
  },
} satisfies Hooks;
