// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <unity.h>
#include <rtc_store.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_random.h>

#define TAG              "diag_data_store_UT"
#define NVS_KEY_B1_CHARS "b1_chars"
#define NVS_KEY_B2_CHARS "b2_chars"
#define INSIGHTS_NVS_NAMESPACE  "nvs"
#define READ_DATA_SIZE  CONFIG_RTC_STORE_DATA_SIZE
static uint8_t data[READ_DATA_SIZE];

typedef struct {
    uint16_t alphabet;  /* Store the ascii of the character which is stored in buf */
    uint16_t len;       /* Length of buf */
    char buf[12];       /* Buffer to store data */
} test_data_t;

static void write_random_critical_data(uint32_t records, char *char_list)
{
    test_data_t data;
    uint32_t i = 0;
    for (i = 0; i < records; i++) {
        data.alphabet = char_list[i] = 'a' + (esp_random() % 26);
        data.len = sizeof(data.buf);
        memset(data.buf, data.alphabet, data.len);

        TEST_ASSERT(rtc_store_critical_data_write(&data, sizeof(data)) == ESP_OK);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

#ifdef CONFIG_DIAG_DATA_STORE_FLASH
    static uint32_t s_sha_off = 16; // flash data starts with 16 byte shasum
#else
    static uint32_t s_sha_off = 0;
#endif

static void validate_critical_data(const void *data, size_t len,
                                   uint32_t records, const char *char_list)
{
    uint32_t i, j;
    test_data_t _read_data;
    for (i = 0; i < records; i++) {
        data++; // skip meta_idx byte
        memcpy(&_read_data, data, sizeof(_read_data));
        TEST_ASSERT(_read_data.alphabet == char_list[i]);
        for (j = 0; j < sizeof(_read_data.buf); j++) {
            TEST_ASSERT(_read_data.buf[j] == char_list[i]);
        }
        data += sizeof(test_data_t);
    }
}

static void init_nvs_flash(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      err = nvs_flash_init();
    }
    TEST_ASSERT(err == ESP_OK);
}

TEST_CASE("data store init deinit", "[data-store]")
{
    init_nvs_flash();
    TEST_ASSERT(rtc_store_init() == ESP_OK);
    rtc_store_deinit();
    nvs_flash_deinit();
}

TEST_CASE("data store write", "[data-store]")
{
    uint32_t data = 0x1234;

    /* diag data store init */
    init_nvs_flash();
    assert(rtc_store_init() == ESP_OK);

    ESP_LOGI(TAG, "Write invalid arguments");
    assert(rtc_store_critical_data_write(NULL, 1000) == ESP_ERR_INVALID_ARG);
    assert(rtc_store_critical_data_write(NULL, 0) == ESP_ERR_INVALID_ARG);
    assert(rtc_store_critical_data_write(&data, 0) == ESP_ERR_INVALID_ARG);

    ESP_LOGI(TAG, "Write 100KB to test no memory error");
    assert(rtc_store_critical_data_write(&data, 1024 * 100) == ESP_FAIL);

    /* data store deinit */
    rtc_store_deinit();
    nvs_flash_deinit();
}

TEST_CASE("data store write read release_all", "[data-store]")
{
    size_t len = 0;
    uint32_t count = 10;
    char char_list[count];

    /* diag data store init */
    init_nvs_flash();
    assert(rtc_store_init() == ESP_OK);

    /* Write critical data */
    write_random_critical_data(count, char_list);

    /* Read critical data and validate */
    len = rtc_store_critical_data_read(data, READ_DATA_SIZE);
    TEST_ASSERT(((len - count) == (count * sizeof(test_data_t) + s_sha_off)));
    validate_critical_data(data + s_sha_off, len - s_sha_off, count, char_list);

    /* Release all the data */
    TEST_ASSERT(rtc_store_critical_data_release(len) == ESP_OK);

    /* Read again, should return NULL and zero length */
    len = 0;
    len = rtc_store_critical_data_read(data, READ_DATA_SIZE);
    TEST_ASSERT((len == 0));

    /* Data store deinit */
    rtc_store_deinit();
    nvs_flash_deinit();
}

TEST_CASE("data store wrapped_read write_till_exact_full", "[data-store]")
{
    size_t len = 0;
    uint32_t count = 15;
    char char_list[count];

    /* diag data store init */
    init_nvs_flash();
    assert(rtc_store_init() == ESP_OK);

    // fill the buffer completely
    memset(data, 0, CONFIG_RTC_STORE_CRITICAL_DATA_SIZE);
    rtc_store_critical_data_write(data, CONFIG_RTC_STORE_CRITICAL_DATA_SIZE - 1);
    len = rtc_store_critical_data_read(data, READ_DATA_SIZE);
    // actual data written was 1 byte more than length provided
    TEST_ASSERT((len == CONFIG_RTC_STORE_CRITICAL_DATA_SIZE));
    /* Release all data */
    TEST_ASSERT(rtc_store_critical_data_release(len) == ESP_OK);

    // fill half the buffer, (this also makes sure if we are cool with prev edge case)
    memset(data, 0, CONFIG_RTC_STORE_CRITICAL_DATA_SIZE);
    rtc_store_critical_data_write(data, CONFIG_RTC_STORE_CRITICAL_DATA_SIZE - 4);
    len = rtc_store_critical_data_read(data, READ_DATA_SIZE);
    // actual data written was 1 byte more than length provided
    TEST_ASSERT((len == CONFIG_RTC_STORE_CRITICAL_DATA_SIZE - 4 + 1));
    /* Release all data */
    TEST_ASSERT(rtc_store_critical_data_release(len) == ESP_OK);

    /* Write critical data: wrap-around read test */
    write_random_critical_data(count, char_list);

    /* Read critical data and validate */
    len = rtc_store_critical_data_read(data, READ_DATA_SIZE);
    TEST_ASSERT((data != NULL) && ((len - count) == (count * sizeof(test_data_t) + s_sha_off)));
    validate_critical_data(data + s_sha_off, len - s_sha_off, count, char_list);

    vTaskDelay(pdMS_TO_TICKS(10));

    /* Release all data */
    TEST_ASSERT(rtc_store_critical_data_release(len) == ESP_OK);

    /* data store deinit */
    rtc_store_deinit();
    nvs_flash_deinit();
}

TEST_CASE("data store write read release_zero read release_zero release_all", "[data-store]")
{
    size_t len = 0;
    uint32_t count = 15;
    char char_list[count];

    /* diag data store init */
    init_nvs_flash();
    assert(rtc_store_init() == ESP_OK);

    /* Write critical data */
    write_random_critical_data(count, char_list);

    /* Read critical data and validate */
    len = rtc_store_critical_data_read(data, READ_DATA_SIZE);
    TEST_ASSERT((data != NULL) && ((len - count) == (count * sizeof(test_data_t) + s_sha_off)));
    validate_critical_data(data + s_sha_off, len - s_sha_off, count, char_list);

    /* Read critical data and validate again */
    len = 0;
    len = rtc_store_critical_data_read(data, READ_DATA_SIZE);
    TEST_ASSERT((data != NULL) && ((len - count) == (count * sizeof(test_data_t) + s_sha_off)));
    validate_critical_data(data + s_sha_off, len - s_sha_off, count, char_list);

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    /* Release all data */
    TEST_ASSERT(rtc_store_critical_data_release(len) == ESP_OK);

    /* data store deinit */
    rtc_store_deinit();
    nvs_flash_deinit();
}

static char *nvs_read_chars(size_t *len, uint32_t bank)
{
    nvs_handle_t handle;
    const char *key = (bank == 1) ? NVS_KEY_B1_CHARS : NVS_KEY_B2_CHARS ;
    TEST_ASSERT(nvs_open(INSIGHTS_NVS_NAMESPACE, NVS_READWRITE, &handle) == ESP_OK);
    TEST_ASSERT(nvs_get_blob(handle, key, NULL, len) == ESP_OK);

    char *chars = calloc(1, *len);
    TEST_ASSERT(chars != NULL);
    TEST_ASSERT(nvs_get_blob(handle, key, chars, len) == ESP_OK);
    nvs_close(handle);
    return chars;
}

static void nvs_write_chars(char *chars, size_t len, uint32_t bank)
{
    nvs_handle_t handle;
    const char *key = (bank == 1) ? NVS_KEY_B1_CHARS : NVS_KEY_B2_CHARS ;
    TEST_ASSERT(nvs_open(INSIGHTS_NVS_NAMESPACE, NVS_READWRITE, &handle) == ESP_OK);
    TEST_ASSERT(nvs_set_blob(handle, key, chars, len) == ESP_OK);
    nvs_commit(handle);
    nvs_close(handle);
}

static void write_critical_data_and_reset(uint32_t bank)
{
    uint32_t count = 8;
    char char_list[count];

    /* diag data store init */
    init_nvs_flash();
    TEST_ASSERT(rtc_store_init() == ESP_OK);

    /* Write critical data records to bank 1 */
    write_random_critical_data(count, char_list);
    nvs_write_chars(char_list, count, bank);

    /* diag data store deinit */
    rtc_store_deinit();

#if CONFIG_DIAG_DATA_STORE_FLASH
    /* reset the device */
    ESP_LOGW(TAG, "Resetting the device");
    esp_restart();
#else
    /* Using RTC store, crash the device, and check data after crash */
    ESP_LOGW(TAG, "Crashing intentionally");
    *(int *)10 = 0;
#endif
}

static void read_critical_data(uint32_t bank)
{
    size_t count = 0;
    char *char_list = NULL;
    size_t len = 0;

    /* diag data store init */
    init_nvs_flash();
    TEST_ASSERT(rtc_store_init() == ESP_OK);

    /* read saved char list from nvs */
    char_list = nvs_read_chars(&count, bank);
    TEST_ASSERT(char_list != NULL && count != 0);

    /* read data and validate */
    len = rtc_store_critical_data_read(data, READ_DATA_SIZE);
    TEST_ASSERT(((len - count) == (count * sizeof(test_data_t) + s_sha_off)));
    validate_critical_data(data + s_sha_off, len - s_sha_off, count, char_list);
    free(char_list);

    /* Release all data */
    TEST_ASSERT(rtc_store_critical_data_release(len) == ESP_OK);

    /* diag data store deinit */
    rtc_store_deinit();
    nvs_flash_deinit();
}

#if CONFIG_DIAG_DATA_STORE_FLASH
static void write_critical_data_in_b1_and_reset(void)
{
    write_critical_data_and_reset(1);
}

static void write_critical_data_in_b2_and_reset(void)
{
    write_critical_data_and_reset(2);
}

static void read_critical_data_in_b1(void)
{
    read_critical_data(1);
}

static void read_critical_data_in_b2(void)
{
    read_critical_data(2);
}

static void write_critical_data_in_b1_b2_and_reset(void)
{
    uint32_t count_1 = 9;
    uint32_t count_2 = 11;
    char char_list_1[count_1];
    char char_list_2[count_2];
    size_t len = 0;

    /* diag data store init */
    init_nvs_flash();
    TEST_ASSERT(rtc_store_init() == ESP_OK);

    /* Write critical data records to bank 1 */
    write_random_critical_data(count_1, char_list_1);
    nvs_write_chars(char_list_1, count_1, 1);

    /* Read data, validate and release zero bytes */
    len = rtc_store_critical_data_read(data, READ_DATA_SIZE);
    TEST_ASSERT(((len + count_1) == (count_1 * sizeof(test_data_t) + s_sha_off)));
    validate_critical_data(data + s_sha_off, len - s_sha_off, count_1, char_list_1);

    /* Write critical data records to bank 2 */
    write_random_critical_data(count_2, char_list_2);
    nvs_write_chars(char_list_2, count_2, 2);

    /* diag data store deinit */
    rtc_store_deinit();
    nvs_flash_deinit();

    /* reset the device */
    esp_restart();
}

static void read_stale_critical_data_in_b1_b2(void)
{
    size_t count_1 = 0;
    size_t count_2 = 0;
    char *char_list_1 = NULL;
    char *char_list_2 = NULL;
    size_t len = 0;

    /* diag data store init */
    init_nvs_flash();
    TEST_ASSERT(rtc_store_init() == ESP_OK);

    /* read saved char list for bank_1 from nvs */
    char_list_1 = nvs_read_chars(&count_1, 1);
    TEST_ASSERT(char_list_1 != NULL && count_1 != 0);

    /* read data from bank_1 and validate and release all */
    len = rtc_store_critical_data_read(data, READ_DATA_SIZE);
    TEST_ASSERT(((len + count_1) == (count_1 * sizeof(test_data_t) + s_sha_off)));
    validate_critical_data(data + s_sha_off, len - s_sha_off, count_1, char_list_1);
    free(char_list_1);
    TEST_ASSERT(rtc_store_critical_data_release(len) == ESP_OK);

    /* read saved char list for bank_2 from nvs */
    char_list_2 = nvs_read_chars(&count_2, 2);
    TEST_ASSERT(char_list_2 != NULL && count_2 != 0);

    /* read data from bank_1 and validate and release all */
    len = rtc_store_critical_data_read(data, READ_DATA_SIZE);
    TEST_ASSERT(((len + count_2) == (count_2 * sizeof(test_data_t) + s_sha_off)));
    validate_critical_data(data + s_sha_off, len - s_sha_off, count_2, char_list_1);
    free(char_list_2);
    TEST_ASSERT(rtc_store_critical_data_release(len) == ESP_OK);

    /* diag data store deinit */
    rtc_store_deinit();
    nvs_flash_deinit();
}

TEST_CASE_MULTIPLE_STAGES("data store validate data in bank_1 after reset", "[data-store-flash]",
                          write_critical_data_in_b1_and_reset, read_critical_data_in_b1);

TEST_CASE_MULTIPLE_STAGES("data store validate data in bank_2 after reset", "[data-store-flash]",
                          write_critical_data_in_b2_and_reset, read_critical_data_in_b2);

TEST_CASE_MULTIPLE_STAGES("data store validate data in bank_1_2 after reset", "[data-store-flash]",
                          write_critical_data_in_b1_b2_and_reset, read_stale_critical_data_in_b1_b2);

#else /* CONFIG_DIAG_DATA_STORE_RTC */

static void write_critical_data_in_rtc_and_reset(void)
{
    write_critical_data_and_reset(1);
}

static void read_critical_data_in_rtc(void)
{
    read_critical_data(1);
}

TEST_CASE_MULTIPLE_STAGES("data store validate data in RTC after crash", "[data-store-rtc]",
                          write_critical_data_in_rtc_and_reset, read_critical_data_in_rtc);
#endif /* CONFIG_DIAG_DATA_STORE_FLASH */
