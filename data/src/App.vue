<template>
  <div class="drawer lg:drawer-open">
    <input id="main-content-drawer" type="checkbox" class="drawer-toggle" />
    <!-- Sidebar -->
    <div class="drawer-side">
      <label for="main-content-drawer" aria-label="close sidebar" class="drawer-overlay"></label>
      <div class="bg-base-200 text-base-content w-70 h-full flex flex-col">
        <!-- Header Section -->
        <div class="text-lg font-bold">
          <div class="flex items-center flex-col p-4 justify-center">
            <img :src="logoSrc" alt="logo" class="w-8 h-8" />
            <div class="flex items-center flex-col">
              <span>HomeKey-ESP32</span>
              <div ref="sidebarTrigger" @mouseenter="showDropdown" @mouseleave="hideDropdown" @click="toggleDropdown" class="relative">
                <div class="flex items-center cursor-pointer">
                  <span class="text-xs opacity-70 align-middle flex items-center gap-2">
                    <div class="inline-grid *:[grid-area:1/1]">
                      <div class="status animate-ping" :class="wsState==='open' ? 'status-success' : 'status-error'"></div>
                      <div class="status" :class="wsState==='open' ? 'status-success' : 'status-error'"></div>
                    </div>
                    <span :class="wsState==='open' ? 'text-success' : 'text-error'">{{ wsState === 'open' ? 'Online' : 'Offline' }}</span>
                  </span>
                </div>
              </div>
            </div>
            
            <!-- Teleported dropdown content with DaisyUI styling -->
            <Teleport to="body">
              <div 
                v-if="isDropdownVisible"
                @mouseenter="showDropdown" 
                @mouseleave="hideDropdown"
                class="absolute bg-base-100 rounded-box shadow-lg p-3 w-64 z-[9999] border border-base-300 transition-all duration-200"
                :style="dropdownStyle">
                <div class="text-sm">
                  <div><strong>Status:</strong> {{ wsState }}</div>
                  <div><strong>Connected:</strong> {{ wsConnected ? 'Yes' : 'No' }}</div>
                  <div v-if="wsInfo.reconnectAttempts > 0"><strong>Reconnect attempts:</strong> {{ wsInfo.reconnectAttempts }}/{{ wsInfo.maxReconnectAttempts }}</div>
                  <div v-if="wsInfo.url"><strong>URL:</strong> {{ wsInfo.url }}</div>
                </div>
                <div class="divider my-2"></div>
                <div class="flex gap-2">
                  <button @click="testWebSocket" class="btn btn-xs btn-primary flex-1">Test</button>
                  <button @click="reconnectWebSocket" class="btn btn-xs btn-secondary flex-1">Reconnect</button>
                </div>
              </div>
            </Teleport>
          </div>
        </div>
        
        <!-- Menu Section -->
        <ul class="menu flex-1 p-4 w-full"
            onclick="document.querySelector('#main-content-drawer.drawer-toggle').click();">
          <li class="my-1"><router-link to="/" class="text-lg flex items-center"><svg xmlns="http://www.w3.org/2000/svg"
              class="h-6 w-6 mr-2" fill="none" viewBox="0 0 24 24" stroke="currentColor">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2"
                d="M13 16h-1v-4h-1m1-4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
            </svg>Info</router-link></li>
          <li class="my-1">
            <router-link to="/mqtt" class="text-lg flex items-center">
              <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5"
                stroke="currentColor" class="w-6 mr-2">
                <path stroke-linecap="round" stroke-linejoin="round"
                  d="M9.348 14.652a3.75 3.75 0 0 1 0-5.304m5.304 0a3.75 3.75 0 0 1 0 5.304m-7.425 2.121a6.75 6.75 0 0 1 0-9.546m9.546 0a6.75 6.75 0 0 1 0 9.546M5.106 18.894c-3.808-3.807-3.808-9.98 0-13.788m13.788 0c3.808 3.807 3.808 9.98 0 13.788M12 12h.008v.008H12V12Zm.375 0a.375.375 0 1 1-.75 0 .375.375 0 0 1 .75 0Z" />
              </svg>
              MQTT
            </router-link>
          </li>
          <li class="my-1"><router-link to="/actions" class="text-lg flex items-center"><svg
              xmlns="http://www.w3.org/2000/svg" class="h-6 w-6 mr-2" fill="none" viewBox="0 0 24 24"
              stroke="currentColor">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2"
                d="M12 18h.01M8 21h8a2 2 0 002-2V5a2 2 0 00-2-2H8a2 2 0 00-2 2v14a2 2 0 002 2z" />
            </svg>Actions</router-link></li>
          <li class="my-1"><router-link to="/misc" class="text-lg flex items-center"><svg
              xmlns="http://www.w3.org/2000/svg" class="h-6 w-6 mr-2" fill="none" viewBox="0 0 24 24"
              stroke="currentColor">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2"
                d="M10.325 4.317c.426-1.756 2.924-1.756 3.35 0a1.724 1.724 0 002.573 1.066c1.543-.94 3.31.826 2.37 2.37a1.724 1.724 0 001.065 2.572c1.756.426 1.756 2.924 0 3.35a1.724 1.724 0 00-1.066 2.573c.94 1.543-.826 3.31-2.37 2.37a1.724 1.724 0 00-2.572 1.065c-.426 1.756-2.924 1.756-3.35 0a1.724 1.724 0 00-2.573-1.066c-1.543.94-3.31-.826-2.37-2.37a1.724 1.724 0 00-1.065-2.572c-1.756-.426-1.756-2.924 0-3.35a1.724 1.724 0 001.066-2.573c-.94-1.543.826-3.31 2.37-2.37.996.608 2.296.07 2.572-1.065z" />
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2"
                d="M15 12a3 3 0 11-6 0 3 3 0 016 0z" />
            </svg>System</router-link></li>
          <li class="mt-auto">
            <label class="swap swap-rotate">
              <input type="checkbox" @click="toggleTheme" :checked="theme === 'dracula'" />
              <svg class="swap-on w-6 h-6" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24"
                stroke="currentColor" stroke-width="2">
                <path stroke-linecap="round" stroke-linejoin="round"
                  d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z" />
              </svg>
              <svg class="swap-off w-6 h-6" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24"
                stroke="currentColor" stroke-width="2">
                <path stroke-linecap="round" stroke-linejoin="round"
                  d="M12 3v1m0 16v1m9-9h-1M4 12H3m15.364 6.364l-.707-.707M6.343 6.343l-.707-.707m12.728 0l-.707.707M6.343 17.657l-.707.707M16 12a4 4 0 11-8 0 4 4 0 018 0z" />
              </svg>
            </label>
          </li>
      </ul>
      </div>
    </div>
    <div class="drawer-content flex flex-col items-center justify-center">
      <!-- Mobile Navbar -->
      <div class="navbar bg-base-100 lg:hidden">
        <div class="navbar-start">
          <label for="main-content-drawer" class="btn btn-ghost drawer-button lg:hidden">
            <svg xmlns="http://www.w3.org/2000/svg" class="h-5 w-5" fill="none" viewBox="0 0 24 24"
              stroke="currentColor">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 6h16M4 12h8m-8 6h16" />
            </svg>
          </label>
        </div>
        <div class="navbar-center">
          <img :src="logoSrc" alt="logo" class="w-8 h-8 mr-2" />
          <div class="flex items-center flex-col">
            <span class="font-bold text-lg">HomeKey-ESP32</span>
            <!-- Status badge with teleported DaisyUI dropdown -->
            <div ref="mobileTrigger" @mouseenter="showDropdown" @mouseleave="hideDropdown" @click="toggleDropdown" class="relative">
              <div class="flex items-center cursor-pointer">
                <span class="text-xs opacity-70 align-middle flex items-center gap-2">
                  <div class="inline-grid *:[grid-area:1/1]">
                    <div class="status animate-ping" :class="wsState==='open' ? 'status-success' : 'status-error'"></div>
                    <div class="status" :class="wsState==='open' ? 'status-success' : 'status-error'"></div>
                  </div>
                  <span :class="wsState==='open' ? 'text-success' : 'text-error'">{{ wsState === 'open' ? 'Online' : 'Offline' }}</span>
                </span>
              </div>
            </div>
          </div>
        </div>
        <div class="navbar-end">
          <label class="swap swap-rotate mr-4">
            <input type="checkbox" @click="toggleTheme" :checked="theme === 'dracula'" />
            <svg class="swap-on w-6 h-6" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24"
              stroke="currentColor" stroke-width="2">
              <path stroke-linecap="round" stroke-linejoin="round"
                d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z" />
            </svg>
            <svg class="swap-off w-6 h-6" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24"
              stroke="currentColor" stroke-width="2">
              <path stroke-linecap="round" stroke-linejoin="round"
                d="M12 3v1m0 16v1m9-9h-1M4 12H3m15.364 6.364l-.707-.707M6.343 6.343l-.707-.707m12.728 0l-.707.707M6.343 17.657l-.707.707M16 12a4 4 0 11-8 0 4 4 0 018 0z" />
            </svg>
          </label>
        </div>
      </div>
      <!-- Content -->
      <main class="flex-1 p-6 overflow-y-auto w-full">
        <router-view></router-view>
      </main>
    </div>
  </div>
