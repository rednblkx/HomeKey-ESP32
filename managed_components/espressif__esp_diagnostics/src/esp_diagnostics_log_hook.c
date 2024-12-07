/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "stdio.h"
#include "string.h"
#include "esp_log.h"
#include "esp_diagnostics.h"
#include "soc/soc_memory_layout.h"
#include "esp_idf_version.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/* Onwards esp-idf v5.0 esp_cpu_process_stack_pc() is moved to
 * components/xtensa/include/esp_cpu_utils.h
 */
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    #if CONFIG_IDF_TARGET_ARCH_XTENSA
        #include "esp_cpu_utils.h"
    #else /* CONFIG_IDF_TARGET_ARCH_RISCV */
        #define esp_cpu_process_stack_pc(x) (x)   // dummy definition to avoid compilation error
    #endif
#else // For esp-idf version <= v4.4
    #include "soc/cpu.h"
#endif

#define IS_LOG_TYPE_ENABLED(type) (s_priv_data.init && (type & s_priv_data.enabled_log_type))

typedef struct {
    uint32_t enabled_log_type;
    esp_diag_log_config_t config;
    bool init;
} log_hook_priv_data_t;

static log_hook_priv_data_t s_priv_data;

#ifdef CONFIG_DIAG_LOG_MSG_ARG_FORMAT_TLV
typedef enum {
    MOD_NONE,   /* none */
    MOD_hh,     /* char */
    MOD_h,      /* short */
    MOD_l,      /* long, NOTE: in case of double, this can be ignored */
    MOD_ll,     /* long long */
    MOD_j,      /* intmax_t or uintmax_t */
    MOD_t,      /* ptrdiff_t */
    MOD_z,      /* size_t */
    MOD_L,      /* long double, NOTE: only for double */
} modifiers_t;

static esp_err_t append_arg(uint8_t *args, uint8_t *out_size, uint8_t max_len,
                            uint8_t type, uint8_t len, void *value)
{
    if ((*out_size + 2 + len) > max_len) {
        return ESP_ERR_NO_MEM;
    }

    args += *out_size;
    *args++ = type;
    *args++ = len;
    memcpy(args, value, len);
    *out_size += (len + 2);
    return ESP_OK;
}

