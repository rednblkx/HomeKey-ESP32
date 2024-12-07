/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sdkconfig.h>
#include <string.h>
#include <inttypes.h>
#include <esp_log.h>
#include <esp_rmaker_cmd_resp.h>
#include <esp_rmaker_utils.h>

#define RMAKER_MAX_CMD  CONFIG_ESP_RMAKER_MAX_COMMANDS

static const char *TAG = "esp_rmaker_common_cmd_resp";

typedef struct {
    uint16_t cmd;
    uint8_t access;
    bool free_on_return;
    esp_rmaker_cmd_handler_t handler;
    void *priv;
} esp_rmaker_cmd_info_t;

typedef struct {
    uint8_t *bufptr;
    int bufsize;
    int curlen;
} esp_rmaker_tlv_data_t;

static esp_rmaker_cmd_info_t *esp_rmaker_cmd_list[RMAKER_MAX_CMD];

/* Get uint16 from Little Endian data buffer */
static uint16_t get_u16_le(const void *val_ptr)
{
    const uint8_t *p = (const uint8_t *) val_ptr;
    uint16_t val;

    val  = (uint16_t)p[0];
    val |= (uint16_t)p[1] << 8;

    return val;
}

/* Put uint16 in Little Endian ordering into data buffer */
static void put_u16_le(void *val_ptr, const uint16_t val)
{
    uint8_t *p = (uint8_t *) val_ptr;

    p[0] = (uint8_t)val & 0xff;
    p[1] = (uint8_t)(val >> 8) & 0xff;
}

/* Initialise TLV Data */
static void esp_rmaker_tlv_data_init(esp_rmaker_tlv_data_t *tlv_data, uint8_t *buf, int buf_size)
{
    tlv_data->bufptr = buf;
    tlv_data->bufsize = buf_size;
    tlv_data->curlen = 0;
}

/* Get length of data, for given type.
 *
 * Returns length of data on success and -1 if the TLV was not found
 */
static int esp_rmaker_get_tlv_length(const uint8_t *buf, int buflen, uint8_t type)
{
    if (!buf ) {
        return -1;
    }
    int curlen = 0;
    int val_len = 0;
    bool found = false;
    while (buflen > 0) {
        if (buf[curlen] == type) {
            uint8_t len = buf[curlen + 1];
            if ((buflen - len) < 2) {
                return -1;
            }
            val_len += len;
            if (len < 255) {
                return val_len;
            } else {
                found = true;
            }

        } else if (found) {
            return val_len;
        }

        /* buf[curlen +1] will give the Length */
        buflen -= (2 + buf[curlen + 1]);
        curlen += (2 + buf[curlen + 1]);
    }
    if (found) {
        return val_len;
    }
    return -1;
}

/* Get the value for the given type.
 *
 * Returns length of data on success and -1 if the TLV was not found
 */
static int esp_rmaker_get_value_from_tlv(const uint8_t *buf, int buflen, uint8_t type, void *val, int val_size)
{
    if (!buf || !val) {
        return -1;
    }
    int curlen = 0;
    int val_len = 0;
    bool found = false;
    while (buflen > 0) {
        if (buf[curlen] == type) {
            uint8_t len = buf[curlen + 1];
            if ((val_size < len) || ((buflen - len) < 2)) {
                return -1;
            }
            memcpy(val + val_len, &buf[curlen + 2], len);
            val_len += len;
            val_size -= len;
            if (len < 255) {
                return val_len;
            } else {
                found = true;
            }

        } else if (found) {
            return val_len;
        }

        /* buf[curlen +1] will give the Length */
        buflen -= (2 + buf[curlen + 1]);
        curlen += (2 + buf[curlen + 1]);
    }
    if (found) {
        return val_len;
    }
    return -1;
}

/* Add a TLV to the TLV buffer */
static int esp_rmaker_add_tlv(esp_rmaker_tlv_data_t *tlv_data, uint8_t type, int len, const void *val)
{
    if (!tlv_data->bufptr || ((len + 2) > (tlv_data->bufsize - tlv_data->curlen))) {
        return -1;
    }
    if (len > 0 && val == NULL) {
        return -1;
    }
    uint8_t *buf_ptr = (uint8_t *)val;
    int orig_len = tlv_data->curlen;
    do {
        tlv_data->bufptr[tlv_data->curlen++] = type;
        int tmp_len;
        if (len > 255) {
            tmp_len = 255;
        } else {
            tmp_len = len;
        }
        tlv_data->bufptr[tlv_data->curlen++] = tmp_len;
        memcpy(&tlv_data->bufptr[tlv_data->curlen], buf_ptr, tmp_len);
        tlv_data->curlen += tmp_len;
        buf_ptr += tmp_len;
        len -= tmp_len;
    } while (len);
    return tlv_data->curlen - orig_len;
}

