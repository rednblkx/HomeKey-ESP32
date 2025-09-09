import { fileURLToPath, URL } from 'node:url'

import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import vueDevTools from 'vite-plugin-vue-devtools'
import { compression } from 'vite-plugin-compression2'
import tailwindcss from "@tailwindcss/vite";

// https://vite.dev/config/
export default defineConfig({
  plugins: [
    tailwindcss(),
    vue(),
    vueDevTools(),
    compression({
      algorithms: [
        'gzip'
      ],
      deleteOriginalAssets: true,
      exclude: /\.(html)$/
    })
  ],
  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url))
    },
  },
  server: {
    proxy: {
      "/config": "http://localhost:8000",
      "/eth_get_config": "http://localhost:8000"
    }
  }
})