static void get_tlv_from_ap(esp_diag_log_data_t *log, const char *format, va_list ap)
{
    const char *p = NULL;
    uint8_t len, out_size = 0;
    uint8_t arg_max_len = sizeof(log->msg_args);
    esp_err_t err = ESP_OK;
    esp_diag_arg_value_t arg_val;
    modifiers_t mf;

    /* there can be flags, field width digits, precision digits, modifiers, specifiers
     * modifier tells the size of the field eg: hh, h, l, ll
     * specifier tells whether it is signed, unsigned, double, string, pointer, etc.
     * Following parsing is done by considering printf manual (man 3 printf)
     */
    for (p = format; *p; p++) {
        if (*p == '%') {
            p++;
        } else {
            continue;
        }
        /* skip zero or more flags */
        while (*p == '#' || *p == '0' || *p == '-' || *p == ' ' || *p == '+' || *p == '\'') {
            p++;
        }
        /* skip the field width digits */
        while (*p >= '0' && *p <= '9') {
            p++;
        }
        /* skip precision bytes, period(.) followed by digits */
        if (*p == '.') {
            p++;
            while (*p >= '0' && *p <= '9') {
                p++;
            }
        }
        /* An optional length modifier, that specifies the size of the argument */
        len = 0;
        mf = MOD_NONE;
        while (*p) {
            bool _default = false;
            switch (*p) {
                case 'h':
                    if (mf == MOD_h) {
                        mf = MOD_hh;
                        len = sizeof(char);
                    } else {
                        mf = MOD_h;
                        len = sizeof(short);
                    }
                    break;
                case 'l':
                    if (mf == MOD_l) {
                        mf = MOD_ll;
                        len = sizeof(long long);
                    } else {
                        mf = MOD_l;
                        len = sizeof(long);
                    }
                    break;
                case 'j':
                    mf = MOD_j;
                    len = sizeof(intmax_t);
                    break;
                case 't':
                    mf = MOD_t;
                    len = sizeof(ptrdiff_t);
                    break;
                case 'z':
                    mf = MOD_z;
                    len = sizeof(size_t);
                    break;
                case 'L':
                    mf = MOD_L;
                    len = sizeof(long double);
                    break;
                default:
                    _default = true;
                    break;
            }
            if (_default) {
                break;
            }
            p++;
        }
        /* specifier, character that specifies the type of conversion to be applied */
        memset(&arg_val, 0, sizeof(arg_val));
        switch (*p) {
            case 'D': /* equivalent to ld */
                arg_val.l = va_arg(ap, long);
                err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_L, sizeof(long), &arg_val.l);
                break;
            case 'd':
            case 'i':
                switch (mf) {
                    case MOD_NONE: /* none, no modifier found */
                    case MOD_z: /* singed integer of size size_t */
                        arg_val.i = va_arg(ap, int);
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_INT, sizeof(int), &arg_val.i);
                        break;
                    case MOD_hh: /* char */
                        arg_val.c = va_arg(ap, int);    /* char is promoted to int */
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_CHAR, len, &arg_val.c);
                        break;
                    case MOD_h: /* short */
                        arg_val.s = va_arg(ap, int);    /* short is promoted to int */
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_SHORT, len, &arg_val.s);
                        break;
                    case MOD_l: /* long */
                        arg_val.l = va_arg(ap, long);
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_L, len, &arg_val.l);
                        break;
                        break;
                    case MOD_ll: /* long long */
                        arg_val.ll = va_arg(ap, long long);
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_LL, len, &arg_val.ll);
                        break;
                    case MOD_j:
                        /* intmax_t */
                        arg_val.imx = va_arg(ap, intmax_t);
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_INTMAX, len, &arg_val.imx);
                        break;
                    case MOD_t:
                        /* ptrdiff_t */
                        arg_val.ptrdiff = va_arg(ap, ptrdiff_t);
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_PTRDIFF, len, &arg_val.ptrdiff);
                        break;
                    default:
                        break;
                }
                break;
            case 'O':   /* equivalent to lo */
            case 'U':   /* equivalent to lu */
                arg_val.ul = va_arg(ap, unsigned long);
                err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_ULL, sizeof(unsigned long), &arg_val.ul);
                break;
            case 'o':
            case 'u':
            case 'x':
            case 'X':
            case 'p':
                switch (mf) {
                    case MOD_NONE:  /* none, no modifier found */
                    case MOD_t:     /* unsigned type of size ptrdiff_t */
                        len = sizeof(unsigned int);
                        arg_val.u = va_arg(ap, unsigned int);
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_UINT, len, &arg_val.u);
                        break;
                    case MOD_hh:    /* unsinged char */
                        arg_val.uc = va_arg(ap, unsigned int);
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_UCHAR, len, &arg_val.uc);
                        break;
                    case MOD_h: /* unsigned short */
                        arg_val.us = va_arg(ap, unsigned int);
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_USHORT, len, &arg_val.us);
                        break;
                    case MOD_l: /* unsigned long */
                        arg_val.ul = va_arg(ap, unsigned long);
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_UL, len, &arg_val.ul);
                        break;
                    case MOD_ll: /* unsigned long long */
                        arg_val.ull = va_arg(ap, unsigned long long);
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_ULL, len, &arg_val.ull);
                        break;
                    case MOD_j: /* uintmax_t */
                        arg_val.umx = va_arg(ap, uintmax_t);
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_UINTMAX, len, &arg_val.umx);
                        break;
                    case MOD_z: /* size_t */
                        arg_val.sz = va_arg(ap, size_t);
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_SIZE, len, &arg_val.sz);
                        break;
                    default:
                        break;
                }
                break;
            case 'a':
            case 'A':
            case 'e':
            case 'E':
            case 'f':
            case 'F':
            case 'g':
            case 'G':
                switch (mf) {
                    case MOD_NONE: /* double */
                    case MOD_l:    /* double */
                        len = sizeof(double);
                        arg_val.d = va_arg(ap, double);
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_DOUBLE, len, &arg_val.d);
                        break;
                    case MOD_L: /* long double */
                        arg_val.ld = va_arg(ap, long double);
                        err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_LDOUBLE, len, &arg_val.ld);
                        break;
                    default:
                        break;
                }
                break;
            case 'c': /* char */
                arg_val.c = va_arg(ap, int);
                err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_CHAR, sizeof(char), &arg_val.c);
                break;
            case 's': /* array of chars */
                arg_val.str = va_arg(ap, char *);
                if (arg_val.str) {
                    len = strlen(arg_val.str);
                } else {
                    len = 0;
                }
                err = append_arg(log->msg_args, &out_size, arg_max_len, ARG_TYPE_STR, len, arg_val.str);
                break;
            case 'n': /* %n outputs the number of bytes printed till that point, so will skip it */
                va_arg(ap, int);
                break;
            default:
                /* since we do not know the size or type of argument so, consuming the unsupported format specifier as integer. */
                va_arg(ap, int);
                break;
        }
        if (err != ESP_OK) {
            break;
        }
    }
    log->msg_args_len = out_size;
}
#endif /* CONFIG_DIAG_LOG_MSG_ARG_FORMAT_TLV */

