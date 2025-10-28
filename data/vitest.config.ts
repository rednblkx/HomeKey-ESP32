/// <reference types="vitest" />
import { defineConfig } from 'vitest/config';
import { svelte } from '@sveltejs/vite-plugin-svelte';
import { svelteTesting } from '@testing-library/svelte/vite';
import { resolve } from 'path';

export default defineConfig({
  plugins: [
    svelte({ hot: !process.env.VITEST }) as any,
    svelteTesting(),
  ],
  test: {
    include: ['src/**/*.test.{js,ts}'],
    environment: 'jsdom',
    setupFiles: ['./src/test/setup.ts'],
    globals: true,
    server: {
      deps: {
        inline: ['@sveltejs/kit']
      }
    }
  },
  resolve: {
    alias: {
      '@': resolve('./src'),
      '$lib': resolve('./src/lib')
    },
  },
});