/* Get user role string from flag. Useful for printing */
const char *esp_rmaker_get_user_role_string(uint8_t user_role)
{
    switch (user_role) {
    case ESP_RMAKER_USER_ROLE_SUPER_ADMIN:
        return "Admin";
    case ESP_RMAKER_USER_ROLE_PRIMARY_USER:
        return "Primary";
    case ESP_RMAKER_USER_ROLE_SECONDARY_USER:
        return "Secondary";
    default:
        return "Invalid Role";
    }
}

/* Prepare the response TLV8 which includes
 *
 * Request Id
 * Status
 * Command Id
 * Response Data
 */
static esp_err_t esp_rmaker_cmd_prepare_response(esp_rmaker_cmd_ctx_t *cmd_ctx, uint8_t status, void *response, size_t response_size, void **output, size_t *output_len)
{
    size_t publish_size = response_size + 100; /* +100 for rest of metadata. TODO: Do exact calculation */
    void *publish_data = MEM_CALLOC_EXTRAM(1, publish_size);
    if (!publish_data) {
        ESP_LOGE(TAG, "Failed to allocate buffer of size %d for response.", response_size + 100);
        return ESP_ERR_NO_MEM;
    }
    esp_rmaker_tlv_data_t tlv_data;
    esp_rmaker_tlv_data_init(&tlv_data, publish_data, publish_size);
    if (strlen(cmd_ctx->req_id)) {
        esp_rmaker_add_tlv(&tlv_data, ESP_RMAKER_TLV_TYPE_REQ_ID, strlen(cmd_ctx->req_id), cmd_ctx->req_id);
    }
    esp_rmaker_add_tlv(&tlv_data, ESP_RMAKER_TLV_TYPE_STATUS, sizeof(status), &status);
    uint8_t cmd_buf[2];
    put_u16_le(cmd_buf, cmd_ctx->cmd);
    esp_rmaker_add_tlv(&tlv_data, ESP_RMAKER_TLV_TYPE_CMD, sizeof(cmd_buf), cmd_buf);
    if (response != NULL && response_size != 0) {
        esp_rmaker_add_tlv(&tlv_data, ESP_RMAKER_TLV_TYPE_DATA, response_size, response);
    }
    ESP_LOGD(TAG, "Generated response of size %d for cmd %d", tlv_data.curlen, cmd_ctx->cmd);
    *output = publish_data;
    *output_len = tlv_data.curlen;
    return ESP_OK;
}

esp_err_t esp_rmaker_cmd_prepare_empty_response(void **output, size_t *output_len)
{
    size_t publish_size = 6; /* unit16 cmd = 0 (4 bytes in TLV), req_id = empty (2 bytes) */
    void *publish_data = MEM_CALLOC_EXTRAM(1, publish_size);
    if (!publish_data) {
        return ESP_ERR_NO_MEM;
    }
    esp_rmaker_tlv_data_t tlv_data;
    esp_rmaker_tlv_data_init(&tlv_data, publish_data, publish_size);
    esp_rmaker_add_tlv(&tlv_data, ESP_RMAKER_TLV_TYPE_REQ_ID, 0, NULL);
    uint8_t cmd_buf[2];
    put_u16_le(cmd_buf, 0);
    esp_rmaker_add_tlv(&tlv_data, ESP_RMAKER_TLV_TYPE_CMD, sizeof(cmd_buf), cmd_buf);
    *output = publish_data;
    *output_len = tlv_data.curlen;
    ESP_LOGD(TAG, "Generated empty response for requesting pending commands.");
    return ESP_OK;
}

/* Register a new command with its handler
 */
esp_err_t esp_rmaker_cmd_register(uint16_t cmd, uint8_t access, esp_rmaker_cmd_handler_t handler, bool free_on_return, void *priv)
{
    int i;
    for (i = 0; i < RMAKER_MAX_CMD; i++) {
        if (esp_rmaker_cmd_list[i] && (esp_rmaker_cmd_list[i]->cmd == cmd)) {
            ESP_LOGE(TAG, "Handler for command %d already exists.", cmd);
            return ESP_FAIL;
        }
    }
    for (i = 0; i < RMAKER_MAX_CMD; i++) {
        if (!esp_rmaker_cmd_list[i]) {
            esp_rmaker_cmd_info_t *cmd_info = calloc(1, sizeof(esp_rmaker_cmd_info_t));
            if (!cmd_info) {
                ESP_LOGE(TAG, "Could not allocate memory for cmd %d", cmd);
                return ESP_ERR_NO_MEM;
            }
            cmd_info->cmd = cmd;
            cmd_info->access = access;
            cmd_info->free_on_return = free_on_return;
            cmd_info->handler = handler;
            cmd_info->priv = priv;
            esp_rmaker_cmd_list[i] = cmd_info;
            ESP_LOGI(TAG, "Registered command %d", cmd);
            return ESP_OK;
        }
    }
    ESP_LOGE(TAG, "No space to add command %d", cmd);
    return ESP_ERR_NO_MEM;
}