static esp_err_t write_data(void *data, size_t len)
{
    if (s_priv_data.config.write_cb) {
        return s_priv_data.config.write_cb(data, len, s_priv_data.config.cb_arg);
    }
    return ESP_FAIL;
}

static esp_err_t diag_log_add(esp_diag_log_type_t type, uint32_t pc, const char *tag, const char *format, va_list args)
{
    esp_diag_log_data_t log;
    va_list ap;
    char *task_name = NULL;

    if (!IS_LOG_TYPE_ENABLED(type)) {
        return ESP_ERR_NOT_FOUND;
    }

    memset(&log, 0, sizeof(log));
    log.type = type;
    log.pc = pc;
    va_copy(ap, args);
    log.timestamp = esp_diag_timestamp_get();
    strlcpy(log.tag, tag, sizeof(log.tag));
    log.msg_ptr = (void *)format;
    log.msg_args_len = sizeof(log.msg_args);
#ifdef CONFIG_DIAG_LOG_MSG_ARG_FORMAT_TLV
    get_tlv_from_ap(&log, format, ap);
#else
    vsnprintf((char *)log.msg_args, log.msg_args_len, format, ap);
    log.msg_args_len = strlen((char *)log.msg_args);
#endif
    va_end(ap);
#if ESP_IDF_VERSION_MAJOR == 4 && ESP_IDF_VERSION_MINOR < 3
    task_name = pcTaskGetTaskName(NULL);
#else
    task_name = pcTaskGetName(NULL);
#endif
    if (task_name) {
        strlcpy(log.task_name, task_name, sizeof(log.task_name));
    }
    return write_data(&log, sizeof(log));
}

/**
 * If error logs are enabled via menuconfig, irrespective of if error logs are disabled
 * using `esp_log_level_set()`, error logs are still reported to Insights cloud
 */
static esp_err_t esp_diag_log_error(uint32_t pc, const char *tag, const char *format, va_list args)
{
    return diag_log_add(ESP_DIAG_LOG_TYPE_ERROR, pc, tag, format, args);
}

/**
 * If warning logs are enabled via menuconfig, irrespective of if warning logs are disabled
 * using `esp_log_level_set()`, warning logs are still reported to Insights cloud
 */
static esp_err_t esp_diag_log_warning(uint32_t pc, const char *tag, const char *format, va_list args)
{
    return diag_log_add(ESP_DIAG_LOG_TYPE_WARNING, pc, tag, format, args);
}

/**
 * Events are reported irrespective of device logging level.
 */
