<template>
  <div class="h-full flex flex-col bg-base-200 text-base-content font-sans">
    <!-- Toolbar -->
    <div class="flex-shrink-0 bg-base-100 border-b border-base-300 p-2">
      <div class="flex items-center justify-between flex-wrap gap-2">
        <div class="flex items-center gap-4 flex-wrap justify-center">
          <div class="form-control">
            <input type="text" placeholder="Search…" class="input input-sm input-bordered w-48" v-model="searchText"
              aria-label="Search logs" />
          </div>
          <!-- Log Level Filter -->
          <div class="form-control">
            <div class="flex items-center gap-2" role="group" aria-label="Log level filters">
                <span class="label-text mr-2">Levels:</span>
              <button v-for="level in Object.keys(logLevels)" :key="level" @click="toggleLogLevel(level)"
                :class="['btn btn-xs', logLevels[level] ? 'btn-accent' : 'btn-outline']" :aria-pressed="logLevels[level]">
                {{ level }}
              </button>
            </div>
          </div>
        </div>

        <div class="flex items-center gap-2 flex-wrap">
          <div class="flex gap-2">
            <div class="form-control">
              <label class="label cursor-pointer p-0">
                <span class="label-text mr-2">Real-time</span>
                <input type="checkbox" class="toggle toggle-sm" v-model="realtime"
                  aria-label="Toggle real-time log updates" />
              </label>
            </div>
            <div class="form-control">
              <label class="label cursor-pointer p-0">
                <span class="label-text mr-2">Auto-scroll</span>
                <input type="checkbox" class="toggle toggle-sm" v-model="autoScroll" aria-label="Toggle auto-scroll" />
              </label>
            </div>
          </div>
          <div class="flex gap-2">
            <button @click="exportLogs()" class="btn btn-sm btn-primary">Export</button>
            <button @click="clearLogs" class="btn btn-sm btn-error">Clear Logs</button>
          </div>
        </div>
      </div>
    </div>

    <!-- Log Display -->
    <div class="bg-base-200 rounded-xl overflow-hidden">
      <div class="bg-base-200 rounded-xl p-2 h-full">
        <DynamicScroller
          ref="scroller"
          class="h-full font-mono text-sm scroll-container"
          :items="filteredLogs"
          :min-item-size="64"
          key-field="id"
          v-slot="{ item, index, active }"
          @scroll="handleScroll"
          @touchstart="handleTouchStart"
          @touchmove="handleTouchMove"
          @touchend="handleTouchEnd"
        >
        <DynamicScrollerItem
          :item="item"
          :active="active"
          :size-dependencies="[item.expanded]"
          :data-index="index"
        >
          <div class="w-full mb-1 last:mb-0">
            <div
              class="flex items-center justify-between sm:gap-4 md:gap-6 p-4 cursor-pointer hover:bg-base-200 transition-colors bg-base-100 border border-base-300 rounded-md"
              @click="toggleExpanded(item)"
            >
              <div class="flex items-center">
                <div class="flex items-center">
                  <div class="flex items-center justify-center mr-1">
                    <svg
                      class="w-4 h-4 transition-transform duration-200"
                      :class="item.expanded ? 'rotate-90' : ''"
                      fill="none"
                      stroke="currentColor"
                      viewBox="0 0 24 24"
                    >
                      <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M9 5l7 7-7 7"></path>
                    </svg>
                  </div>
                  <component :is="getLogLevelIcon(item.level)" :class="['h-4 w-4 mr-1 flex-shrink-0', getLogLevelColor(item.level)]" aria-hidden="true" />
                  <span :class="['font-bold mr-2', getLogLevelColor(item.level)]">{{ item.level }}</span>
                </div>
                <div class="flex max-sm:flex-col">
                  <span class="text-base-content/90 mr-2 badge badge-dash">{{ item.tag }}</span>
                  <span class="line-clamp-1 text-base-content/90 break-all wrap-anywhere ">{{ item.msg }}</span>
                </div>
              </div>
              <div class="tooltip tooltip-left" :data-tip="item.localts">
                <span class="text-xs text-base-content/50 whitespace-wrap">{{ formatRelativeTime(item.localts) }}</span>
              </div>
            </div>
            <div
              class="bg-base-100/50 w-full flex border-t border-base-300 transition-all duration-200 ease-in-out rounded-b-md"
              :class="item.expanded ? 'max-h-96 opacity-100' : 'max-h-0 opacity-0'"
              style="overflow: hidden;"
            >
              <div class="flex flex-col p-4">
                <p class="font-mono text-xs wrap-anywhere text-wrap break-all"><strong>Device Timestamp:</strong> {{ item.ts }}</p>
                <div class="mt-2 text-xs whitespace-pre-wrap break-all">{{ item.msg }}</div>
                <button @click="copyLog(item)" class="btn btn-xs btn-soft btn-secondary mt-2"
                  aria-label="Copy log message">
                  <ClipboardIcon class="w-4 h-4" />
                </button>
              </div>
            </div>
          </div>
        </DynamicScrollerItem>
      </DynamicScroller>
      </div>
    </div>
  </div>
