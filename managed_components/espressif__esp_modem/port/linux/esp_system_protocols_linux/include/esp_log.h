//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdio.h>

#define ESP_LOG_INFO 1
#define ESP_LOG_BUFFER_HEXDUMP(...)

#define LOG_COLOR_BLACK   "30"
#define LOG_COLOR_RED     "31"
#define LOG_COLOR_GREEN   "32"
#define LOG_COLOR_BROWN   "33"
#define LOG_COLOR_BLUE    "34"
#define LOG_COLOR_PURPLE  "35"
#define LOG_COLOR_CYAN    "36"
#define LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define LOG_BOLD(COLOR)   "\033[1;" COLOR "m"
#define LOG_RESET_COLOR   "\033[0m"
#define LOG_COLOR_E       LOG_COLOR(LOG_COLOR_RED) "E"
#define LOG_COLOR_W       LOG_COLOR(LOG_COLOR_BROWN) "W"
#define LOG_COLOR_I       LOG_COLOR(LOG_COLOR_GREEN) "I"
#define LOG_COLOR_D       LOG_COLOR(LOG_COLOR_CYAN) "D"
#define LOG_COLOR_V       "V"

#define ESP_LOGE(TAG, ...)  do { \
printf(LOG_COLOR_E); printf("(%s) ", TAG); printf(__VA_ARGS__); printf(LOG_RESET_COLOR "\n"); } while(0)
#define ESP_LOGW(TAG, ...)  do { \
printf(LOG_COLOR_W); printf("(%s) ", TAG); printf(__VA_ARGS__); printf(LOG_RESET_COLOR "\n"); } while(0)
#define ESP_LOGI(TAG, ...)  do { \
printf(LOG_COLOR_I); printf("(%s) ", TAG); printf(__VA_ARGS__); printf(LOG_RESET_COLOR "\n"); } while(0)
#define ESP_LOGD(TAG, ...)  do { \
printf(LOG_COLOR_D); printf("(%s) ", TAG); printf(__VA_ARGS__); printf(LOG_RESET_COLOR "\n"); } while(0)
#define ESP_LOGV(TAG, ...)  do { \
printf(LOG_COLOR_V); printf("(%s) ", TAG); printf(__VA_ARGS__); printf(LOG_RESET_COLOR "\n"); } while(0)
