import { defineConfig } from "vite";
import { fileURLToPath, URL } from "node:url";
import devtoolsJson from "vite-plugin-devtools-json";
import tailwindcss from "@tailwindcss/vite";
import { svelte } from "@sveltejs/vite-plugin-svelte";
import { router } from "sv-router/vite-plugin";
import compression from "vite-plugin-compression2";
import { execSync } from "node:child_process";

export default defineConfig(({ mode }) => {
  const isDev = mode === "development";
  const version : string = execSync('git rev-parse --short=16 HEAD').toString().trim();

  return {
    plugins: [
      svelte(),
      router({
        path: "src/routes",
        allLazy: false,
        js: false,
      }),
      tailwindcss(),
      ...(isDev ? [devtoolsJson()] : []),
      compression({
        algorithms: [
          'gzip'
        ],
        deleteOriginalAssets: true
      })
    ],
    define: { __DEV__: isDev, __VERSION__: JSON.stringify(version) },
    resolve: {
      alias: {
        "@": fileURLToPath(new URL("./src", import.meta.url)),
        "$lib": fileURLToPath(new URL("./src/lib", import.meta.url)),
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
        "/captive_portal_config": "http://localhost:8000"
      },
    },
  };
});