</template>

<script setup>
import {
  ref,
  onMounted,
  onBeforeUnmount,
  computed,
  watch,
  nextTick
} from 'vue';
import {
  ExclamationCircleIcon,
  ExclamationTriangleIcon,
  InformationCircleIcon,
  CheckCircleIcon,
  ClipboardIcon
} from '@heroicons/vue/24/solid';
import {
  inject
} from 'vue';
import { DynamicScroller, DynamicScrollerItem } from 'vue-virtual-scroller';
import 'vue-virtual-scroller/dist/vue-virtual-scroller.css';

// Media query for mobile detection
const isMobile = ref(window.innerWidth < 768);
const mobileMediaQuery = window.matchMedia('(max-width: 767px)');

// FILTERING AND DATA
const searchText = ref('');
const realtime = ref(true);
const autoScroll = ref(true);
const scroller = ref(null);
const userHasScrolled = ref(false);
const logLevels = ref({
  ERROR: true,
  WARN: true,
  INFO: true,
  DEBUG: true,
});
const logs = ref([]);
let simulationInterval = null;

// Enhanced scroll handling for mobile/touch devices
const touchStartY = ref(0);
const touchEndY = ref(0);
const isScrolling = ref(false);
const scrollAnimationFrame = ref(null);
const SCROLL_EDGE_MARGIN = 16; // 16px edge margin as requested
const SMOOTH_SCROLL_DURATION = 300; // ms for smooth scroll animation

/**
 * Toggles the expanded state of a log item for virtual scrolling.
 * @param {object} item - The log item to toggle.
 */
const toggleExpanded = (item) => {
  item.expanded = !item.expanded;
};

/**
 * Returns the appropriate icon component for a given log level.
 * @param {string} level - The log level (e.g., 'ERROR', 'WARN').
 */
const getLogLevelIcon = (level) => {
  const icons = {
    ERROR: ExclamationCircleIcon,
    WARN: ExclamationTriangleIcon,
    INFO: InformationCircleIcon,
    DEBUG: CheckCircleIcon,
  };
  return icons[level] || InformationCircleIcon;
};

/**
 * Returns the TailwindCSS color class for a given log level.
 * @param {string} level - The log level.
 */
const getLogLevelColor = (level) => {
  const colors = {
    ERROR: 'text-error',
    WARN: 'text-warning',
    INFO: 'text-info',
    DEBUG: 'text-accent',
  };
  return colors[level] || 'text-base-content';
};

/**
 * Formats a timestamp into a relative time string (e.g., "5m ago").
 * @param {string} ts - The ISO timestamp string.
 */
const formatRelativeTime = (ts) => {
  const now = new Date();
  const then = new Date(ts);
  const diffInSeconds = Math.floor((now - then) / 1000);

  if (diffInSeconds < 5) return 'just now';
  if (diffInSeconds < 60) return `${diffInSeconds}s ago`;
  if (diffInSeconds < 3600) return `${Math.floor(diffInSeconds / 60)}m ago`;
  if (diffInSeconds < 86400) return `${Math.floor(diffInSeconds / 3600)}h ago`;
  return then.toLocaleDateString();
};

/**
 * Copies a single log entry to the clipboard.
 * @param {object} log - The log object to copy.
 */
const copyLog = (log) => {
  const logText = `[${log.localts}] [${log.level}] ${log.msg}`;
  navigator.clipboard.writeText(logText);
  // TODO: Add user feedback (e.g., a toast notification)
};


/**
 * Clears all logs from the display.
 */
const clearLogs = () => {
  logs.value = [];
};

/**
 * Toggles the visibility of a specific log level.
 * @param {string} level - The log level to toggle.
 */
const toggleLogLevel = (level) => {
  logLevels.value[level] = !logLevels.value[level];
  localStorage.setItem('logviewer-levels', JSON.stringify(logLevels.value));
};

/**
 * A computed property that filters logs based on search text and log level.
 */
