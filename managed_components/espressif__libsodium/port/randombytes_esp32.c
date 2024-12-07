/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#if __has_include("esp_random.h")
#include "esp_random.h"
#else
#include "esp_system.h"
#endif
#include "randombytes_internal.h"

static const char *randombytes_esp32xx_implementation_name(void)
{
    return CONFIG_IDF_TARGET;
}

/*
  Plug the ESP32 hardware RNG into libsodium's custom RNG support, as per
  https://download.libsodium.org/doc/advanced/custom_rng.html

  Note that this RNG is selected by default (see randombytes_default.h), so there
  is no need to call randombytes_set_implementation().
*/
const struct randombytes_implementation randombytes_esp32_implementation = {
    .implementation_name = randombytes_esp32xx_implementation_name,
    .random = esp_random,
    .stir = NULL,
    .uniform = NULL,
    .buf = esp_fill_random,
    .close = NULL,
};
