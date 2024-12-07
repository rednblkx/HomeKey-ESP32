// Copyright 2021-2022 Espressif Systems (Shanghai) PTE LTD
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

#include "generate/esp_modem_command_declare_helper.inc"


#define DECLARE_ALL_COMMAND_APIS(...) \
/**
 * @brief Sends the initial AT sequence to sync up with the device
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(sync, command_result, 0) \
    \
/**
 * @brief Reads the operator name
 * @param[out] name operator name
 * @param[out] act access technology
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(get_operator_name, command_result, 2, STRING_OUT(p1, name), INT_OUT(p2, act)) \
    \
/**
 * @brief Stores current user profile
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(store_profile, command_result, 0) \
    \
/**
 * @brief Sets the supplied PIN code
 * @param[in] pin Pin
 * @return OK, FAIL or TIMEOUT
 */\
ESP_MODEM_DECLARE_DCE_COMMAND(set_pin, command_result, 1, STRING_IN(p1, pin)) \
    \
/**
 * @brief Execute the supplied AT command in raw mode (doesn't append '\r' to command, returns everything)
 * @param[in] cmd String command that's send to DTE
 * @param[out] out Raw output from DTE
 * @param[in] pass Pattern in response for the API to return OK
 * @param[in] fail Pattern in response for the API to return FAIL
 * @param[in] timeout AT command timeout in milliseconds
 * @return OK, FAIL or TIMEOUT
 */\
ESP_MODEM_DECLARE_DCE_COMMAND(at_raw, command_result, 5, STRING_IN(p1, cmd), STRING_OUT(p2, out), STRING_IN(p3, pass), STRING_IN(p4, fail), INT_IN(p5, timeout)) \
    \
/**
 * @brief Execute the supplied AT command
 * @param[in] cmd AT command
 * @param[out] out Command output string
 * @param[in] timeout AT command timeout in milliseconds
 * @return OK, FAIL or TIMEOUT
 */\
ESP_MODEM_DECLARE_DCE_COMMAND(at, command_result, 3, STRING_IN(p1, cmd), STRING_OUT(p2, out), INT_IN(p3, timeout)) \
    \