esp_err_t esp_diag_log_event(const char *tag, const char *format, ...)
{
    esp_err_t err;
    va_list args;
    uint32_t pc = esp_cpu_process_stack_pc((uint32_t)__builtin_return_address(0));
    va_start(args, format);
    err = diag_log_add(ESP_DIAG_LOG_TYPE_EVENT, pc, tag, format, args);
    va_end(args);
    return err;
}

void esp_diag_log_hook_enable(uint32_t type)
{
    s_priv_data.enabled_log_type |= type;
}

void esp_diag_log_hook_disable(uint32_t type)
{
    s_priv_data.enabled_log_type &= (~type);
}

static void esp_diag_log(esp_log_level_t level, uint32_t pc, const char *tag, const char *format, va_list list)
{
    if (level == ESP_LOG_ERROR) {
        esp_diag_log_error(pc, tag, format, list);
    } else if (level == ESP_LOG_WARN) {
        esp_diag_log_warning(pc, tag, format, list);
    }
}

esp_err_t esp_diag_log_hook_init(esp_diag_log_config_t *config)
{
    if (!config && !config->write_cb) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_priv_data.init) {
        return ESP_FAIL;
    }
    memcpy(&s_priv_data.config, config, sizeof(esp_diag_log_config_t));
    s_priv_data.init = true;
    return ESP_OK;
}

#ifdef CONFIG_LIB_BUILDER_COMPILE
extern int log_printfv(const char *format, va_list arg);

void __real_log_printf(const char *format, ...);

void __wrap_log_printf(const char *format, ...)
{
    va_list list;
    va_start(list, format);
    uint32_t pc = 0;
    pc = esp_cpu_process_stack_pc((uint32_t)__builtin_return_address(0));
    if (strlen(format) > 7 && format[6] == 'E') {
        esp_diag_log(ESP_LOG_ERROR, pc, "arduino-esp32", format, list);
    } else if (strlen(format) > 7 && format[6] == 'W') {
        esp_diag_log(ESP_LOG_WARN, pc, "arduino-esp32", format, list);
    }
    log_printfv(format, list);
    va_end(list);
}
#endif

void esp_diag_log_writev(esp_log_level_t level,
                         const char *tag,
                         const char *format,
                         va_list args)
{
#ifndef CONFIG_DIAG_LOG_DROP_WIFI_LOGS
    /* Only collect logs with "wifi" tag */
    if (strcmp(tag, "wifi") == 0) {
        uint32_t pc = 0;
        pc = esp_cpu_process_stack_pc((uint32_t)__builtin_return_address(0));
        esp_diag_log(level, pc, tag, format, args);
    }
#endif /* !CONFIG_DIAG_LOG_DROP_WIFI_LOGS */
}

void esp_diag_log_write(esp_log_level_t level,
                        const char *tag,
                        const char *format,
                        va_list list)
{
#ifndef BOOTLOADER_BUILD
    /* Logs with "wifi" tag, will be collected in esp_log_writev() */
    if (strcmp(tag, "wifi") != 0) {
        uint32_t pc = 0;
        pc = esp_cpu_process_stack_pc((uint32_t)__builtin_return_address(0));
        esp_diag_log(level, pc, tag, format, list);
    }
#endif
}

#if !CONFIG_DIAG_USE_EXTERNAL_LOG_WRAP
/* Wrapping esp_log_write() and esp_log_writev() reduces the
 * changes required in esp_log module to support diagnostics
 */
void __real_esp_log_writev(esp_log_level_t level,
                           const char *tag,
                           const char *format,
                           va_list args);

void __wrap_esp_log_writev(esp_log_level_t level,
                           const char *tag,
                           const char *format,
                           va_list args)
{
    esp_diag_log_write(level, tag, format, args);
    __real_esp_log_writev(level, tag, format, args);
}

void __wrap_esp_log_write(esp_log_level_t level,
                   const char *tag,
                   const char *format, ...)
{
    va_list list;
    va_start(list, format);
    esp_diag_log_writev(level, tag, format, list);
    esp_log_writev(level, tag, format, list);
    va_end(list);
}
#endif // CONFIG_DIAG_USE_EXTERNAL_LOG_WRAP
