/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_log.h"

/**
 * @defgroup ESP_MODEM_DCE_FACTORY
 * @brief DCE modem factory
 */

#define esp_modem_create_dce_from dce_factory::Factory::create_unique_dce_from

namespace esp_modem::dce_factory {

using config = ::esp_modem_dce_config;

/** @addtogroup ESP_MODEM_DCE_FACTORY
* @{
*/


/**
 * @brief Helper class for creating a user define pointer in a specific way, either as a plain pointer, shared_ptr or unique_ptr
 */
class FactoryHelper {
public:
    static std::unique_ptr<PdpContext> create_pdp_context(std::string &apn);

    template <typename T, typename T_Ptr, typename ...Args>
    static auto make(Args &&... args) -> typename std::enable_if<std::is_same<T_Ptr, T *>::value, T *>::type
    {
        return new T(std::forward<Args>(args)...);
    }

    template <typename T, typename T_Ptr, typename ...Args>
    static auto make(Args &&... args) ->  typename std::enable_if<std::is_same<T_Ptr, std::shared_ptr<T>>::value, std::shared_ptr<T>>::type
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <typename T, typename T_Ptr = std::unique_ptr<T>, typename ...Args>
    static auto make(Args && ... args) -> typename std::enable_if<std::is_same<T_Ptr, std::unique_ptr<T>>::value, std::unique_ptr<T>>::type
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
};

/**
 * @brief Creator class for building a DCE_T<Module> in a specific way, either from a Module object or by default from the DTE and netif
 *
 * @throws
 *      - esp_modem::esp_err_exception on invalid arguments
 *      - std::bad_alloc if failed to allocate
 */
template<typename T_Module>
class Creator {
    static_assert(std::is_base_of<ModuleIf, T_Module>::value, "Builder must be used only for Module classes");
public:
    Creator(std::shared_ptr<DTE> dte, esp_netif_t *esp_netif): dte(std::move(dte)), device(nullptr), netif(esp_netif)
    {
        ESP_MODEM_THROW_IF_FALSE(netif != nullptr, "Null netif");
    }

    Creator(std::shared_ptr<DTE> dte, esp_netif_t *esp_netif, std::shared_ptr<T_Module> dev): dte(std::move(dte)), device(std::move(dev)), netif(esp_netif)
    {
        ESP_MODEM_THROW_IF_FALSE(netif != nullptr, "Null netif");
    }

    explicit Creator(std::shared_ptr<DTE> dte): dte(std::move(dte)), device(nullptr), netif(nullptr)
    {
    }

    ~Creator()
    {
        if (device != nullptr) {
            ESP_LOGE("dce_factory::~Creator", "module was captured or created but never used");
        }
    }

    template<typename T_Ptr>
    auto create_module(const esp_modem_dce_config *config) -> T_Ptr
    {
        return FactoryHelper::make<T_Module, T_Ptr>(dte, config);
    }

    template<typename T_Dce, typename T_Ptr>
    auto create(const esp_modem_dce_config *config) -> T_Ptr
    {
        if (dte == nullptr) {
            return nullptr;
        }
        if (device == nullptr) {
            device = create_module<decltype(device)>(config);
            if (device == nullptr) {
                return nullptr;
            }
        }
        return FactoryHelper::make<T_Dce, T_Ptr>(std::move(dte), std::move(device), netif);
    }

private:
    std::shared_ptr<DTE> dte;
    std::shared_ptr<T_Module> device;
    esp_netif_t *netif;
};

/**
 * @brief Specific modem choice when creating by the Factory
 */
enum class ModemType {
    GenericModule,      /*!< Default generic module with the most common commands */
    SIM7600,            /*!< Derived from the GenericModule, specifics applied to SIM7600 model */
    SIM7070,            /*!< Derived from the GenericModule, specifics applied to SIM7070 model */
    SIM7000,            /*!< Derived from the GenericModule, specifics applied to SIM7000 model */
    BG96,               /*!< Derived from the GenericModule, specifics applied to BG69 model */
    SIM800,             /*!< Derived from the GenericModule with specifics applied to SIM800 model */
};

/**
 * @brief Factory class for creating virtual DCE objects based on the configuration of the supplied module.
 * This could also be used to create a custom module or a DCE_T<module>, provided user app derives from this factory.
 */
class Factory {
public:
    explicit Factory(ModemType modem): m(modem) {}

    /**
     * @brief Create a default unique_ptr DCE in a specific way (from the module)
     * @tparam Module Specific Module used in this DCE
     * @tparam Args Arguments to the builder, i.e. constructor of esp_modem::DCE_T class
     * @param cfg DCE configuration structure ::esp_modem_dte_config
     * @param args typically a DTE object and a netif handle for PPP network
     * @return unique_ptr DCE of the created DCE on success
     */
    template <typename T_Module, typename ...Args>
    static std::unique_ptr<DCE> build_unique(const config *cfg, Args &&... args)
    {
        return build_generic_DCE<T_Module, DCE, std::unique_ptr<DCE>>(cfg, std::forward<Args>(args)...);
    }