const filteredLogs = computed(() => {
  return logs.value.filter(log => {
    // Normalize log level to uppercase just in case of inconsistent data
    const logLevel = log.level ? log.level.toUpperCase() : 'UNKNOWN';

    // The level must exist in our filter object AND be set to true.
    const levelMatch = logLevels.value.hasOwnProperty(logLevel) && logLevels.value[logLevel];

    const searchMatch = !searchText.value ||
      log.msg.toLowerCase().includes(searchText.value.toLowerCase()) ||
      (log.tag && log.tag.toLowerCase().includes(searchText.value.toLowerCase()));

    return levelMatch && searchMatch;
  });
});

/**
 * Exports the currently filtered logs to a file (JSON or TXT).
 * @param {string} format - The export format ('json' or 'txt').
 */
const exportLogs = (format = 'json') => {
  const logsToExport = filteredLogs.value;
  let data;
  let filename;

  if (format === 'json') {
    data = JSON.stringify(logsToExport, null, 2);
    filename = 'logs.json';
  } else {
    data = logsToExport.map(log => `[${log.localts}] [${log.level}] ${log.msg}`).join('\n');
    filename = 'logs.txt';
  }

  const blob = new Blob([data], {
    type: 'text/plain'
  });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = filename;
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
  URL.revokeObjectURL(url);
};


/**
 * Enhanced smooth scroll with requestAnimationFrame fallback for embedded contexts
 */
const smoothScrollToBottom = (duration = SMOOTH_SCROLL_DURATION) => {
  if (!scroller.value || filteredLogs.value.length === 0) return;

  const startTime = performance.now();
  const startScrollTop = scroller.value.$el.scrollTop;
  const targetScrollTop = scroller.value.$el.scrollHeight - scroller.value.$el.clientHeight;

  const scrollStep = (currentTime) => {
    const elapsed = currentTime - startTime;
    const progress = Math.min(elapsed / duration, 1);
    
    // Easing function for smooth animation
    const easeInOutQuad = (t) => t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
    
    const currentScrollTop = startScrollTop + (targetScrollTop - startScrollTop) * easeInOutQuad(progress);
    scroller.value.$el.scrollTop = currentScrollTop;
    
    if (progress < 1) {
      scrollAnimationFrame.value = requestAnimationFrame(scrollStep);
    } else {
      scrollAnimationFrame.value = null;
    }
  };
  
  // Cancel any existing animation
  if (scrollAnimationFrame.value) {
    cancelAnimationFrame(scrollAnimationFrame.value);
  }
  
  scrollAnimationFrame.value = requestAnimationFrame(scrollStep);
};

/**
 * Standard scroll to bottom with fallback for embedded contexts
 */
const scrollToBottom = () => {
  if (scroller.value && filteredLogs.value.length > 0) {
    if (isMobile.value) {
      // Use smooth scroll for mobile/touch devices
      smoothScrollToBottom();
    } else {
      // Use direct scroll for desktop with pointer devices
      scroller.value.scrollToItem(filteredLogs.value.length - 1);
    }
  }
};

/**
 * Enhanced scroll detection with edge margins and touch support
 */
const handleScroll = () => {
  if (!scroller.value) return;
  
  // Get the current scroll offset and viewable area
  const scrollOffset = scroller.value.$el.scrollTop;
  const scrollHeight = scroller.value.$el.scrollHeight;
  const clientHeight = scroller.value.$el.clientHeight;
  
  // Apply edge margin for mobile/touch devices (16px as requested)
  const edgeMargin = isMobile.value ? SCROLL_EDGE_MARGIN : 10;
  
  // Check if we're at the bottom (with edge margin for precision)
  const isAtBottom = scrollHeight - scrollOffset <= clientHeight + edgeMargin;
  userHasScrolled.value = !isAtBottom;
};

/**
 * Touch-based gesture detection for mobile devices
 */
const handleTouchStart = (event) => {
  if (!isMobile.value) return;
  touchStartY.value = event.touches[0].clientY;
  isScrolling.value = false;
};

const handleTouchMove = (event) => {
  if (!isMobile.value) return;
  touchEndY.value = event.touches[0].clientY;
  const touchDiff = touchStartY.value - touchEndY.value;
  
  // Detect scroll gesture (upward swipe)
  if (Math.abs(touchDiff) > 10) { // Minimum swipe threshold
    isScrolling.value = true;
    
    // If scrolling up, mark user as having scrolled
    if (touchDiff > 0) {
      userHasScrolled.value = true;
    }
  }
};

