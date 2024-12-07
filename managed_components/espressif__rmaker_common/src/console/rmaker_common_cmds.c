/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_timer.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include <esp_console.h>
#include <lwip/sockets.h>
#include <esp_system.h>
#include <argtable3/argtable3.h>
#include <esp_heap_caps.h>
#ifdef CONFIG_HEAP_TRACING
#include <esp_heap_trace.h>
#endif

#include <esp_rmaker_utils.h>


#include "rmaker_console_internal.h"

static const char *TAG = "esp_rmaker_commands";

static int reboot_cli_handler(int argc, char *argv[])
{
    /* Just to go to the next line */
    printf("\n");
    esp_restart();
    return 0;
}

static int up_time_cli_handler(int argc, char *argv[])
{
    printf("%s: Uptime of the device: %lld milliseconds\n", TAG, esp_timer_get_time() / 1000);
    return 0;
}

static int task_dump_cli_handler(int argc, char *argv[])
{
#ifndef CONFIG_FREERTOS_USE_TRACE_FACILITY
    printf("%s: To use this utility enable: Component config --> FreeRTOS --> Enable FreeRTOS trace facility\n", TAG);
#else
    int num_of_tasks = uxTaskGetNumberOfTasks();
    TaskStatus_t *task_array = MEM_CALLOC_EXTRAM(num_of_tasks, sizeof(TaskStatus_t));
    if (!task_array) {
        ESP_LOGE(TAG, "Memory allocation for task list failed.");
        return -1;
    }
    num_of_tasks = uxTaskGetSystemState(task_array, num_of_tasks, NULL);
    printf("%s: \tName\tNumber\tPriority\tStackWaterMark\n", TAG);
    for (int i = 0; i < num_of_tasks; i++) {
        printf("%16s\t%d\t%d\t%" PRIu32 "\n",
               task_array[i].pcTaskName,
               task_array[i].xTaskNumber,
               task_array[i].uxCurrentPriority,
               task_array[i].usStackHighWaterMark);
    }
    free(task_array);
#endif
    return 0;
}

static int cpu_dump_cli_handler(int argc, char *argv[])
{
#ifndef CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS
    printf("%s: To use this utility enable: Component config --> FreeRTOS --> Enable FreeRTOS to collect run time stats\n", TAG);
#else
    char *buf = MEM_CALLOC_EXTRAM(1, 2 * 1024);
    if (!buf) {
        ESP_LOGE(TAG, "Memory allocation for cpu dump failed.");
        return -1;
    }
    vTaskGetRunTimeStats(buf);
    printf("%s: Run Time Stats:\n%s\n", TAG, buf);
    free(buf);
#endif
    return 0;
}

static int mem_dump_cli_handler(int argc, char *argv[])
{
    printf("\tDescription\tInternal\tSPIRAM\n");
    printf("Current Free Memory\t%d\t\t%d\n",
           heap_caps_get_free_size(MALLOC_CAP_8BIT) - heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
           heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    printf("Largest Free Block\t%d\t\t%d\n",
           heap_caps_get_largest_free_block(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL),
           heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
    printf("Min. Ever Free Size\t%d\t\t%d\n",
           heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL),
           heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM));
    return 0;
}