    /**
     * @brief Create a DCE
     * @tparam Module Specific Module used in this DCE
     * @tparam Args Arguments to the builder, i.e. constructor of esp_modem::DCE_T class
     * @param cfg DCE configuration structure ::esp_modem_dte_config
     * @param args typically a DTE object and a netif handle for PPP network
     * @return DCE pointer the created DCE on success
     */
    template <typename T_Module, typename ...Args>
    static DCE *build(const config *cfg, Args &&... args)
    {
        return build_generic_DCE<T_Module, DCE>(cfg, std::forward<Args>(args)...);
    }


    template <typename T_Module, typename ...Args>
    static std::shared_ptr<T_Module> build_shared_module(const config *cfg, Args &&... args)
    {
        return build_module_T<T_Module>(cfg, std::forward<Args>(args)...);
    }


    template <typename ...Args>
    std::shared_ptr<GenericModule> build_shared_module(const config *cfg, Args &&... args)
    {
        switch (m) {
        case ModemType::SIM800:
            return build_shared_module<SIM800>(cfg, std::forward<Args>(args)...);
        case ModemType::SIM7600:
            return build_shared_module<SIM7600>(cfg, std::forward<Args>(args)...);
        case ModemType::SIM7070:
            return build_shared_module<SIM7070>(cfg, std::forward<Args>(args)...);
        case ModemType::SIM7000:
            return build_shared_module<SIM7000>(cfg, std::forward<Args>(args)...);
        case ModemType::BG96:
            return build_shared_module<BG96>(cfg, std::forward<Args>(args)...);
        case ModemType::GenericModule:
            return build_shared_module<GenericModule>(cfg, std::forward<Args>(args)...);
        default:
            break;
        }
        return nullptr;
    }

    /**
     * @brief Create a default unique_ptr DCE generically, with the chosen module derived from the GenericModule
     * @tparam Args Arguments to the builder, i.e. constructor of esp_modem::DCE_T class
     * @param cfg DCE configuration structure ::esp_modem_dte_config
     * @param args typically a DTE object and a netif handle for PPP network
     * @return unique_ptr DCE of the created DCE on success
     */
    template <typename ...Args>
    std::unique_ptr<DCE> build_unique(const config *cfg, Args &&... args)
    {
        switch (m) {
        case ModemType::SIM800:
            return build_unique<SIM800>(cfg, std::forward<Args>(args)...);
        case ModemType::SIM7600:
            return build_unique<SIM7600>(cfg, std::forward<Args>(args)...);
        case ModemType::SIM7070:
            return build_unique<SIM7070>(cfg, std::forward<Args>(args)...);
        case ModemType::SIM7000:
            return build_unique<SIM7000>(cfg, std::forward<Args>(args)...);
        case ModemType::BG96:
            return build_unique<BG96>(cfg, std::forward<Args>(args)...);
        case ModemType::GenericModule:
            return build_unique<GenericModule>(cfg, std::forward<Args>(args)...);
        default:
            break;
        }
        return nullptr;
    }

    template <typename ...Args>
    DCE *build(const config *cfg, Args &&... args)
    {
        switch (m) {
        case ModemType::SIM800:
            return build<SIM800>(cfg, std::forward<Args>(args)...);
        case ModemType::SIM7600:
            return build<SIM7600>(cfg, std::forward<Args>(args)...);
        case ModemType::SIM7070:
            return build<SIM7070>(cfg, std::forward<Args>(args)...);
        case ModemType::SIM7000:
            return build<SIM7000>(cfg, std::forward<Args>(args)...);
        case ModemType::BG96:
            return build<BG96>(cfg, std::forward<Args>(args)...);
        case ModemType::GenericModule:
            return build<GenericModule>(cfg, std::forward<Args>(args)...);
        default:
            break;
        }
        return nullptr;
    }

    template <typename T_Module, typename Ptr = std::unique_ptr<DCE>>
    static Ptr create_unique_dce_from(const esp_modem::dce_config *config,
                                      std::shared_ptr<esp_modem::DTE> dte,
                                      esp_netif_t *netif)
    {
        return build_generic_DCE<T_Module, DCE, Ptr>(config, std::move(dte), netif);
    }

private:
    ModemType m;

protected:
    template <typename T_Module, typename Ptr = std::shared_ptr<T_Module>, typename ...Args>
    static Ptr build_module_T(const config *cfg, Args && ... args)
    {
        Creator<T_Module> creator(std::forward<Args>(args)...);
        return creator.template create_module<Ptr>(cfg);
    }


    template <typename T_Module, typename T_Dce = DCE_T<T_Module>, typename T_DcePtr = T_Dce *,  typename ...Args>
    static auto build_generic_DCE(const config *cfg, Args && ... args) -> T_DcePtr
    {
        Creator<T_Module> creator(std::forward<Args>(args)...);
        return creator.template create<T_Dce, T_DcePtr>(cfg);
    }
};

/**
 * @}
 */

} // namespace esp_modem::dce_factory
