/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <soc/soc_memory_layout.h>
#include <esp_rmaker_utils.h>

#include <cbor.h>

static const char *TAG = "insight_cbor_dec";

#define CBOR_CHECK(a, str, goto_tag, ret_value, ...)                              \
    do {                                                                          \
        if ((a) != CborNoError) {                                                 \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret = ret_value;                                                      \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

static void dumpbytes(const uint8_t *buf, size_t len)
{
    while (len--) {
        printf("%02X ", *buf++);
    }
}

static void indent(int nestingLevel)
{
    printf("%*s", 2 * nestingLevel, "");
}

// extremely light implementation of stack needed for cbor dump
typedef struct cbor_dump_value {
    CborValue *it; // current iterator
    int nesting_level;
    CborType parent_type;
    int cnt; // count of elements already processed
} cbor_dump_value_t;

typedef struct cbor_dump_node cbor_dump_node_t;
typedef struct cbor_dump_node {
    cbor_dump_value_t *data;
    cbor_dump_node_t *next;
} cbor_dump_node_t;

static cbor_dump_node_t *dump_list_head = NULL;

static cbor_dump_value_t* cbor_dump_list_get_head()
{
    cbor_dump_value_t *value = NULL;
    if (dump_list_head) {
        cbor_dump_node_t *head = dump_list_head;
        dump_list_head = head->next;
        value = head->data;
        free(head);
    }
    return value;
}

static void cbor_dump_list_insert_at_head(cbor_dump_value_t* data)
{
    cbor_dump_node_t *node = MEM_ALLOC_EXTRAM(sizeof(cbor_dump_node_t));
    node->data = data;
    node->next = dump_list_head;
    dump_list_head = node;
}

/**
 * Decode CBOR data manually
 */
static CborError insights_dump_cbor_buffer(cbor_dump_value_t *value)
{
    if (!value) {
        return ESP_FAIL;
    }
    while (1) {
        CborError ret = CborNoError;
        int cnt = value->cnt;
        CborValue *it = value->it;
        int nestingLevel = value->nesting_level;
        CborType parent_type = value->parent_type;

        while (!cbor_value_at_end(it)) {
            CborType type = cbor_value_get_type(it);
            cnt++;

            if ((cnt % 2 == 0) || (parent_type == CborArrayType)) {
                if (cnt) {
                    puts(",");
                } else {
                    puts("");
                }
                indent(nestingLevel);
            } else {
                printf(" : ");
            }

            if (type == CborArrayType || type == CborMapType) {
                // push current node
                cbor_dump_value_t *value = MEM_ALLOC_EXTRAM(sizeof(cbor_dump_value_t));
                value->cnt = cnt;
                value->it = it;
                value->nesting_level = nestingLevel;
                value->parent_type = parent_type;
                cbor_dump_list_insert_at_head(value);

                CborValue *recursed = MEM_ALLOC_EXTRAM(sizeof(CborValue));
                if (false == cbor_value_is_container(it)) {
                    goto err;
                }
                ret = cbor_value_enter_container(it, recursed);
                CBOR_CHECK(ret, "enter container failed", err, ret);

                cnt = -1;
                nestingLevel += 1;
                parent_type = type;
                it = recursed;
                if (type == CborArrayType) {
                    printf("[");
                } else {
                    printf("{");
                }
                continue;
            }

            switch (type) {
            case CborArrayType:
            case CborMapType:
                continue;
            case CborIntegerType: {
                int64_t val;
                ret = cbor_value_get_int64(it, &val);
                CBOR_CHECK(ret, "parse int64 failed", err, ret);
                printf("%lld", (long long)val);
                break;
            }
            case CborByteStringType: {
                uint8_t *buf;
                size_t n;
                ret = cbor_value_dup_byte_string(it, &buf, &n, it);
                CBOR_CHECK(ret, "parse byte string failed", err, ret);
                dumpbytes(buf, n);
                free(buf);
                continue;
            }
            case CborTextStringType: {
                char *buf;
                size_t n;
                ret = cbor_value_dup_text_string(it, &buf, &n, it);
                CBOR_CHECK(ret, "parse text string failed", err, ret);
                printf("\"%s\"", buf);
                free(buf);
                continue;
            }
            case CborTagType: {
                CborTag tag;
                ret = cbor_value_get_tag(it, &tag);
                CBOR_CHECK(ret, "parse tag failed", err, ret);
                printf("Tag(%lld)\n", (long long)tag);
                break;
            }
            case CborSimpleType: {
                uint8_t type;
                ret = cbor_value_get_simple_type(it, &type);
                CBOR_CHECK(ret, "parse simple type failed", err, ret);
                printf("simple(%u)\n", type);
                break;
            }
            case CborNullType:
                printf("null");
                break;
            case CborUndefinedType:
                printf("undefined");
                break;
            case CborBooleanType: {
                bool val;
                ret = cbor_value_get_boolean(it, &val);
                CBOR_CHECK(ret, "parse boolean type failed", err, ret);
                printf(val ? "true" : "false");
                break;
            }
            case CborHalfFloatType: {
                uint16_t val;
                ret = cbor_value_get_half_float(it, &val);
                CBOR_CHECK(ret, "parse half float type failed", err, ret);
                printf("__f16(%04x)", val);
                break;
            }
            case CborFloatType: {
                float val;
                ret = cbor_value_get_float(it, &val);
                CBOR_CHECK(ret, "parse float type failed", err, ret);
                printf("%g", val);
                break;
            }
            case CborDoubleType: {
                double val;
                ret = cbor_value_get_double(it, &val);
                CBOR_CHECK(ret, "parse double float type failed", err, ret);
                printf("%g", val);
                break;
            }
            case CborInvalidType: {
                ret = CborErrorUnknownType;
                CBOR_CHECK(ret, "unknown cbor type", err, ret);
                break;
            }
            }
            ret = cbor_value_advance_fixed(it);
            CBOR_CHECK(ret, "fix value failed", err, ret);
        }

        printf("\n");
        cbor_dump_value_t *data = cbor_dump_list_get_head();
        if (data) {
            free(value); // free current
            value = data;
            CborType type = cbor_value_get_type(value->it);
            if (type == CborArrayType || type == CborMapType) {
                ret = cbor_value_leave_container(value->it, it);
                free(it);
                CBOR_CHECK(ret, "leave container failed", err, ret);
                indent(value->nesting_level);
                if (type == CborArrayType) {
                    printf("]");
                } else {
                    printf("}");
                }
            }
            continue;
        }
        ret = CborNoError;
err:
        // done
        printf("\n");
        if (value) {
            free(value->it);
            free(value);
            value = NULL;
        }

        // final cleanup
        value = cbor_dump_list_get_head();
        while (value) {
            free(value->it);
            free(value);
            value = cbor_dump_list_get_head();
        }
        dump_list_head = NULL;

        return ret;
    }
}

esp_err_t esp_insights_cbor_decode_dump(const uint8_t *buffer, int len)
{
    if (!buffer || len <= 0) {
        return ESP_ERR_INVALID_ARG;
    }
    CborParser root_parser;
    CborValue *it = MEM_ALLOC_EXTRAM(sizeof(CborValue));

    // Initialize the cbor parser and the value iterator
    cbor_parser_init(buffer, len, 0, &root_parser, it);

    cbor_dump_value_t *value = MEM_ALLOC_EXTRAM(sizeof(cbor_dump_value_t));
    value->cnt = -1;
    value->it = it;
    value->nesting_level = 0;
    value->parent_type = CborMapType;

    if (insights_dump_cbor_buffer(value) != CborNoError) {
        ESP_LOGI(TAG, "cbor dump failed");
    }

    return ESP_OK;
}