/* Find the command infor for given command
 *
 * Returns pointer to the info if found and NULL on error
 */
static esp_rmaker_cmd_info_t *esp_rmaker_get_cmd_info(uint16_t cmd)
{
    int i;
    for (i = 0; i < RMAKER_MAX_CMD; i++) {
        if (esp_rmaker_cmd_list[i] && (esp_rmaker_cmd_list[i]->cmd == cmd)) {
            ESP_LOGI(TAG, "Handler found for command %d.", cmd);
            return esp_rmaker_cmd_list[i];
        }
    }
    ESP_LOGE(TAG, "No handler found for command %d.", cmd);
    return NULL;
}

/* De-register given command */
esp_err_t esp_rmaker_cmd_deregister(uint16_t cmd)
{
    int i;
    for (i = 0; i < RMAKER_MAX_CMD; i++) {
        if (esp_rmaker_cmd_list[i] && (esp_rmaker_cmd_list[i]->cmd == cmd)) {
            free(esp_rmaker_cmd_list[i]);
            esp_rmaker_cmd_list[i] = NULL;
            return ESP_OK;
        }
    }
    ESP_LOGE(TAG, "Cannot unregister command %d as it wasn't registered.", cmd);
    return ESP_ERR_INVALID_ARG;
}

/* Main command response handling function.
 *
 * It parses the rceived data to find the command and other metadata and
 * prepares the response to be sent
 */
esp_err_t esp_rmaker_cmd_response_handler(const void *input, size_t input_len, void **output, size_t *output_len)
{
    esp_rmaker_cmd_ctx_t cmd_ctx = {0};

    /* Read request id, user role and command, since these are mandatory fields */
    esp_rmaker_get_value_from_tlv(input, input_len, ESP_RMAKER_TLV_TYPE_REQ_ID, &cmd_ctx.req_id, sizeof(cmd_ctx.req_id));
    esp_rmaker_get_value_from_tlv(input, input_len, ESP_RMAKER_TLV_TYPE_USER_ROLE, &cmd_ctx.user_role, sizeof(cmd_ctx.user_role));
    uint8_t cmd_buf[2] = {0};
    esp_rmaker_get_value_from_tlv(input, input_len, ESP_RMAKER_TLV_TYPE_CMD, cmd_buf, sizeof(cmd_buf));
    cmd_ctx.cmd = get_u16_le(cmd_buf);

    if (strlen(cmd_ctx.req_id) == 0 || cmd_ctx.user_role == 0 || cmd_ctx.cmd == 0) {
        ESP_LOGE(TAG, "Request id, user role or command id cannot be 0");
        return esp_rmaker_cmd_prepare_response(&cmd_ctx, ESP_RMAKER_CMD_STATUS_CMD_INVALID, NULL, 0, output, output_len);
    }
    ESP_LOGI(TAG, "Got Req. Id: %s, Role = %s, Cmd = %d", cmd_ctx.req_id,
             esp_rmaker_get_user_role_string(cmd_ctx.user_role), cmd_ctx.cmd);

    /* Search for the command info and handle it if found */
    esp_rmaker_cmd_info_t *cmd_info = esp_rmaker_get_cmd_info(cmd_ctx.cmd);
    if (cmd_info) {
        if (cmd_info->access & cmd_ctx.user_role) {
            void *data = NULL;
            int data_size = esp_rmaker_get_tlv_length(input, input_len, ESP_RMAKER_TLV_TYPE_DATA);
            if (data_size > 0) {
                /* TODO: If data size < 255, can just use the pointer to input */
                data = MEM_CALLOC_EXTRAM(1, data_size);
                if (!data) {
                    ESP_LOGE(TAG, "Failed to allocate buffer of size %d for data.", data_size);
                    return ESP_ERR_NO_MEM;
                }
                esp_rmaker_get_value_from_tlv(input, input_len, ESP_RMAKER_TLV_TYPE_DATA, data, data_size);
            } else {
                /* It is not mandatory to have data for a given command. So, just throwing a warning */
                ESP_LOGW(TAG, "No data received for the command.");
                data_size = 0;
            }
            void *response;
            size_t response_size = 0;
            esp_err_t err = cmd_info->handler(data, data_size, &response, &response_size, &cmd_ctx, cmd_info->priv);
            if (err == ESP_OK) {
                err = esp_rmaker_cmd_prepare_response(&cmd_ctx, ESP_RMAKER_CMD_STATUS_SUCCESS, response, response_size, output, output_len);
            } else {
                err = esp_rmaker_cmd_prepare_response(&cmd_ctx, ESP_RMAKER_CMD_STATUS_FAILED, NULL, 0, output, output_len);
            }
            if (response && cmd_info->free_on_return) {
                ESP_LOGI(TAG, "Freeing response buffer.");
                free(response);
            }
            return err;
        } else {
            return esp_rmaker_cmd_prepare_response(&cmd_ctx, ESP_RMAKER_CMD_STATUS_AUTH_FAIL, NULL, 0, output, output_len);
        }
    }
    return esp_rmaker_cmd_prepare_response(&cmd_ctx, ESP_RMAKER_CMD_STATUS_NOT_FOUND, NULL, 0, output, output_len);
}

