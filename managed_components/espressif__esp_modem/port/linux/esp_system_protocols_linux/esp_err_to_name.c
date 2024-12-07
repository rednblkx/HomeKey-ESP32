/*
 * SPDX-FileCopyrightText: Franz Hoepfinger
 *
 * SPDX-License-Identifier: Apache-2.0
 */



static const char esp_unknown_msg[] =  "ERROR";

const char *esp_err_to_name(int code)
{
    return esp_unknown_msg;
}
