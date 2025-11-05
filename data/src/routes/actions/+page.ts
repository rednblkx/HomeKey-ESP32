import type { PageLoad } from './$types';
import type { ActionsConfig } from '$lib/types/api';

export const ssr = false;

export const load: PageLoad = async ({ fetch }) => {
   try {
     const actions = await fetch('/config?type=actions');
     const res = await actions.json();
     if (!res.success) {
       throw new Error(res.error);
     }
     return { actions: res.data as ActionsConfig, error: null };
   } catch (error) {
     console.error('Failed to load actions:', error);
     return {
       actions: null,
       error: error instanceof Error ? error.message : 'Unknown error'
     };
   }
}
