import type { PageLoad } from './$types';
import type { MiscConfig } from '$lib/types/api';

export const ssr = false;

export const load: PageLoad = async ({ fetch }) => {
   try {
     const actions = await fetch('/config?type=actions');
     if (!actions.ok) {
       throw new Error(`HTTP error! status: ${actions.status}`);
     }
     return { actions: await actions.json() as MiscConfig, error: null };
   } catch (error) {
     console.error('Failed to load actions:', error);
     return {
       actions: null,
       error: error instanceof Error ? error.message : 'Unknown error'
     };
   }
}