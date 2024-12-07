/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <memory>
#include <utility>
#include "generate/esp_modem_command_declare.inc"
#include "cxx_include/esp_modem_command_library.hpp"
#include "cxx_include/esp_modem_types.hpp"
#include "esp_modem_dce_config.h"

namespace esp_modem {

/**
 * @defgroup ESP_MODEM_MODULE
 * @brief Definition of modules representing specific modem devices
 */

/** @addtogroup ESP_MODEM_MODULE
* @{
*/

enum class command_result;
class DTE;
struct PdpContext;

/**
 * @brief This is a basic building block for custom modules as well as for the supported modules in the esp-modem component
 * It derives from the ModuleIf.
 */
class GenericModule: public ModuleIf {
public:
    /**
     * @brief We can construct a generic device with an existent DTE and it's configuration
     * The configuration could be either the dce-config struct or just a pdp context
     */
    explicit GenericModule(std::shared_ptr<DTE> dte, std::unique_ptr<PdpContext> pdp):
        dte(std::move(dte)), pdp(std::move(pdp)) {}
    explicit GenericModule(std::shared_ptr<DTE> dte, const esp_modem_dce_config *config);

    /**
     * @brief This is a mandatory method for ModuleIf class, which sets up the device
     * to be able to connect to the network. This typically consists of setting basic
     * communication parameters and setting the PDP (defining logical access point
     * to cellular network)
     */
    bool setup_data_mode() override
    {
        if (set_echo(false) != command_result::OK) {
            return false;
        }
        if (set_pdp_context(*pdp) != command_result::OK) {
            return false;
        }
        return true;
    }

    /**
     * @brief This is a mandatory method of ModuleIf class, which defines
     * basic commands for switching between DATA, COMMAND and CMUX modes
     */
    bool set_mode(modem_mode mode) override
    {
        if (mode == modem_mode::DATA_MODE) {
            if (set_data_mode() != command_result::OK) {
                return resume_data_mode() == command_result::OK;
            }
            return true;
        } else if (mode == modem_mode::COMMAND_MODE) {
            Task::Delay(1000); // Mandatory 1s pause before
            int retry = 0;
            while (retry++ < 3) {
                if (set_command_mode() == command_result::OK) {
                    return true;
                }
                // send a newline to delimit the escape from the upcoming sync command
                uint8_t delim = '\n';
                dte->write(&delim, 1);
                if (sync() == command_result::OK) {
                    return true;
                }
                Task::Delay(1000); // Mandatory 1s pause before escape
            }
            return false;
        } else if (mode == modem_mode::CMUX_MODE) {
            return set_cmux() == command_result::OK;
        }
        return true;
    }

    /**
     * @brief Additional method providing runtime configuration of PDP context
     */
    void configure_pdp_context(std::unique_ptr<PdpContext> new_pdp)
    {
        pdp = std::move(new_pdp);
    }

    /**
     * @brief Simplified version of operator name (without the ACT, which is included in the command library)
     */
    command_result get_operator_name(std::string &name)
    {
        int dummy_act;
        return get_operator_name(name, dummy_act);
    }

    /**
     * @brief Common DCE commands generated from the API AT list
     */
#define ESP_MODEM_DECLARE_DCE_COMMAND(name, return_type, num, ...) \
    virtual return_type name(__VA_ARGS__);

    DECLARE_ALL_COMMAND_APIS(virtual return_type name(...); )

#undef ESP_MODEM_DECLARE_DCE_COMMAND


protected:
    std::shared_ptr<DTE> dte;         /*!< Generic device needs the DTE as a channel talk to the module using AT commands */
    std::unique_ptr<PdpContext> pdp;  /*!< It also needs a PDP data, const information used for setting up cellular network */
};

// Definitions of other supported modules with some specific commands overwritten

/**
 * @brief Specific definition of the SIM7600 module
 */
class SIM7600: public GenericModule {
    using GenericModule::GenericModule;
public:
    command_result get_battery_status(int &voltage, int &bcs, int &bcl) override;
    command_result power_down() override;
    command_result set_gnss_power_mode(int mode) override;
    command_result set_network_bands(const std::string &mode, const int *bands, int size) override;
};

/**
 * @brief Specific definition of the SIM7070 module
 */
class SIM7070: public GenericModule {
    using GenericModule::GenericModule;
public:
    command_result power_down() override;
    command_result set_data_mode() override;

};

/**
 * @brief Specific definition of the SIM7000 module
 */
class SIM7000: public GenericModule {
    using GenericModule::GenericModule;
public:
    command_result power_down() override;
};

/**
 * @brief Specific definition of the SIM800 module
 */
class SIM800: public GenericModule {
    using GenericModule::GenericModule;
public:
    command_result power_down() override;
};

/**
 * @brief Specific definition of the BG96 module
 */
class BG96: public GenericModule {
    using GenericModule::GenericModule;
public:
    command_result set_pdp_context(PdpContext &pdp) override;
};

/**
 * @}
 */

} // namespace esp_modem
