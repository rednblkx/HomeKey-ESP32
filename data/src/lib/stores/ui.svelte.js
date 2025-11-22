// UI State Store for layout management
// Using Svelte 5 runes for reactive state management

/**
 * @typedef {Object} UIState
 * @property {boolean} drawerOpen - Whether the main drawer is open
 */

/**
 * @type {UIState}
 */
export const uiState = $state({
  drawerOpen: false,
});

/**
 * Toggle the drawer open/closed state
 */
export function toggleDrawer() {
  uiState.drawerOpen = !uiState.drawerOpen;
}

/**
 * Open the drawer
 */
export function openDrawer() {
  uiState.drawerOpen = true;
}

/**
 * Close the drawer
 */
export function closeDrawer() {
  uiState.drawerOpen = false;
}

/**
 * Set the drawer state explicitly
 * @param {boolean} isOpen
 */
export function setDrawerState(isOpen) {
  uiState.drawerOpen = isOpen;
}