import { defineConfig } from "vitest/config";
import { fileURLToPath, URL } from "node:url";
import devtoolsJson from "vite-plugin-devtools-json";
import tailwindcss from "@tailwindcss/vite";
import { sveltekit } from "@sveltejs/kit/vite";

export default defineConfig(({ mode }) => {
  const isDev = mode === "development";

  return {
    plugins: [
      sveltekit(),
      tailwindcss(),
      ...(isDev ? [devtoolsJson()] : [])
    ],
    define: { __DEV__: isDev },
    resolve: {
      alias: {
        "@": fileURLToPath(new URL("./src", import.meta.url)),
      },
    },
    build: {
      cssMinify: 'lightningcss',
      minify: 'esbuild',
      target: 'es2020',
      rollupOptions: {
        output: {
          manualChunks: undefined,
        },
      },
    },
    esbuild: {
      drop: isDev ? [] : ['console', 'debugger'],
      legalComments: 'none',
    },
    server: {
      proxy: {
        "/certificates": "http://localhost:8000",
        "/config": "http://localhost:8000",
        "/eth_get_config": "http://localhost:8000",
        "/nfc_get_presets": "http://localhost:8000",
        "/ws": { target: "ws://localhost:8000", ws: true },
      },
    },
  };
});