static int sock_dump_cli_handler(int argc, char *argv[])
{
#if LWIP_IPV4
    int i, ret, used_sockets = 0;

    struct sockaddr_in local_sock, peer_sock;
    socklen_t local_sock_len = sizeof(struct sockaddr_in), peer_sock_len = sizeof(struct sockaddr_in);
    char local_ip_addr[16], peer_ip_addr[16];
    unsigned int local_port, peer_port;

    int sock_type;
    socklen_t sock_type_len;

#define TOTAL_NUM_SOCKETS MEMP_NUM_NETCONN
    printf("sock_fd\tprotocol\tlocal_addr\t\tpeer_addr\n");
    for (i = LWIP_SOCKET_OFFSET; i < LWIP_SOCKET_OFFSET + TOTAL_NUM_SOCKETS; i++) {
        memset(&local_sock, 0, sizeof(struct sockaddr_in));
        memset(&peer_sock, 0, sizeof(struct sockaddr_in));
        local_sock_len = sizeof(struct sockaddr);
        peer_sock_len = sizeof(struct sockaddr);
        memset(local_ip_addr, 0, sizeof(local_ip_addr));
        memset(peer_ip_addr, 0, sizeof(peer_ip_addr));
        local_port = 0;
        peer_port = 0;
        sock_type = 0;
        sock_type_len = sizeof(int);

        ret = getsockname(i, (struct sockaddr *)&local_sock, &local_sock_len);
        if (ret >= 0) {
            used_sockets++;
            inet_ntop(AF_INET, &local_sock.sin_addr, local_ip_addr, sizeof(local_ip_addr));
            local_port = ntohs(local_sock.sin_port);
            getsockopt(i, SOL_SOCKET, SO_TYPE, &sock_type, &sock_type_len);
            printf("%d\t%d:%s\t%16s:%d", i, sock_type, sock_type == SOCK_STREAM ? "tcp" : sock_type == SOCK_DGRAM ? "udp" : "raw", local_ip_addr, local_port);

            ret = getpeername(i, (struct sockaddr *)&peer_sock, &peer_sock_len);
            if (ret >= 0) {
                inet_ntop(AF_INET, &peer_sock.sin_addr, peer_ip_addr, sizeof(peer_ip_addr));
                peer_port = ntohs(peer_sock.sin_port);
                printf("\t%16s:%d", peer_ip_addr, peer_port);
            }
            printf("\n");
        }
    }
    printf("Remaining sockets: %d\n", TOTAL_NUM_SOCKETS - used_sockets);
#else
    printf("%s: To use this utility enable: Component config --> LWIP --> Enable IPv4 \n", TAG);
#endif /* LWIP_IPV4 */
    return 0;
}

#ifdef CONFIG_HEAP_TRACING
static int heap_trace_records;
static heap_trace_record_t *heap_trace_records_buf;
static int cli_heap_trace_start()
{
    if (!heap_trace_records_buf) {
        // this buffer is required to be in internal
        heap_trace_records_buf = malloc(heap_trace_records * sizeof(heap_trace_record_t));
        if (!heap_trace_records_buf) {
            printf("%s: Failed to allocate records buffer\n", TAG);
            return -1;
        }
        if (heap_trace_init_standalone(heap_trace_records_buf, heap_trace_records) != ESP_OK) {
            printf("%s: Failed to initialise tracing\n", TAG);
            goto error1;
        }
    }
    if (heap_trace_start(HEAP_TRACE_LEAKS) != ESP_OK) {
        printf("%s: Failed to start heap trace\n", TAG);
        goto error2;
    }
    return 0;
error2:
    heap_trace_init_standalone(NULL, 0);
error1:
    free(heap_trace_records_buf);
    heap_trace_records_buf = NULL;
    return -1;
}

static int cli_heap_trace_stop()
{
    if (!heap_trace_records_buf) {
        printf("%s: Tracing not started?\n", TAG);
        return 0;
    }
    heap_trace_stop();
    heap_trace_dump();
    heap_trace_init_standalone(NULL, 0);
    free(heap_trace_records_buf);
    heap_trace_records_buf = NULL;
    return 0;
}
#endif

static int heap_trace_cli_handler(int argc, char *argv[])
{
    int ret = 0;
#ifndef CONFIG_HEAP_TRACING
    printf("%s: To use this utility enable: Component config --> Heap memory debugging --> Enable heap tracing\n", TAG);
#else
    if (argc < 2) {
        printf("%s: Incorrect arguments\n", TAG);
        return -1;
    }
    if (strcmp(argv[1], "start") == 0) {
#define DEFAULT_HEAP_TRACE_RECORDS 200
        if (argc != 3) {
            heap_trace_records = DEFAULT_HEAP_TRACE_RECORDS;
        } else {
            heap_trace_records = atoi(argv[2]);
        }
        printf("%s: Using a buffer to trace %d records\n", TAG, heap_trace_records);
        ret = cli_heap_trace_start();
    } else if (strcmp(argv[1], "stop") == 0) {
        ret = cli_heap_trace_stop();
    } else {
        printf("%s: Invalid argument:%s:\n", TAG, argv[1]);
        ret = -1;
    }
#endif
    return ret;
}

