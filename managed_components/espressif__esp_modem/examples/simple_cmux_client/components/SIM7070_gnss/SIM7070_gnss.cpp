/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
//
// Created on: 23.08.2022
// Author: franz

#include <string_view>
#include <charconv>
#include <list>
#include "sdkconfig.h"
#include "cxx_include/esp_modem_dte.hpp"
#include "cxx_include/esp_modem_dce.hpp"
#include "esp_modem_config.h"
#include "cxx_include/esp_modem_api.hpp"
#include "cxx_include/esp_modem_command_library_utils.hpp"
#include "esp_log.h"
#include "SIM7070_gnss.hpp"

constexpr auto const TAG = "SIM7070_gnss";


namespace gnss_factory {
using namespace esp_modem;
using namespace dce_factory;

class LocalFactory: public Factory {
    using DCE_gnss_ret = std::unique_ptr<DCE_gnss>;   // this custom Factory manufactures only unique_ptr<DCE>'s
public:
    static DCE_gnss_ret create(const dce_config *config, std::shared_ptr<DTE> dte, esp_netif_t *netif)
    {
        return Factory::build_generic_DCE<SIM7070_gnss, DCE_gnss, DCE_gnss_ret>
               (config, std::move(dte), netif);
    }
};

} // namespace gnss_factory

/**
 * @brief Helper create method which employs the DCE factory for creating DCE objects templated by a custom module
 * @return unique pointer of the resultant DCE
 */
std::unique_ptr<DCE_gnss> create_SIM7070_GNSS_dce(const esp_modem::dce_config *config,
        std::shared_ptr<esp_modem::DTE> dte,
        esp_netif_t *netif)
{
    return gnss_factory::LocalFactory::create(config, std::move(dte), netif);
}

