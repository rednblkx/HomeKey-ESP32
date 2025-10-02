// Global loading states store using Svelte 5 runes

/**
 * @typedef {Object} LoadingStates
 * @property {boolean} pageLoading - Page-level loading state
 * @property {boolean} hkInfoLoading - HomeKey info loading state
 * @property {boolean} systemInfoLoading - System info loading state
 * @property {boolean} configLoading - Configuration loading state
 * @property {boolean} websocketLoading - WebSocket loading state
 */

/** @type {LoadingStates} */
export const loadingStates = $state({
  // Page-level loading states
  pageLoading: false,

  // Component-specific loading states
  hkInfoLoading: false,
  systemInfoLoading: false,

  // Async operation loading states
  configLoading: false,
  websocketLoading: false,
});

// Loading state management functions
/**
 * @param {keyof LoadingStates} key - The loading state key to set
 * @param {boolean} value - The loading state value
 */
export function setLoadingState(key, value) {
  loadingStates[key] = value;
}

/**
 * @param {keyof LoadingStates} key - The loading state key to get
 * @returns {boolean} The current loading state value
 */
export function getLoadingState(key) {
  return loadingStates[key];
}

/**
 * @param {(keyof LoadingStates)[]} keys - Array of loading state keys to check
 * @returns {boolean} True if any of the specified states are loading
 */
export function isAnyLoading(keys) {
  return keys.some(key => loadingStates[key]);
}

/**
 * @param {(keyof LoadingStates)[]} keys - Array of loading state keys to check
 * @returns {boolean} True if all of the specified states are loading
 */
export function isAllLoading(keys) {
  return keys.every(key => loadingStates[key]);
}

/**
 * Reset all loading states to false
 */
export function resetAllLoadingStates() {
  /** @type {(keyof LoadingStates)[]} */
  (Object.keys(loadingStates)).forEach(key => {
    loadingStates[key] = false;
  });
}