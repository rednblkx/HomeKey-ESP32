import { createRouter, createWebHashHistory } from 'vue-router'
import HKInfo from '@/components/HKInfo.vue'
import AppMqtt from '@/components/AppMqtt.vue'
import AppActions from '@/components/AppActions.vue'
import AppMisc from '@/components/AppMisc.vue'
import AppOTA from '@/components/AppOTA.vue'
import LogViewer from '@/components/LogViewer.vue'

const routes = [
  {
    path: '/',
    name: 'HKInfo',
    component: HKInfo
  },
  {
    path: '/mqtt',
    name: 'Mqtt',
    component: AppMqtt
  },
  {
    path: '/actions',
    name: 'Actions',
    component: AppActions
  },
  {
    path: '/misc',
    name: 'Misc',
    component: AppMisc
  },
  {
    path: '/ota',
    name: 'OTA',
    component: AppOTA
  },
  {
    path: '/logs',
    name: 'Logs',
    component: LogViewer
  }
]

const router = createRouter({
  history: createWebHashHistory(),
  routes
})

export default router
