import '@testing-library/jest-dom';
import { beforeAll, vi } from 'vitest';

// Mock WebSocket for tests
// @ts-ignore
global.WebSocket = class MockWebSocket {
  static CONNECTING = 0;
  static OPEN = 1;
  static CLOSING = 2;
  static CLOSED = 3;

  constructor() {
    // @ts-ignore
    this.readyState = 1; // OPEN
    // @ts-ignore
    this.onopen = null;
    // @ts-ignore
    this.onmessage = null;
    // @ts-ignore
    this.onclose = null;
    // @ts-ignore
    this.onerror = null;
  }

  send() { }
  close() { }
};

// Mock ResizeObserver
// @ts-ignore
global.ResizeObserver = class ResizeObserver {
  constructor(cb: any) {
    // @ts-ignore
    this.cb = cb;
  }
  observe() { }
  unobserve() { }
  disconnect() { }
};

// Mock matchMedia
Object.defineProperty(window, 'matchMedia', {
  writable: true,
  value: (query: string) => ({
    matches: false,
    media: query,
    onchange: null,
    addListener: () => { },
    removeListener: () => { },
    addEventListener: () => { },
    removeEventListener: () => { },
    dispatchEvent: () => { },
  }),
});

// Mock IntersectionObserver
// @ts-ignore
global.IntersectionObserver = class IntersectionObserver {
  constructor() { }
  observe() { }
  unobserve() { }
  disconnect() { }
};

// Mock onMount to prevent SSR issues
vi.mock('svelte', async () => {
  const actual = await vi.importActual('svelte');
  return {
    ...actual,
    onMount: vi.fn((fn) => fn()), // Execute immediately for tests
  };
});

vi.mock('$app/environment', async () => {
  const actual = await vi.importActual('$app/environment');
  return {
    ...actual
  };
});

// Setup global test utilities
beforeAll(() => {
  // Any global setup can go here
});
