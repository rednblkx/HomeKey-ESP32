#include "sdkconfig.h"

#if CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC

#include "nvs_sec_provider.h"

/**
 * @brief Register the flash-encryption-based NVS security scheme before any
 *        NVS init runs.
 *
 * HomeSpan's global Span instance calls nvs_flash_init() from a C++ static
 * constructor (default init priority), which runs before the built-in
 * registration in nvs_sec_provider (ESP_SYSTEM_INIT_FN, SECONDARY/150) —
 * C++ constructors execute before secondary init functions. This static
 * object uses an explicit low init_priority so its constructor runs first.
 * The later built-in registration simply overwrites this one.
 *
 * The object lives in a static library and is referenced by nothing, so the
 * linker would drop it; the -u flag on nvs_sec_early_init (set in
 * CMakeLists.txt) forces the member to be linked in.
 */
extern "C" void nvs_sec_early_init(void)
{
    nvs_sec_config_flash_enc_t sec_scheme_cfg = NVS_SEC_PROVIDER_CFG_FLASH_ENC_DEFAULT();
    nvs_sec_scheme_t *handle = nullptr;
    nvs_sec_provider_register_flash_enc(&sec_scheme_cfg, &handle);
}

namespace {

struct EarlyNvsSecRegistrar {
    EarlyNvsSecRegistrar() { nvs_sec_early_init(); }
};

__attribute__((init_priority(101))) EarlyNvsSecRegistrar early_nvs_sec_registrar;

} // namespace

#endif // CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC
