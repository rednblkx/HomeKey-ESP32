import type { LogEntry } from "$lib/types/api";

export let logs = $state<LogEntry[]>([]);
export const clearLogs = () => logs.length = 0;
let logIdCounter = $state<number>(0);
export const logIdIncrement = () => logIdCounter++;
export let realtimeLogging = $state<boolean>(true);
