import { defineConfig } from "vitest/config";
import { fileURLToPath, URL } from "node:url";
import devtoolsJson from "vite-plugin-devtools-json";
import tailwindcss from "@tailwindcss/vite";
import { sveltekit } from "@sveltejs/kit/vite";

export default defineConfig(({ mode }) => {
  const isDev = mode === "development";

  return {
    plugins: [sveltekit(), tailwindcss(), devtoolsJson()],
    define: { __DEV__: isDev },
    resolve: {
      alias: {
        "@": fileURLToPath(new URL("./src", import.meta.url)),
      },
      conditions: process.env.VITEST ? ['browser'] : undefined,
    },
    server: {
      proxy: {
        "/certificates": "http://localhost:8000",
        "/config": "http://localhost:8000",
        "/eth_get_config": "http://localhost:8000",
        "/ws": { target: "ws://localhost:8000", ws: true },
      },
    },
    test: {
      expect: { requireAssertions: true },
      projects: [
        {
          extends: "./vite.config.ts",
          test: {
            name: "client",
            environment: "browser",
            browser: {
              enabled: true,
              provider: "playwright",
              instances: [{ browser: "chromium" }],
            },
            include: ["src/**/*.svelte.{test,spec}.{js,ts}"],
            exclude: ["src/lib/server/**"],
            setupFiles: ["./vitest-setup-client.ts"],
          },
        },
        {
          extends: "./vite.config.ts",
          test: {
            name: "server",
            environment: "node",
            include: ["src/**/*.{test,spec}.{js,ts}"],
            exclude: ["src/**/*.svelte.{test,spec}.{js,ts}"],
          },
        },
      ],
    },
  };
});