/**
 * @brief Checks if the SIM needs a PIN
 * @param[out] pin_ok true if the SIM card doesn't need a PIN to unlock
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(read_pin, command_result, 1, BOOL_OUT(p1, pin_ok)) \
    \
/**
 * @brief Sets echo mode
 * @param[in] echo_on true if echo mode on (repeats the commands)
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_echo, command_result, 1, BOOL_IN(p1, echo_on)) \
    \
/**
 * @brief Sets the Txt or Pdu mode for SMS (only txt is supported)
 * @param[in] txt true if txt mode
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(sms_txt_mode, command_result, 1, BOOL_IN(p1, txt)) \
    \
/**
 * @brief Sets the default (GSM) character set
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(sms_character_set, command_result, 0) \
    \
/**
 * @brief Sends SMS message in txt mode
 * @param[in] number Phone number to send the message to
 * @param[in] message Text message to be sent
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(send_sms, command_result, 2, STRING_IN(p1, number), STRING_IN(p2, message)) \
    \
/**
 * @brief Resumes data mode (Switches back to the data mode, which was temporarily suspended)
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(resume_data_mode, command_result, 0) \
    \
/**
 * @brief Sets php context
 * @param[in] p1 PdP context struct to setup modem cellular connection
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_pdp_context, command_result, 1, STRUCT_OUT(PdpContext, p1)) \
    \
/**
 * @brief Switches to the command mode
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_command_mode, command_result, 0) \
    \
/**
 * @brief Switches to the CMUX mode
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_cmux, command_result, 0) \
    \
/**
 * @brief Reads the IMSI number
 * @param[out] imsi Module's IMSI number
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(get_imsi, command_result, 1, STRING_OUT(p1, imsi)) \
    \
/**
 * @brief Reads the IMEI number
 * @param[out] imei Module's IMEI number
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(get_imei, command_result, 1, STRING_OUT(p1, imei)) \
    \
/**
 * @brief Reads the module name
 * @param[out] name module name
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(get_module_name, command_result, 1, STRING_OUT(p1, name)) \
    \
/**
 * @brief Sets the modem to data mode
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_data_mode, command_result, 0) \
    \
/**
 * @brief Get Signal quality
 * @param[out] rssi signal strength indication
 * @param[out] ber channel bit error rate
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(get_signal_quality, command_result, 2, INT_OUT(p1, rssi), INT_OUT(p2, ber)) \
    \
/**
 * @brief Sets HW control flow
 * @param[in] dce_flow 0=none, 2=RTS hw flow control of DCE
 * @param[in] dte_flow 0=none, 2=CTS hw flow control of DTE
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_flow_control, command_result, 2, INT_IN(p1, dce_flow), INT_IN(p2, dte_flow)) \
    \
/**
 * @brief Hangs up current data call
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(hang_up, command_result, 0) \
    \
/**
 * @brief Get voltage levels of modem power up circuitry
 * @param[out] voltage Current status in mV
 * @param[out] bcs charge status (-1-Not available, 0-Not charging, 1-Charging, 2-Charging done)
 * @param[out] bcl 1-100% battery capacity, -1-Not available
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(get_battery_status, command_result, 3, INT_OUT(p1, voltage), INT_OUT(p2, bcs), INT_OUT(p3, bcl)) \
    \
/**
 * @brief Power down the module
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(power_down, command_result, 0) \
    \
/**
 * @brief Reset the module
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(reset, command_result, 0) \
    \
/**
 * @brief Configures the baudrate
 * @param[in] baud Desired baud rate of the DTE
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_baud, command_result, 1, INT_IN(p1, baud)) \
    \
/**
 * @brief Force an attempt to connect to a specific operator
 * @param[in] mode mode of attempt
 * mode=0 - automatic
 * mode=1 - manual
 * mode=2 - deregister
 * mode=3 - set format for read operation
 * mode=4 - manual with fallback to automatic
 * @param[in] format what format the operator is given in
 * format=0 - long format
 * format=1 - short format
 * format=2 - numeric
 * @param[in] oper the operator to connect to
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_operator, command_result, 3, INT_IN(p1, mode), INT_IN(p2, format), STRING_IN(p3, oper)) \
    \
/**
 * @brief Attach or detach from the GPRS service
 * @param[in] state 1-attach 0-detach
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_network_attachment_state, command_result, 1, INT_IN(p1, state)) \
    \
/**
 * @brief Get network attachment state
 * @param[out] state 1-attached 0-detached
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(get_network_attachment_state, command_result, 1, INT_OUT(p1, state)) \
    \
/**
 * @brief What mode the radio should be set to
 * @param[in] state state 1-full 0-minimum ...
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_radio_state, command_result, 1, INT_IN(p1, state)) \
    \
/**
 * @brief Get current radio state
 * @param[out] state 1-full 0-minimum ...
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(get_radio_state, command_result, 1, INT_OUT(p1, state)) \
    \
/**
 * @brief Set network mode
 * @param[in] mode preferred mode
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_network_mode, command_result, 1, INT_IN(p1, mode)) \
    \
/**
 * @brief Preferred network mode (CAT-M and/or NB-IoT)
 * @param[in] mode preferred selection
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_preferred_mode, command_result, 1, INT_IN(p1, mode)) \
    \
/**
 * @brief Set network bands for CAT-M or NB-IoT
 * @param[in] mode CAT-M or NB-IoT
 * @param[in] bands bitmap in hex representing bands
 * @param[in] size size of teh bands bitmap
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_network_bands, command_result, 3, STRING_IN(p1, mode), INTEGER_LIST_IN(p2, bands), INT_IN(p3, size)) \
    \
/**
 * @brief Show network system mode
 * @param[out] mode current network mode
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(get_network_system_mode, command_result, 1, INT_OUT(p1, mode)) \
    \
/**
 * @brief GNSS power control
 * @param[out] mode power mode (0 - off, 1 - on)
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_gnss_power_mode, command_result, 1, INT_IN(p1, mode)) \
    \
/**
 * @brief GNSS power control
 * @param[out] mode power mode (0 - off, 1 - on)
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(get_gnss_power_mode, command_result, 1, INT_OUT(p1, mode)) \
    \


#ifdef GENERATE_DOCS
// cat ../include/generate/esp_modem_command_declare.inc | clang++ -E -P -CC  -xc++ -I../include -DGENERATE_DOCS  - | sed -n '1,/DCE command documentation/!p'
// cat ../include/generate/esp_modem_command_declare.inc | clang -E -P -CC  -xc -I../include -DGENERATE_DOCS  - | sed -n '1,/DCE command documentation/!p' > c_api.h
// cat ../include/generate/esp_modem_command_declare.inc | clang -E -P  -xc -I../include -DGENERATE_DOCS -DGENERATE_RST_LINKS - | sed 's/NL/\n/g' > cxx_api_links.rst

// call parametrs by names for documentation
#undef _ARG
#define _ARG(param, name) name
//  --- DCE command documentation starts here ---
#ifdef __cplusplus
class esp_modem::DCE : public DCE_T<GenericModule> {
public:
    using DCE_T<GenericModule>::DCE_T;
#define ESP_MODEM_DECLARE_DCE_COMMAND(name, return_type, TEMPLATE_ARG, ...) return_type name (__VA_ARGS__);
#elif defined(GENERATE_RST_LINKS)
#define ESP_MODEM_DECLARE_DCE_COMMAND(name, return_type, TEMPLATE_ARG, ...) NL- :cpp:func:`esp_modem::DCE::name`
#else
#define ESP_MODEM_DECLARE_DCE_COMMAND(name, return_type, TEMPLATE_ARG, ...) return_type esp_modem_ ## name (__VA_ARGS__);
#endif

    DECLARE_ALL_COMMAND_APIS()

#ifdef __cplusplus
};
#endif

#endif