static int register_generic_debug_commands()
{
    const esp_console_cmd_t debug_commands[] = {
        {
            .command = "reboot",
            .help = "",
            .func = reboot_cli_handler,
        },
        {
            .command = "up-time",
            .help = "Get the device up time in milliseconds.",
            .func = up_time_cli_handler,
        },
        {
            .command = "mem-dump",
            .help = "Get the available memory.",
            .func = mem_dump_cli_handler,
        },
        {
            .command = "task-dump",
            .help = "Get the list of all the running tasks.",
            .func = task_dump_cli_handler,
        },
        {
            .command = "cpu-dump",
            .help = "Get the CPU utilisation by all the runninng tasks.",
            .func = cpu_dump_cli_handler,
        },
        {
            .command = "sock-dump",
            .help = "Get the list of all the active sockets.",
            .func = sock_dump_cli_handler,
        },
        {
            .command = "heap-trace",
            .help = "Start or stop heap tracing. Usage: heap-trace <start|stop> <bufer_size>",
            .func = heap_trace_cli_handler,
        },
    };

    int cmds_num = sizeof(debug_commands) / sizeof(esp_console_cmd_t);
    int i;
    for (i = 0; i < cmds_num; i++) {
        ESP_LOGI(TAG, "Registering command: %s", debug_commands[i].command);
        esp_console_cmd_register(&debug_commands[i]);
    }
    return 0;
}

static int reset_to_factory_handler(int argc, char** argv)
{
#define RESET_DELAY         2
#define REBOOT_DELAY        2
    printf("%s: Resetting to Factory Defaults...\n", TAG);
    esp_rmaker_factory_reset(RESET_DELAY, REBOOT_DELAY);
    return ESP_OK;
}

static void register_reset_to_factory()
{
    const esp_console_cmd_t cmd = {
        .command = "reset-to-factory",
        .help = "Reset the board to factory defaults",
        .func = &reset_to_factory_handler,
    };
    ESP_LOGI(TAG, "Registering command: %s", cmd.command);
    esp_console_cmd_register(&cmd);
}

static int local_time_cli_handler(int argc, char *argv[])
{
    char local_time[64];
    if (esp_rmaker_get_local_time_str(local_time, sizeof(local_time)) == ESP_OK) {
        printf("%s: Current local time: %s\n", TAG, local_time);
    } else {
        printf("%s: Current local time (truncated): %s\n", TAG, local_time);
    }
    return ESP_OK;
}

static int tz_set_cli_handler(int argc, char *argv[])
{
    if (argc < 2) {
        printf("%s: Invalid Usage.\n", TAG);
        return ESP_ERR_INVALID_ARG;
    }
    if (strcmp(argv[1], "posix") == 0) {
        if (argv[2]) {
            esp_rmaker_time_set_timezone_posix(argv[2]);
        } else {
            printf("%s: Invalid Usage.\n", TAG);
            return ESP_ERR_INVALID_ARG;
        }
    } else {
        esp_rmaker_time_set_timezone(argv[1]);
    }
    return ESP_OK;
}

static void register_time_commands()
{
    const esp_console_cmd_t local_time_cmd = {
        .command = "local-time",
        .help = "Get the local time of device.",
        .func = &local_time_cli_handler,
    };
    ESP_LOGI(TAG, "Registering command: %s", local_time_cmd.command);
    esp_console_cmd_register(&local_time_cmd);

    const esp_console_cmd_t tz_set_cmd = {
        .command = "tz-set",
        .help = "Set Timezone. Usage: tz-set [posix] <tz_string>.",
        .func = &tz_set_cli_handler,
    };
    ESP_LOGI(TAG, "Registering command: %s", tz_set_cmd.command);
    esp_console_cmd_register(&tz_set_cmd);
}

void esp_rmaker_common_register_commands()
{
    static bool registered = false;
    if (registered) {
        ESP_LOGI(TAG, "Skipping already registered commands");
        return;
    }
    register_generic_debug_commands();
    register_reset_to_factory();
    register_time_commands();
    registered = true;
}
