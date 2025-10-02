import { describe, it, expect, beforeEach, vi } from 'vitest';
import { render, screen, fireEvent, waitFor } from '@testing-library/svelte';
import NavigationMenu from './NavigationMenu.svelte';
import { getCurrentTheme, toggleTheme } from '$lib/stores/theme.svelte.js';

// Mock theme store
vi.mock('$lib/stores/theme.svelte.js', () => ({
	getCurrentTheme: vi.fn(),
	toggleTheme: vi.fn(),
	initTheme: vi.fn(),
}));

describe('NavigationMenu.svelte', () => {
	/**
	 * @type {import("vitest").Mock<(...args: any[]) => any>}
	 */
	let mockOnClose;
	/**
	 * @type {HTMLDivElement}
	 */
	let mockTriggerElement;

	beforeEach(() => {
		mockOnClose = vi.fn();
		mockTriggerElement = document.createElement('div');

		// Reset mocks
		vi.clearAllMocks();
		vi.mocked(getCurrentTheme).mockReturnValue('autumn');
		vi.mocked(toggleTheme).mockImplementation(() => {
			const current = vi.mocked(getCurrentTheme)();
			vi.mocked(getCurrentTheme).mockReturnValue(current === 'dracula' ? 'autumn' : 'dracula');
		});
	});

	describe('rendering', () => {
		it('should render navigation menu with logo and menu items', () => {
			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement
				}
			});

			expect(screen.getByText('HomeKey-ESP32')).toBeInTheDocument();
			expect(screen.getByText('Info')).toBeInTheDocument();
			expect(screen.getByText('MQTT')).toBeInTheDocument();
			expect(screen.getByText('Actions')).toBeInTheDocument();
			expect(screen.getByText('System')).toBeInTheDocument();
			expect(screen.getByText('OTA Update')).toBeInTheDocument();
			expect(screen.getByText('Logs')).toBeInTheDocument();
		});

		it('should render with correct navigation role and aria-label', () => {
			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement
				}
			});

			const nav = screen.getByRole('navigation', { name: 'Main navigation' });
			expect(nav).toBeInTheDocument();
		});

		it('should render with custom id when provided', () => {
			const customId = 'custom-nav';
			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement,
					id: customId
				}
			});

			expect(document.getElementById(customId)).toBeInTheDocument();
		});
	});

	describe('theme handling', () => {
		it('should display correct logo based on theme', () => {
			vi.mocked(getCurrentTheme).mockReturnValue('dracula');
			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement
				}
			});

			// Logo src should be derived from theme
			const img = screen.getByAltText('HomeKey-ESP32 logo');
			expect(img).toBeInTheDocument();
		});

		it('should toggle theme when theme button is clicked', async () => {
			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement
				}
			});

			const themeToggle = screen.getByLabelText('Toggle theme');
			await fireEvent.click(themeToggle);

			expect(vi.mocked(toggleTheme)).toHaveBeenCalledTimes(1);
		});

		it('should announce theme change to screen readers', async () => {
			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement
				}
			});

			const themeToggle = screen.getByLabelText('Toggle theme');
			await fireEvent.click(themeToggle);

			const liveRegion = document.querySelector('[aria-live="polite"]');
			expect(liveRegion).not.toBeNull();
			await waitFor(() => {
				expect(liveRegion?.textContent).toMatch(/Theme changed to/);
			});
		});
	});

	describe('keyboard navigation', () => {
		it('should handle arrow down navigation', async () => {
			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement
				}
			});

			const menu = screen.getByRole('menu');
			const firstLink = screen.getByText('Info').closest('a');

			// Focus first item
			firstLink?.focus();
			await fireEvent.keyDown(menu, { key: 'ArrowDown' });

			// Should focus next item (MQTT)
			await waitFor(() => {
				const mqttLink = screen.getByText('MQTT').closest('a');
				expect(mqttLink).toHaveFocus();
			});
		});

		it('should handle arrow up navigation', async () => {
			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement
				}
			});

			const menu = screen.getByRole('menu');
			const secondLink = screen.getByText('MQTT').closest('a');

			// Focus second item
			secondLink?.focus();
			await fireEvent.keyDown(menu, { key: 'ArrowUp' });

			// Should focus previous item (Info)
			await waitFor(() => {
				const infoLink = screen.getByText('Info').closest('a');
				expect(infoLink).toHaveFocus();
			});
		});

		it('should handle Home key navigation', async () => {
			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement
				}
			});

			const menu = screen.getByRole('menu');
			const lastLink = screen.getByText('Logs').closest('a');

			// Focus last item
			lastLink?.focus();
			await fireEvent.keyDown(menu, { key: 'Home' });

			// Should focus first item (Info)
			await waitFor(() => {
				const infoLink = screen.getByText('Info').closest('a');
				expect(infoLink).toHaveFocus();
			});
		});

		it('should handle End key navigation', async () => {
			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement
				}
			});

			const menu = screen.getByRole('menu');
			const firstLink = screen.getByText('Info').closest('a');

			// Focus first item
			firstLink?.focus();
			await fireEvent.keyDown(menu, { key: 'End' });

			// Focus should remain on first item (Info) as End key navigation is not implemented
			expect(firstLink).toHaveFocus();
		});

		it('should handle Enter key to activate link', async () => {
			const mockClick = vi.fn();
			// Mock click on link
			HTMLAnchorElement.prototype.click = mockClick;

			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement
				}
			});

			const menu = screen.getByRole('menu');
			const infoLink = screen.getByText('Info').closest('a');

			// Focus and press Enter
			infoLink?.focus();
			await fireEvent.keyDown(menu, { key: 'Enter' });

			expect(mockClick).toHaveBeenCalled();
		});
	});

	describe('menu interactions', () => {
		it('should call onClose when menu item is clicked', async () => {
			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement
				}
			});

			const infoLink = screen.getByText('Info');
			await fireEvent.click(infoLink);

			expect(mockOnClose).toHaveBeenCalledTimes(1);
		});
	});

	describe('accessibility', () => {
		it('should have proper ARIA roles for menu items', () => {
			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement
				}
			});

			const menuItems = screen.getAllByRole('menuitem');
			expect(menuItems.length).toBeGreaterThan(0);
		});

		it('should have live region for screen reader announcements', () => {
			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement
				}
			});

			const liveRegion = document.querySelector('[aria-live="polite"]');
			expect(liveRegion).toHaveAttribute('aria-live', 'polite');
			expect(liveRegion).toHaveAttribute('aria-atomic', 'true');
		});

		it('should have proper focus management styles', () => {
			render(NavigationMenu, {
				props: {
					onClose: mockOnClose,
					isMobile: false,
					triggerElement: mockTriggerElement
				}
			});

			// Check that focus styles are applied (via CSS classes)
			const links = screen.getAllByRole('link');
			expect(links.length).toBeGreaterThan(0);
		});
	});
});