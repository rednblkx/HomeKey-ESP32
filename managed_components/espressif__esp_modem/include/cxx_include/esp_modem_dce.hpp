/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <utility>
#include "cxx_include/esp_modem_netif.hpp"
#include "cxx_include/esp_modem_dce_module.hpp"

namespace esp_modem {

/**
 * @defgroup ESP_MODEM_DCE
 * @brief Definition of DCE abstraction
 */
/** @addtogroup ESP_MODEM_DCE
 * @{
 */


/**
 * @brief Helper class responsible for switching modes of the DCE's
 */
class DCE_Mode {
public:
    DCE_Mode(): mode(modem_mode::UNDEF) {}
    ~DCE_Mode() = default;
    bool set(DTE *dte, ModuleIf *module, Netif &netif, modem_mode m);
    modem_mode get();
    modem_mode guess(DTE *dte, bool with_cmux = false);

private:
    bool set_unsafe(DTE *dte, ModuleIf *module, Netif &netif, modem_mode m);
    modem_mode guess_unsafe(DTE *dte, bool with_cmux);
    modem_mode mode;

};

/**
 * @brief General DCE class templated on a specific module. It is responsible for all the necessary transactions
 * related to switching modes and consequent synergy with aggregated objects of DTE, Netif and a specific Module
 */
template<class SpecificModule>
class DCE_T {
    static_assert(std::is_base_of<ModuleIf, SpecificModule>::value, "DCE must be instantiated with Module class only");
public:
    explicit DCE_T(const std::shared_ptr<DTE> &dte, std::shared_ptr<SpecificModule> dev, esp_netif_t *netif):
        dte(dte), device(std::move(dev)), netif(dte, netif)
    { }

    ~DCE_T() = default;

    /**
     * @brief Set data mode!
     */
    void set_data()
    {
        set_mode(modem_mode::DATA_MODE);
    }

    void exit_data()
    {
        set_mode(modem_mode::COMMAND_MODE);
    }

    void set_cmux()
    {
        set_mode(modem_mode::CMUX_MODE);
    }

    SpecificModule *get_module()
    {
        return device.get();
    }

    command_result command(const std::string &command, got_line_cb got_line, uint32_t time_ms)
    {
        return dte->command(command, std::move(got_line), time_ms);
    }

    modem_mode guess_mode(bool with_cmux = false)
    {
        return mode.guess(dte.get(), with_cmux);
    }

    bool set_mode(modem_mode m)
    {
        return mode.set(dte.get(), device.get(), netif, m);
    }

    modem_mode get_mode()
    {
        return mode.get();
    }

    bool recover()
    {
        return dte->recover();
    }

#ifdef CONFIG_ESP_MODEM_URC_HANDLER
    void set_urc(got_line_cb on_read_cb)
    {
        dte->set_urc_cb(on_read_cb);
    }
#endif

    /**
     * @brief Pauses/Unpauses network temporarily
     * @param do_pause true to pause, false to unpause
     * @param force true to ignore command failures and continue
     * @return command_result of the underlying commands
     */
    command_result pause_netif(bool do_pause, bool force = false, int delay = 1000)
    {
        command_result result;
        if (do_pause) {
            netif.pause();
            Task::Delay(delay); // Mandatory 1s pause before
            dte->set_command_callbacks();
            result = device->set_command_mode();
        } else {
            result = device->resume_data_mode();
            if (result == command_result::OK || force) {
                netif.resume();
            }
        }
        return result;
    }

protected:
    std::shared_ptr<DTE> dte;
    std::shared_ptr<SpecificModule> device;
    Netif netif;
    DCE_Mode mode;
};

/**
 * @brief Common abstraction of the modem DCE, specialized by the GenericModule which is a parent class for the supported
 * devices and most common modems, as well.
 */
class DCE : public DCE_T<GenericModule> {
public:

    using DCE_T<GenericModule>::DCE_T;
#define ESP_MODEM_DECLARE_DCE_COMMAND(name, return_type, num, ...) \
    template <typename ...Agrs> \
    return_type name(Agrs&&... args)   \
    {   \
        return device->name(std::forward<Agrs>(args)...); \
    }

    DECLARE_ALL_COMMAND_APIS(forwards name(...)
    {
        device->name(...);
    } )

#undef ESP_MODEM_DECLARE_DCE_COMMAND

};

/**
 * @}
 */

} // esp_modem