</template>

<script>
import { computed } from 'vue';

export default {
  data() {
    return {
      theme: 'autumn',
      dropdownToggle: '',
      wsState: 'connecting',
      wsConnected: false,
      wsInfo: {},
      sysInfo: {},
      _wsOff: null,
      _wsPoll: null,
      isDropdownVisible: false,
      dropdownStyle: {},
      hideTimeout: null,
      clickOutsideHandler: null,
    };
  },
  provide() {
    return {
      sysInfo: computed(() => this.sysInfo),
    };
  },
  computed: {
    logoSrc() {
      return this.theme === 'dracula' ? '/static/logo-white.webp' : '/static/logo-dark.webp';
    },
  },
  watch: {
    isDropdownVisible(newVal) {
      if (newVal) {
        this.$nextTick(() => {
          this.addClickOutsideListener();
        });
      } else {
        this.removeClickOutsideListener();
      }
    },
  },
  methods: {
    toggleTheme() {
      this.theme = this.theme === 'dracula' ? 'autumn' : 'dracula';
      document.documentElement.setAttribute('data-theme', this.theme);
      localStorage.setItem('theme', this.theme);
    },
    getSystemTheme() {
      return window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dracula' : 'autumn';
    },
    testWebSocket() {
      if (this.$ws) {
        // Test with echo
        this.$ws.echo({ test: 'Hello WebSocket!', timestamp: Date.now() });
        // Request status
        this.$ws.requestStatus();
        // Send ping
        this.$ws.ping();
        
        // Show notification
        console.log('WebSocket test messages sent');
      }
    },
    reconnectWebSocket() {
      if (this.$ws) {
        this.$ws.disconnect();
        setTimeout(() => {
          this.$ws.connect();
        }, 1000);
      }
    },
    updateWebSocketInfo() {
      if (this.$ws && typeof this.$ws.getConnectionInfo === 'function') {
        this.wsInfo = this.$ws.getConnectionInfo();
        this.wsConnected = this.wsInfo.connected || false;
      }
    },
    showDropdown() {
      if (this.hideTimeout) {
        clearTimeout(this.hideTimeout);
        this.hideTimeout = null;
      }
      this.isDropdownVisible = true;
      this.$nextTick(() => {
        this.calculateDropdownPosition();
      });
    },
    hideDropdown() {
      this.hideTimeout = setTimeout(() => {
        this.isDropdownVisible = false;
      }, 300);
    },
    toggleDropdown() {
      if (this.isDropdownVisible) {
        this.isDropdownVisible = false;
      } else {
        this.showDropdown();
      }
    },
    calculateDropdownPosition() {
      const viewportWidth = window.innerWidth;
      const isMobile = viewportWidth < 1024;
      
      let triggerElement = isMobile ? this.$refs.mobileTrigger : this.$refs.sidebarTrigger;
      if (!triggerElement) return;
      
      const rect = triggerElement.getBoundingClientRect();
      const dropdownWidth = 256; // w-64
      
      let left, top;
      
      if (isMobile) {
        // Center horizontally on mobile, position below badge
        left = Math.max(8, (viewportWidth - dropdownWidth) / 2);
        top = rect.bottom + 8;
      } else {
        // Position to the right on desktop, with fallback to left
        left = rect.right + 8;
        top = rect.top;
        
        // If it would go off-screen, position to the left
        if (left + dropdownWidth > viewportWidth) {
          left = rect.left - dropdownWidth - 8;
        }
      }
      
      this.dropdownStyle = {
        left: `${left}px`,
        top: `${top}px`,
      };
    },
    addClickOutsideListener() {
      this.clickOutsideHandler = (event) => {
        const dropdown = event.target.closest('.fixed.bg-base-100');
        const sidebarTrigger = this.$refs.sidebarTrigger;
        const mobileTrigger = this.$refs.mobileTrigger;
        
        if (!dropdown && 
            (!sidebarTrigger || !sidebarTrigger.contains(event.target)) &&
            (!mobileTrigger || !mobileTrigger.contains(event.target))) {
          this.isDropdownVisible = false;
        }
      };
      
      setTimeout(() => {
        document.addEventListener('click', this.clickOutsideHandler);
      }, 100);
    },
    removeClickOutsideListener() {
      if (this.clickOutsideHandler) {
        document.removeEventListener('click', this.clickOutsideHandler);
        this.clickOutsideHandler = null;
      }
    },
  },
  mounted() {
    const savedTheme = localStorage.getItem('theme');
    if (savedTheme) {
      this.theme = savedTheme;
    } else {
      this.theme = this.getSystemTheme();
    }
    document.documentElement.setAttribute('data-theme', this.theme);

    window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', e => {
      if (!localStorage.getItem('theme')) {
        this.theme = e.matches ? 'dracula' : 'autumn';
        document.documentElement.setAttribute('data-theme', this.theme);
      }
    });

    // Initialize WebSocket connection
    if (this.$ws) {
      this.$ws.connect();
    }

    // WebSocket status wiring
    this.updateWebSocketInfo();
    if (this.$ws && typeof this.$ws.state === 'function') {
      this.wsState = this.$ws.state();
    } else if (this.$ws && this.$ws.connected) {
      this.wsState = 'open';
    } else {
      this.wsState = 'closed';
    }
    
    this._wsOff = this.$ws?.on((evt) => {
      if (evt.type === 'status') {
        this.wsState = evt.state;
        this.updateWebSocketInfo();
      } else if (evt.type === 'message') {
        // Log received messages for debugging
        console.log('WebSocket message received:', evt.data);
        switch (evt.data.type) {
          case 'sysinfo':
            this.sysInfo = { ...this.sysInfo, ...evt.data };
            break;
          case 'metrics':
            this.sysInfo = { ...this.sysInfo, ...evt.data };
            break;
        }
      }
    }) || null;

    // Fallback: poll state periodically in case early events were missed
    this._wsPoll = setInterval(() => {
      this.updateWebSocketInfo();
      if (this.$ws && typeof this.$ws.state === 'function') {
        const s = this.$ws.state();
        if (s && s !== this.wsState) this.wsState = s;
      }
    }, 2000);



  },
  beforeUnmount() {
    if (this._wsOff) {
      try { this._wsOff(); } catch (_) {}
      this._wsOff = null;
    }
    if (this._wsPoll) {
      clearInterval(this._wsPoll);
      this._wsPoll = null;
    }
    this.removeClickOutsideListener();
  },
};
</script>
