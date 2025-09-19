import { fileURLToPath, URL } from 'node:url'

import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import vueDevTools from 'vite-plugin-vue-devtools'
import { compression } from 'vite-plugin-compression2'
import tailwindcss from "@tailwindcss/vite";

// https://vite.dev/config/
export default defineConfig(({ mode }) => {
  const isDev = mode === 'development'
  
  return {
    plugins: [
      tailwindcss(),
      vue(),
      // Only include devtools in development
      vueDevTools(),
      compression({
        algorithms: [
          'gzip'
        ],
        deleteOriginalAssets: true,
        exclude: /\.(html)$/
      })
    ],
    define: {
      // Make environment available to your app
      __DEV__: isDev,
      __PROD__: !isDev
    },
    resolve: {
      alias: {
        '@': fileURLToPath(new URL('./src', import.meta.url))
      },
    },
    server: {
      proxy: {
        "/config": "http://localhost:8000",
        "/eth_get_config": "http://localhost:8000",
        "/ws": {
          target: "ws://localhost:8000",
          ws: true
        }
      }
    }
  }
})