esp_modem::command_result get_gnss_information_sim70xx_lib(esp_modem::CommandableIf *t, sim70xx_gps_t &gps)
{

    ESP_LOGV(TAG, "%s", __func__ );
    std::string str_out;
    auto ret = esp_modem::dce_commands::generic_get_string(t, "AT+CGNSINF\r", str_out);
    if (ret != esp_modem::command_result::OK) {
        return ret;
    }
    std::string_view out(str_out);


    constexpr std::string_view pattern = "+CGNSINF: ";
    if (out.find(pattern) == std::string_view::npos) {
        return esp_modem::command_result::FAIL;
    }
    /**
     * Parsing +CGNSINF:
    | **Index** | **Parameter**          | **Unit**           | **Range**                                                                            | **Length** |
    |-----------|------------------------|--------------------|--------------------------------------------------------------------------------------|------------|
    | 1         | GNSS run status        | --                 | 0-1                                                                                  | 1          |
    | 2         | Fix status             | --                 | 0-1                                                                                  | 1          |
    | 3         | UTC date & Time        | yyyyMMddhhmmss.sss | yyyy: [1980,2039] MM : [1,12] dd: [1,31] hh: [0,23] mm: [0,59] ss.sss:[0.000,60.999] | 18         |
    | 4         | Latitude               | ±dd.dddddd         | [-90.000000,90.000000]                                                               | 10         |
    | 5         | Longitude              | ±dd.dddddd         | -180.000000,180.000000]                                                              | 11         |
    | 6         | MSL Altitude           | meters             | [0,999.99]                                                                           | 8          |
    | 7         | Speed Over Ground      | Km/hour            | [0,360.00]                                                                           | 6          |
    | 8         | Course Over Ground     | degrees            | 0,1,2[1]                                                                             | 6          |
    | 9         | Fix Mode               | --                 |                                                                                      | 1          |
    | 10        | Reserved1              |                    |                                                                                      | 0          |
    | 11        | HDOP                   | --                 | [0,99.9]                                                                             | 4          |
    | 12        | PDOP                   | --                 | [0,99.9]                                                                             | 4          |
    | 13        | VDOP                   | --                 | [0,99.9]                                                                             | 4          |
    | 14        | Reserved2              |                    |                                                                                      | 0          |
    | 15        | GPS Satellites in View | --                 | -- [0,99]                                                                            | 2          |
    | 16        | Reserved3              |                    |                                                                                      | 0          |
    | 17        | HPA[2]                 | meters             | [0,9999.9]                                                                           | 6          |
    | 18        | VPA[2]                 | meters             | [0,9999.9]                                                                           | 6          |
     */
    out = out.substr(pattern.size());
    int pos = 0;
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    //GNSS run status
    int GNSS_run_status;
    if (std::from_chars(out.data(), out.data() + pos, GNSS_run_status).ec == std::errc::invalid_argument) {
        return esp_modem::command_result::FAIL;
    }
    gps.run = (gps_run_t)GNSS_run_status;
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    //Fix status
    {
        std::string_view fix_status = out.substr(0, pos);
        if (fix_status.length() > 1) {
            int Fix_status;
            if (std::from_chars(out.data(), out.data() + pos, Fix_status).ec == std::errc::invalid_argument) {
                return esp_modem::command_result::FAIL;
            }
            gps.fix = (gps_fix_t)Fix_status;
        } else {
            gps.fix  = GPS_FIX_INVALID;
        }
    } //clean up Fix status
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    //UTC date &  Time
    {
        std::string_view UTC_date_and_Time = out.substr(0, pos);
        if (UTC_date_and_Time.length() > 1) {
            if (std::from_chars(out.data() + 0, out.data() + 4, gps.date.year).ec == std::errc::invalid_argument) {
                return esp_modem::command_result::FAIL;
            }
            if (std::from_chars(out.data() + 4, out.data() + 6, gps.date.month).ec == std::errc::invalid_argument) {
                return esp_modem::command_result::FAIL;
            }
            if (std::from_chars(out.data() + 6, out.data() + 8, gps.date.day).ec == std::errc::invalid_argument) {
                return esp_modem::command_result::FAIL;
            }
            if (std::from_chars(out.data() + 8, out.data() + 10, gps.tim.hour).ec == std::errc::invalid_argument) {
                return esp_modem::command_result::FAIL;
            }
            if (std::from_chars(out.data() + 10, out.data() + 12, gps.tim.minute).ec == std::errc::invalid_argument) {
                return esp_modem::command_result::FAIL;
            }
            if (std::from_chars(out.data() + 12, out.data() + 14, gps.tim.second).ec == std::errc::invalid_argument) {
                return esp_modem::command_result::FAIL;
            }
            if (std::from_chars(out.data() + 15, out.data() + 18, gps.tim.thousand).ec == std::errc::invalid_argument) {
                return esp_modem::command_result::FAIL;
            }
        } else {
            gps.date.year    = 0;
            gps.date.month   = 0;
            gps.date.day     = 0;
            gps.tim.hour     = 0;
            gps.tim.minute   = 0;
            gps.tim.second   = 0;
            gps.tim.thousand = 0;
        }

    } //clean up UTC date &  Time
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    //Latitude
    {
        std::string_view Latitude = out.substr(0, pos);
        if (Latitude.length() > 1) {
            gps.latitude  = std::stof(std::string(out.substr(0, pos)));
        } else {
            gps.latitude  = 0;
        }
    } //clean up Latitude
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    //Longitude
    {
        std::string_view Longitude = out.substr(0, pos);
        if (Longitude.length() > 1) {
            gps.longitude  = std::stof(std::string(out.substr(0, pos)));
        } else {
            gps.longitude  = 0;
        }
    } //clean up Longitude
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    //Altitude
    {
        std::string_view Altitude = out.substr(0, pos);
        if (Altitude.length() > 1) {
            gps.altitude  = std::stof(std::string(out.substr(0, pos)));
        } else {
            gps.altitude  = 0;
        }
    } //clean up Altitude
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    //Speed Over Ground Km/hour
    {
        std::string_view gps_speed = out.substr(0, pos);
        if (gps_speed.length() > 1) {
            gps.speed  = std::stof(std::string(gps_speed));
        } else {
            gps.speed  = 0;
        }
    } //clean up gps_speed
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    //Course Over Ground degrees
    {
        std::string_view gps_cog = out.substr(0, pos);
        if (gps_cog.length() > 1) {
            gps.cog  = std::stof(std::string(gps_cog));
        } else {
            gps.cog  = 0;
        }
    } //clean up gps_cog
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    // Fix Mode
    {
        std::string_view FixModesubstr = out.substr(0, pos);
        if (FixModesubstr.length() > 1) {
            int Fix_Mode;
            if (std::from_chars(out.data(), out.data() + pos, Fix_Mode).ec == std::errc::invalid_argument) {
                return esp_modem::command_result::FAIL;
            }
            gps.fix_mode = (gps_fix_mode_t)Fix_Mode;
        } else {
            gps.fix_mode  = GPS_MODE_INVALID;
        }
    } //clean up Fix Mode
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    //HDOP
    {
        std::string_view HDOP = out.substr(0, pos);
        if (HDOP.length() > 1) {
            gps.dop_h  = std::stof(std::string(HDOP));
        } else {
            gps.dop_h  = 0;
        }
    } //clean up HDOP
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    //PDOP
    {
        std::string_view PDOP = out.substr(0, pos);
        if (PDOP.length() > 1) {
            gps.dop_p  = std::stof(std::string(PDOP));
        } else {
            gps.dop_p  = 0;
        }
    } //clean up PDOP
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    //VDOP
    {
        std::string_view VDOP = out.substr(0, pos);
        if (VDOP.length() > 1) {
            gps.dop_v  = std::stof(std::string(VDOP));
        } else {
            gps.dop_v  = 0;
        }
    } //clean up VDOP
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    //sats_in_view
    {
        std::string_view sats_in_view = out.substr(0, pos);
        if (sats_in_view.length() > 1) {
            if (std::from_chars(out.data(), out.data() + pos, gps.sat.num).ec == std::errc::invalid_argument) {
                return esp_modem::command_result::FAIL;
            }
        } else {
            gps.sat.num  = 0;
        }
    } //clean up sats_in_view

    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    out = out.substr(pos + 1);
    if ((pos = out.find(',')) == std::string::npos) {
        return esp_modem::command_result::FAIL;
    }
    //HPA
    {
        std::string_view HPA = out.substr(0, pos);
        if (HPA.length() > 1) {
            gps.hpa  = std::stof(std::string(HPA));
        } else {
            gps.hpa  = 0;
        }
    } //clean up HPA
    out = out.substr(pos + 1);
    //VPA
    {
        std::string_view VPA = out.substr(0, pos);
        if (VPA.length() > 1) {
            gps.vpa  = std::stof(std::string(VPA));
        } else {
            gps.vpa  = 0;
        }
    } //clean up VPA

    return esp_modem::command_result::OK;
}

esp_modem::command_result SIM7070_gnss::get_gnss_information_sim70xx(sim70xx_gps_t &gps)
{
    return get_gnss_information_sim70xx_lib(dte.get(), gps);
}

esp_modem::command_result DCE_gnss::get_gnss_information_sim70xx(sim70xx_gps_t &gps)
{
    return device->get_gnss_information_sim70xx(gps);
}
