import { createApp } from 'vue'
import App from '@/App.vue'
import router from '@/router'
import "@/style.css"
import ws from '@/services/ws.js'

const app = createApp(App)

app.use(router)

// Expose WebSocket globally - connection will be initiated in App.vue
app.config.globalProperties.$ws = ws

app.mount('#app')
