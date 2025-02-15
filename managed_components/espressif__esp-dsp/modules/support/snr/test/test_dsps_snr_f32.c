// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
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
#include "unity.h"
#include "dsp_platform.h"
#include "esp_log.h"

#include "dsps_view.h"
#include "dsps_snr.h"
#include "dsps_fft2r.h"


static const char *TAG = "dsps_snr_f32";

static float data[1024];

TEST_CASE("dsps_snr_f32 functionality", "[dsps]")
{
    int N = sizeof(data) / sizeof(float) / 2;
    int check_bin = 32;
    float snr_exp = 0.001;
    for (int i = 0 ; i < N ; i++) {
        data[i] = 1 * sinf(M_PI / N * check_bin * i) / (N / 2);
        data[i] += 0.001 / N; //0.1*sinf(M_PI/N*check_bin*i*2)/(N/2);
    }

    float snr = dsps_snr_f32(data, N, 1);
    TEST_ASSERT_EQUAL(-round(20 * log10(snr_exp) + 3), (int)round(snr));
    ESP_LOGI(TAG, "dsps_snr_f32 = %f dB", snr);
    dsps_fft2r_deinit_fc32();
}