const handleTouchEnd = () => {
  if (!isMobile.value) return;
  
  // Reset scroll state after a delay
  setTimeout(() => {
    isScrolling.value = false;
  }, 150);
};

/**
 * Media query change handler
 */
const handleMediaQueryChange = (e) => {
  isMobile.value = e.matches;
  
  // Reinitialize scroll behavior when switching between mobile/desktop
  if (autoScroll.value && !userHasScrolled.value) {
    nextTick(() => {
      scrollToBottom();
    });
  }
};


// Watch for new logs and auto-scroll if enabled and user hasn't scrolled up.
watch(
  () => logs.value.length,
  () => {
    if (autoScroll.value && !userHasScrolled.value) {
      nextTick(scrollToBottom);
    }
  }
);

// If the user toggles auto-scroll on, scroll to the bottom.
watch(autoScroll, (newValue) => {
  if (newValue) {
    nextTick(scrollToBottom);
    userHasScrolled.value = false; // Reset user scroll state
  }
});


onMounted(() => {
  const savedLogLevels = localStorage.getItem('logviewer-levels');
  if (savedLogLevels) {
    logLevels.value = JSON.parse(savedLogLevels);
  }

  // Setup media query listener for mobile detection
  mobileMediaQuery.addListener(handleMediaQueryChange);
  isMobile.value = mobileMediaQuery.matches;

  // Initialize enhanced scroll handling
  initEnhancedScroll();

  // ----- WebSocket Handling (if needed) -----
  const ws = inject('websocket');
  if (ws) {
    const handleWebSocketMessage = (event) => {
      if (!realtime.value) return;
      try {
        let log = event.data;
        if (log && log.type === 'log') {
          log = {
            ...log,
            id: parseInt(Date.now() + (Math.random()*1000)),
            localts: new Date().toISOString(),
            expanded: false,
          };
          logs.value.push(log);
        }
      } catch (e) {
        console.error('Error handling WebSocket message:', e);
      }
    };
    ws.on(handleWebSocketMessage);
  }
});

/**
 * Initialize enhanced scroll handling for mobile/touch devices
 */
const initEnhancedScroll = () => {
  const scrollContainer = scroller.value?.$el;
  if (!scrollContainer) return;

  // Add CSS class for styling
  scrollContainer.classList.add('enhanced-scroll-container');

  // Setup touch event listeners for mobile devices
  if (isMobile.value) {
    scrollContainer.addEventListener('touchstart', handleTouchStart, { passive: true });
    scrollContainer.addEventListener('touchmove', handleTouchMove, { passive: true });
    scrollContainer.addEventListener('touchend', handleTouchEnd, { passive: true });
  }

  // Add CSS for edge margin visualization and smooth scrolling
  const style = document.createElement('style');
  style.textContent = `
    .enhanced-scroll-container {
      scroll-behavior: smooth;
      -webkit-overflow-scrolling: touch;
      padding: 0 ${SCROLL_EDGE_MARGIN}px;
      margin: 0 -${SCROLL_EDGE_MARGIN}px;
    }
    
    .enhanced-scroll-container::-webkit-scrollbar {
      width: 6px;
    }
    
    .enhanced-scroll-container::-webkit-scrollbar-track {
      background: rgba(0,0,0,0.1);
      border-radius: 3px;
    }
    
    .enhanced-scroll-container::-webkit-scrollbar-thumb {
      background: rgba(255,255,255,0.3);
      border-radius: 3px;
    }
    
    .enhanced-scroll-container::-webkit-scrollbar-thumb:hover {
      background: rgba(255,255,255,0.5);
    }
  `;
  document.head.appendChild(style);
};


onBeforeUnmount(() => {
  // Clean up the simulation interval when the component is destroyed.
  if (simulationInterval) {
    clearInterval(simulationInterval);
  }

  // Clean up media query listener
  mobileMediaQuery.removeListener(handleMediaQueryChange);

  // Clean up touch event listeners
  const scrollContainer = scroller.value?.$el;
  if (scrollContainer) {
    scrollContainer.removeEventListener('touchstart', handleTouchStart);
    scrollContainer.removeEventListener('touchmove', handleTouchMove);
    scrollContainer.removeEventListener('touchend', handleTouchEnd);
  }

  // Clean up animation frames
  if (scrollAnimationFrame.value) {
    cancelAnimationFrame(scrollAnimationFrame.value);
  }

  // Clean up dynamically added styles
  const styleElements = document.head.querySelectorAll('style');
  styleElements.forEach(style => {
    if (style.textContent.includes('enhanced-scroll-container')) {
      style.remove();
    }
  });
});
</script>