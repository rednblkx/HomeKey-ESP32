import { describe, it, expect, beforeEach } from 'vitest';
import { uiState, toggleDrawer, openDrawer, closeDrawer, setDrawerState } from './ui.svelte.js';

describe('ui.svelte.js', () => {
	beforeEach(() => {
		// Reset UI state to initial state before each test
		uiState.drawerOpen = false;
	});

	describe('uiState initialization', () => {
		it('should initialize with drawer closed', () => {
			expect(uiState.drawerOpen).toBe(false);
		});
	});

	describe('toggleDrawer function', () => {
		it('should toggle drawer from closed to open', () => {
			expect(uiState.drawerOpen).toBe(false);

			toggleDrawer();

			expect(uiState.drawerOpen).toBe(true);
		});

		it('should toggle drawer from open to closed', () => {
			uiState.drawerOpen = true;

			toggleDrawer();

			expect(uiState.drawerOpen).toBe(false);
		});

		it('should toggle multiple times correctly', () => {
			expect(uiState.drawerOpen).toBe(false);

			toggleDrawer();
			expect(uiState.drawerOpen).toBe(true);

			toggleDrawer();
			expect(uiState.drawerOpen).toBe(false);

			toggleDrawer();
			expect(uiState.drawerOpen).toBe(true);
		});
	});

	describe('openDrawer function', () => {
		it('should open the drawer when closed', () => {
			expect(uiState.drawerOpen).toBe(false);

			openDrawer();

			expect(uiState.drawerOpen).toBe(true);
		});

		it('should keep drawer open when already open', () => {
			uiState.drawerOpen = true;

			openDrawer();

			expect(uiState.drawerOpen).toBe(true);
		});
	});

	describe('closeDrawer function', () => {
		it('should close the drawer when open', () => {
			uiState.drawerOpen = true;

			closeDrawer();

			expect(uiState.drawerOpen).toBe(false);
		});

		it('should keep drawer closed when already closed', () => {
			expect(uiState.drawerOpen).toBe(false);

			closeDrawer();

			expect(uiState.drawerOpen).toBe(false);
		});
	});

	describe('setDrawerState function', () => {
		it('should set drawer to open', () => {
			expect(uiState.drawerOpen).toBe(false);

			setDrawerState(true);

			expect(uiState.drawerOpen).toBe(true);
		});

		it('should set drawer to closed', () => {
			uiState.drawerOpen = true;

			setDrawerState(false);

			expect(uiState.drawerOpen).toBe(false);
		});

		it('should handle setting to same state', () => {
			expect(uiState.drawerOpen).toBe(false);

			setDrawerState(false);

			expect(uiState.drawerOpen).toBe(false);
		});
	});

	describe('reactive behavior', () => {
		it('should be reactive to state changes', () => {
			let drawerState = false;

			// Simulate reactive effect
			const effect = () => {
				drawerState = uiState.drawerOpen;
			};

			effect();
			expect(drawerState).toBe(false);

			openDrawer();
			effect();
			expect(drawerState).toBe(true);

			closeDrawer();
			effect();
			expect(drawerState).toBe(false);
		});

		it('should handle direct state mutation', () => {
			expect(uiState.drawerOpen).toBe(false);

			uiState.drawerOpen = true;
			expect(uiState.drawerOpen).toBe(true);

			uiState.drawerOpen = false;
			expect(uiState.drawerOpen).toBe(false);
		});
	});

	describe('function composition', () => {
		it('should allow function chaining through state changes', () => {
			// Start closed
			expect(uiState.drawerOpen).toBe(false);

			// Open via function
			openDrawer();
			expect(uiState.drawerOpen).toBe(true);

			// Toggle (should close)
			toggleDrawer();
			expect(uiState.drawerOpen).toBe(false);

			// Set explicitly to open
			setDrawerState(true);
			expect(uiState.drawerOpen).toBe(true);

			// Close via function
			closeDrawer();
			expect(uiState.drawerOpen).toBe(false);
		});
	});

	describe('edge cases', () => {
		it('should handle rapid state changes', () => {
			// Simulate rapid toggling
			toggleDrawer(); // true
			toggleDrawer(); // false
			openDrawer();   // true
			closeDrawer();  // false
			setDrawerState(true);  // true
			toggleDrawer(); // false

			expect(uiState.drawerOpen).toBe(false);
		});

		it('should maintain state isolation between tests', () => {
			// This test should start with clean state regardless of previous tests
			expect(uiState.drawerOpen).toBe(false);
		});
	});
});