/****************************************** Testing Functions ******************************************/
static const char *cmd_status[] = {
    [ESP_RMAKER_CMD_STATUS_SUCCESS] = "Success",
    [ESP_RMAKER_CMD_STATUS_FAILED] = "Fail",
    [ESP_RMAKER_CMD_STATUS_CMD_INVALID] = "Invalid command data",
    [ESP_RMAKER_CMD_STATUS_AUTH_FAIL] = "Auth fail",
    [ESP_RMAKER_CMD_STATUS_NOT_FOUND] = "Command not found",
};

/* Send test command */
esp_err_t esp_rmaker_cmd_resp_test_send(const char *req_id, uint8_t role, uint16_t cmd, const void *data,
                                        size_t data_size, esp_rmaker_cmd_send_t cmd_send, void *priv_data)
{
    if (!cmd_send) {
        ESP_LOGE(TAG, "No callback to trigger the command.");
        return ESP_ERR_INVALID_ARG;
    }
    uint8_t cmd_data[200];
    esp_rmaker_tlv_data_t tlv_data;
    esp_rmaker_tlv_data_init(&tlv_data, cmd_data, sizeof(cmd_data));
    if (req_id) {
        esp_rmaker_add_tlv(&tlv_data, ESP_RMAKER_TLV_TYPE_REQ_ID, strlen(req_id), req_id);
    }
    esp_rmaker_add_tlv(&tlv_data, ESP_RMAKER_TLV_TYPE_USER_ROLE, sizeof(role), &role);
    uint8_t cmd_buf[2];
    put_u16_le(cmd_buf, cmd);
    esp_rmaker_add_tlv(&tlv_data, ESP_RMAKER_TLV_TYPE_CMD, sizeof(cmd_buf), cmd_buf);
    if (data != NULL && data_size != 0) {
        esp_rmaker_add_tlv(&tlv_data, ESP_RMAKER_TLV_TYPE_DATA, data_size, data);
    }
    ESP_LOGI(TAG, "Sending command of size %d for cmd %d", tlv_data.curlen, cmd);
    return cmd_send(cmd_data, tlv_data.curlen, priv_data);
}

/* Parse response */
esp_err_t esp_rmaker_cmd_resp_parse_response(const void *response, size_t response_len, void *priv_data)
{
    if (!response) {
        ESP_LOGE(TAG, "NULL response. Cannot parse.");
        return ESP_ERR_INVALID_ARG;
    }
    char req_id[REQ_ID_LEN] = {0};
    if (esp_rmaker_get_value_from_tlv(response, response_len, ESP_RMAKER_TLV_TYPE_REQ_ID, req_id, sizeof(req_id)) > 0) {
        ESP_LOGI(TAG, "RESP: Request Id: %s", req_id);
    }

    uint16_t cmd;
    uint8_t cmd_buf[2];
    if (esp_rmaker_get_value_from_tlv(response, response_len, ESP_RMAKER_TLV_TYPE_CMD, cmd_buf, sizeof(cmd_buf)) > 0) {
        cmd = get_u16_le(cmd_buf);
        ESP_LOGI(TAG, "RESP: Command: %" PRIu16, cmd);
    }

    uint8_t status;
    if (esp_rmaker_get_value_from_tlv(response, response_len, ESP_RMAKER_TLV_TYPE_STATUS, &status, sizeof(status)) > 0) {
        ESP_LOGI(TAG, "RESP: Status: %" PRIu8 ": %s", status, cmd_status[status]);
    }

    char resp_data[200];
    int resp_size = esp_rmaker_get_value_from_tlv(response, response_len, ESP_RMAKER_TLV_TYPE_DATA, resp_data, sizeof(resp_data));
    if (resp_size > 0) {
        resp_data[resp_size] = 0;
        ESP_LOGI(TAG, "RESP: Data: %s", resp_data);
    }
    return ESP_OK;
}
