# Changelog

## [1.3.0](https://github.com/espressif/esp-protocols/commits/modem-v1.3.0)

### Features

- Add mode detection to the example ([18f196fa](https://github.com/espressif/esp-protocols/commit/18f196fa))
- Support for pausing network in C-API ([1db83cd1](https://github.com/espressif/esp-protocols/commit/1db83cd1))
- Add support for pausing netif ([247f1681](https://github.com/espressif/esp-protocols/commit/247f1681), [#699](https://github.com/espressif/esp-protocols/issues/699))

### Bug Fixes

- Minor cleanup of pppos example ([5e929902](https://github.com/espressif/esp-protocols/commit/5e929902))
- Fix PPP mode detection to accept LCP/conf ([c989c6ad](https://github.com/espressif/esp-protocols/commit/c989c6ad))
- Refine mode switch data->command ([8b6ea331](https://github.com/espressif/esp-protocols/commit/8b6ea331), [#692](https://github.com/espressif/esp-protocols/issues/692))
- Detect serial ports properly ([0cb59ff8](https://github.com/espressif/esp-protocols/commit/0cb59ff8))
- Fix CMUX enter to ignore URC before transition ([1284f66d](https://github.com/espressif/esp-protocols/commit/1284f66d), [#669](https://github.com/espressif/esp-protocols/issues/669))

## [1.2.1](https://github.com/espressif/esp-protocols/commits/modem-v1.2.1)

### Bug Fixes

- Use higher GPIO range to support new chips ([428fdbbd](https://github.com/espressif/esp-protocols/commit/428fdbbd), [#558](https://github.com/espressif/esp-protocols/issues/558))
- Remove tests and support for IDFv4.4, added IDFv5.4 ([433a033f](https://github.com/espressif/esp-protocols/commit/433a033f))
- Fix typo GENETIC -> GENERIC in mode types ([090b1ff8](https://github.com/espressif/esp-protocols/commit/090b1ff8), [#667](https://github.com/espressif/esp-protocols/issues/667))
- Add support for URC handler into C-API ([295d99df](https://github.com/espressif/esp-protocols/commit/295d99df), [#180](https://github.com/espressif/esp-protocols/issues/180))

## [1.2.0](https://github.com/espressif/esp-protocols/commits/modem-v1.2.0)

### Features

- Add support for guessing mode ([52598e5f](https://github.com/espressif/esp-protocols/commit/52598e5f))
- Delete CMUX internal implementation even if terminal exit fails ([0e0cbd6b](https://github.com/espressif/esp-protocols/commit/0e0cbd6b))
- Add support for handling URC ([1b6a3b3b](https://github.com/espressif/esp-protocols/commit/1b6a3b3b), [#180](https://github.com/espressif/esp-protocols/issues/180))
- add ability to change ESP_MODEM_C_API_STR_MAX from Kconfig ([17909892](https://github.com/espressif/esp-protocols/commit/17909892))
- Added target test config with CHAP authentication ([f8ae7def](https://github.com/espressif/esp-protocols/commit/f8ae7def))
- example add esp32p4 usb support ([adafeae5](https://github.com/espressif/esp-protocols/commit/adafeae5))
- Publish mbedtls component ([0140455f](https://github.com/espressif/esp-protocols/commit/0140455f))
- host test support of the latest ESP-IDF release ([3f74b4e8](https://github.com/espressif/esp-protocols/commit/3f74b4e8))

### Bug Fixes

- Fix console example to use urc/detect features ([1a9eaf3e](https://github.com/espressif/esp-protocols/commit/1a9eaf3e))
- Update target test builds to use external Catch2 ([554f022c](https://github.com/espressif/esp-protocols/commit/554f022c))
- Fix arguments names when spawn esp_modem_xxx declarations ([b6792c52](https://github.com/espressif/esp-protocols/commit/b6792c52))
- Remove catch dependency ([c3480768](https://github.com/espressif/esp-protocols/commit/c3480768))
- Examples: use local configs for MQTT topic/data ([f5c13b92](https://github.com/espressif/esp-protocols/commit/f5c13b92))
- Fixed clang-tidy warnings ([70fa3af7](https://github.com/espressif/esp-protocols/commit/70fa3af7))
- Fix CI build per IDFv5.3 ([d0c17ef0](https://github.com/espressif/esp-protocols/commit/d0c17ef0))
- Fixed UART task to check for buffered data periodically ([4bdd90cc](https://github.com/espressif/esp-protocols/commit/4bdd90cc), [#536](https://github.com/espressif/esp-protocols/issues/536))
- Cleanup unused configs from PPPoS example ([08a62ccc](https://github.com/espressif/esp-protocols/commit/08a62ccc))
- Update CMUX example with SIM7070_gnss cleaned-up ([56fe5327](https://github.com/espressif/esp-protocols/commit/56fe5327))
- Update console example with SIM7070_gnss format comments ([5baaf542](https://github.com/espressif/esp-protocols/commit/5baaf542))
- Fix remaining print format warnings ([3b80181d](https://github.com/espressif/esp-protocols/commit/3b80181d))

### Updated

- docs(modem): Fix esp_modem_at_raw() description (C-API) ([492a6a00](https://github.com/espressif/esp-protocols/commit/492a6a00))
- ci(common): updated github actions(checkout, upload, download) v3 to 4, Ubuntu 20.04 to v22.04 ([a23a0027](https://github.com/espressif/esp-protocols/commit/a23a0027))

## [1.1.0](https://github.com/espressif/esp-protocols/commits/modem-v1.1.0)

### Features

- Added support for at_raw() command ([ae38110](https://github.com/espressif/esp-protocols/commit/ae38110), [#471](https://github.com/espressif/esp-protocols/issues/471))
- Added iperf test for PPP netifs ([976e98d](https://github.com/espressif/esp-protocols/commit/976e98d))
- Added test that performs OTA to exercise modem layers ([f2223dd](https://github.com/espressif/esp-protocols/commit/f2223dd))

### Bug Fixes

- Fixed OTA test to gracefully fail with no verification ([1dc4299](https://github.com/espressif/esp-protocols/commit/1dc4299))
- Added C-API to configure APN ([ce7dadd](https://github.com/espressif/esp-protocols/commit/ce7dadd), [#485](https://github.com/espressif/esp-protocols/issues/485))
- Fixed AT commands to copy strings to prevent overrides ([741d166](https://github.com/espressif/esp-protocols/commit/741d166), [#463](https://github.com/espressif/esp-protocols/issues/463))
- Fixed incorrect dial command format ([0998f3d](https://github.com/espressif/esp-protocols/commit/0998f3d), [#433](https://github.com/espressif/esp-protocols/issues/433))
- Fixed documentation and example on creating custom device ([577de67](https://github.com/espressif/esp-protocols/commit/577de67), [#452](https://github.com/espressif/esp-protocols/issues/452))
- Removed CI jobs for IDF v4.2 ([d88cd61](https://github.com/espressif/esp-protocols/commit/d88cd61))
- Fixed OAT test to verify server cert and CN ([edc3e72](https://github.com/espressif/esp-protocols/commit/edc3e72))
- Fixed set_pdp_context() command timeout ([1d80cbc](https://github.com/espressif/esp-protocols/commit/1d80cbc), [#455](https://github.com/espressif/esp-protocols/issues/455))

### Updated

- docs(modem): Added description of manual test procedure ([68ce794](https://github.com/espressif/esp-protocols/commit/68ce794))

## [1.0.5](https://github.com/espressif/esp-protocols/commits/modem-v1.0.5)

### Major changes

- Added support for implementing user defined modules in standard C-API ([Support for custom modules with C-API](https://github.com/espressif/esp-protocols/commit/0254d50))

### Bug Fixes

- Added test injecting unexpected replies ([b220d1e](https://github.com/espressif/esp-protocols/commit/b220d1e), [#426](https://github.com/espressif/esp-protocols/issues/426))
- Fixed inconsistent state on data after OK ([bf99f28](https://github.com/espressif/esp-protocols/commit/bf99f28), [#426](https://github.com/espressif/esp-protocols/issues/426))
- TLS example: Added restore session support in mbedtls-wrap ([79d38e5](https://github.com/espressif/esp-protocols/commit/79d38e5))
- Fixed examples to show netif on ppp-changed event ([a70b197](https://github.com/espressif/esp-protocols/commit/a70b197))
- remove unused GNU Make based buildsystem files ([d6b6f63](https://github.com/espressif/esp-protocols/commit/d6b6f63))
- specify override_path in example manifest files ([5b78da4](https://github.com/espressif/esp-protocols/commit/5b78da4))
- Added test-cases that exercise mode transitions ([aff571d](https://github.com/espressif/esp-protocols/commit/aff571d))
- Fixed mode transitions between any state and UNDEF ([93cb2ca](https://github.com/espressif/esp-protocols/commit/93cb2ca), [#320](https://github.com/espressif/esp-protocols/issues/320))
- Fixed API docs within doxygen comments ([020b407](https://github.com/espressif/esp-protocols/commit/020b407))
- Support for custom modules with C-API ([0254d50](https://github.com/espressif/esp-protocols/commit/0254d50))
- Fix CRLF issue with esp_modem_c_api.cpp ([2661b4d](https://github.com/espressif/esp-protocols/commit/2661b4d))

### Updated

- ci(common): Created reusable action for host and coverage tests ([9ad04de](https://github.com/espressif/esp-protocols/commit/9ad04de))

## [1.0.4](https://github.com/espressif/esp-protocols/commits/modem-v1.0.4)

### Bug Fixes

- Added USB runner with ESP32S2 with A7670 ([edeb936](https://github.com/espressif/esp-protocols/commit/edeb936))
- Extend docs on AT client example ([1f2ceed](https://github.com/espressif/esp-protocols/commit/1f2ceed))
- Fix pytest exclusion, gitignore, and changelog checks ([2696221](https://github.com/espressif/esp-protocols/commit/2696221))
- Fix DTE to post fragments to parsers for USB term ([1db1e15](https://github.com/espressif/esp-protocols/commit/1db1e15))
- Fix DUAL_MODE regression from cb6e03ac ([2aada0f](https://github.com/espressif/esp-protocols/commit/2aada0f))
- Fix AT client example to use custom AT processing ([1a5ba98](https://github.com/espressif/esp-protocols/commit/1a5ba98), [#352](https://github.com/espressif/esp-protocols/issues/352))

## [1.0.3](https://github.com/espressif/esp-protocols/commits/modem-v1.0.3)

### Bug Fixes

- Fix to allow MSC frame (SIM800 CMUX) after v1.0.2 ([8d5947e](https://github.com/espressif/esp-protocols/commit/8d5947e), [#366](https://github.com/espressif/esp-protocols/issues/366))
- Add CMUX example to target tests ([4f2ebaa](https://github.com/espressif/esp-protocols/commit/4f2ebaa))

## [1.0.2](https://github.com/espressif/esp-protocols/commits/modem-v1.0.2)

### Features

- Add factory method for simple creation of custom DCEs ([4cf9e50](https://github.com/espressif/esp-protocols/commit/4cf9e50))
- Support custom transport in AT TCP client example ([ae629ed](https://github.com/espressif/esp-protocols/commit/ae629ed))

### Bug Fixes

- Fix host test race with async read and d'structor ([c89d42f](https://github.com/espressif/esp-protocols/commit/c89d42f))
- More error handling in cmux protocol ([8edbac6](https://github.com/espressif/esp-protocols/commit/8edbac6))
- Fix netif to set PPP config in C++ way ([5287432](https://github.com/espressif/esp-protocols/commit/5287432))
- Fix vfs terminal not to reset read_cb() automatically ([25a35e2](https://github.com/espressif/esp-protocols/commit/25a35e2))
- Fix netif data race causing PPP startup delays ([c8c0507](https://github.com/espressif/esp-protocols/commit/c8c0507), [#308](https://github.com/espressif/esp-protocols/issues/308))
- Added support for inflatable buffer ([cb6e03a](https://github.com/espressif/esp-protocols/commit/cb6e03a), [#272](https://github.com/espressif/esp-protocols/issues/272))
- Fix LoadProhibited after failed CMUX initialization (IDFGH-10845) ([60c87dd](https://github.com/espressif/esp-protocols/commit/60c87dd))

## [1.0.1](https://github.com/espressif/esp-protocols/commits/modem-v1.0.1)

### Bug Fixes

- Support UART clean build with IDF v5.2 ([e620eb5](https://github.com/espressif/esp-protocols/commit/e620eb5))
- enable code coverage publishing to github pages ([4910e89](https://github.com/espressif/esp-protocols/commit/4910e89))
- fix esp_modem build issue ([ab94566](https://github.com/espressif/esp-protocols/commit/ab94566))
- Example to use 1.0.0 ([afb6930](https://github.com/espressif/esp-protocols/commit/afb6930))
- Changelog to correctly pick references ([423e965](https://github.com/espressif/esp-protocols/commit/423e965))

### Updated

- docs(esp_modem): updated documents to show missed topics ([0534853](https://github.com/espressif/esp-protocols/commit/0534853))
- docs(common): improving documentation ([ca3fce0](https://github.com/espressif/esp-protocols/commit/ca3fce0))

## [1.0.0](https://github.com/espressif/esp-protocols/commits/modem-v1.0.0)

### Major changes

- Enable DTE to redefine on_read() and write(cmd) directly ([DTE to support sending and receiving raw data](https://github.com/espressif/esp-protocols/commit/bf114d3))

### Features

- Add mqtt example in AT-only mode ([7547267](https://github.com/espressif/esp-protocols/commit/7547267))
- DTE to support sending and receiving raw data ([bf114d3](https://github.com/espressif/esp-protocols/commit/bf114d3))

### Bug Fixes

- Changelog to correctly pick references ([c59e330](https://github.com/espressif/esp-protocols/commit/c59e330))
- AT-only example: support MQTT over TLS on BG96 ([31d4323](https://github.com/espressif/esp-protocols/commit/31d4323))
- AT-only example: initial adaptation to BG96 ([ceedcfc](https://github.com/espressif/esp-protocols/commit/ceedcfc))
- warning in ap-2-pppos example when using lwip/napt ([bf32e45](https://github.com/espressif/esp-protocols/commit/bf32e45))
- trigger PR jobs based on labels ([13eca12](https://github.com/espressif/esp-protocols/commit/13eca12))
- Reintroduce missing CHANGELOGs ([200cbb3](https://github.com/espressif/esp-protocols/commit/200cbb3), [#235](https://github.com/espressif/esp-protocols/issues/235))
- Use default UART CLK source ([d0814bf](https://github.com/espressif/esp-protocols/commit/d0814bf), [#241](https://github.com/espressif/esp-protocols/issues/241))
- run CI build job for all targets ([a089e0d](https://github.com/espressif/esp-protocols/commit/a089e0d))

### Updated

- Update examples for USB v1.1 ([610372f](https://github.com/espressif/esp-protocols/commit/610372f))


## [0.1.28](https://github.com/espressif/esp-protocols/commits/01c26c8)

### Updated

- Dual DTE support ([01c26c8](https://github.com/espressif/esp-protocols/commit/01c26c8))


## [0.1.27](https://github.com/espressif/esp-protocols/commits/44bae24)

### Updated

- Return true from on_data callback in data mode ([44bae24](https://github.com/espressif/esp-protocols/commit/44bae24))


## [0.1.26](https://github.com/espressif/esp-protocols/commits/05fff94)

### Features

- Added target test ([4314c78](https://github.com/espressif/esp-protocols/commit/4314c78))
- Add support for manual CMUX operations ([ac5d438](https://github.com/espressif/esp-protocols/commit/ac5d438), [#168](https://github.com/espressif/esp-protocols/issues/168))

### Bug Fixes

- Example to use variable mqtt topic/data ([8958d5e](https://github.com/espressif/esp-protocols/commit/8958d5e))
- Fix cmux client compilation issue ([f71192b](https://github.com/espressif/esp-protocols/commit/f71192b))
- Exit data mode only after state change ([9a7bd90](https://github.com/espressif/esp-protocols/commit/9a7bd90))
- Support AT with callback in C-API ([2180ab1](https://github.com/espressif/esp-protocols/commit/2180ab1), [#143](https://github.com/espressif/esp-protocols/issues/143))

### Updated

- update(esp_modem): Bump component version to 0.1.26 ([05fff94](https://github.com/espressif/esp-protocols/commit/05fff94), [#213](https://github.com/espressif/esp-protocols/issues/213))
- Added the missing definition for the esp_modem_set_baud function (IDFGH-9181) (#209) ([0e215b1](https://github.com/espressif/esp-protocols/commit/0e215b1))
- CI: Added coverage analyzer for esp_modem host tests ([6b684ce](https://github.com/espressif/esp-protocols/commit/6b684ce))
- Update esp_modem_command_library.cpp ([5304a6e](https://github.com/espressif/esp-protocols/commit/5304a6e))
- Added badges with version of components to the respective README files ([e4c8a59](https://github.com/espressif/esp-protocols/commit/e4c8a59))
- esp_modem/examples: Add an example to synchronise PSM sleep in Sim70XX modem and esp32. ([1ca1391](https://github.com/espressif/esp-protocols/commit/1ca1391))


## [0.1.25](https://github.com/espressif/esp-protocols/commits/f1ae14f)

### Bug Fixes

- Cleanup custom lib-commands and factory ([65c0e0e](https://github.com/espressif/esp-protocols/commit/65c0e0e))
- Examples to configure MQTT Broker ([71a2388](https://github.com/espressif/esp-protocols/commit/71a2388))
- Extend pppos example project config ([ebc36a3](https://github.com/espressif/esp-protocols/commit/ebc36a3))
- Uart Terminal read_cb race ([a02bf05](https://github.com/espressif/esp-protocols/commit/a02bf05))

### Updated

- update(esp_modem): Bump component version ([f1ae14f](https://github.com/espressif/esp-protocols/commit/f1ae14f))
- fix(esp_modem) Add example that reads GNSS info ([652314e](https://github.com/espressif/esp-protocols/commit/652314e))
- CI: fixing the files to be complient with pre-commit hooks ([945bd17](https://github.com/espressif/esp-protocols/commit/945bd17))
- Quick fix esp_modem: Added modem_console commands for deep-sleep and modem PSM. ([cf504ec](https://github.com/espressif/esp-protocols/commit/cf504ec))
- fix(esp-modem) Make simple_cmux_client example configurable ([511ed54](https://github.com/espressif/esp-protocols/commit/511ed54))
- Added modem_console commands for deep-sleep and modem PSM. ([85a2e25](https://github.com/espressif/esp-protocols/commit/85a2e25))
- ci: Compile esp_modem examples with USB support ([988b3f9](https://github.com/espressif/esp-protocols/commit/988b3f9))
- Expand PPPoS example with USB DTE ([53b5933](https://github.com/espressif/esp-protocols/commit/53b5933))
- Allow USB DTE reconnection ([bf84ae9](https://github.com/espressif/esp-protocols/commit/bf84ae9))
- Add ConsoleCommand destructor ([a89a0ab](https://github.com/espressif/esp-protocols/commit/a89a0ab))
- esp_modem_get_gnss_power_mode (#136) ([fe536e4](https://github.com/espressif/esp-protocols/commit/fe536e4))
- remove unused Config Parameters ! ([55b4775](https://github.com/espressif/esp-protocols/commit/55b4775))


## [0.1.24](https://github.com/espressif/esp-protocols/commits/9b48b0a)

### Features

- Make some CMUX params compile-time configurable ([25ac2d9](https://github.com/espressif/esp-protocols/commit/25ac2d9))

### Bug Fixes

- CMUX to ignore MSC frames ([ce175df](https://github.com/espressif/esp-protocols/commit/ce175df), [#140](https://github.com/espressif/esp-protocols/issues/140))
- Fix CRLF endlines in examples ([07a347f](https://github.com/espressif/esp-protocols/commit/07a347f))
- Console example to use configurable flow-ctrl ([d9c9681](https://github.com/espressif/esp-protocols/commit/d9c9681))

### Updated

- update(esp_modem): Bump component version ([9b48b0a](https://github.com/espressif/esp-protocols/commit/9b48b0a))
- Allow error handler register in C ([62cb235](https://github.com/espressif/esp-protocols/commit/62cb235))


## [0.1.23](https://github.com/espressif/esp-protocols/commits/04c711f)

### Features

- Add exit PPP in example ([290197c](https://github.com/espressif/esp-protocols/commit/290197c))

### Updated

- Move common C definitions in to separate header ([04c711f](https://github.com/espressif/esp-protocols/commit/04c711f))
- Minor formatting fix and spelling ([accf924](https://github.com/espressif/esp-protocols/commit/accf924))
- extended the modem_console example.  (#120) ([ef0e48a](https://github.com/espressif/esp-protocols/commit/ef0e48a))


## [0.1.22](https://github.com/espressif/esp-protocols/commits/187ef76)

### Bug Fixes

- DTE command race of timeout vs reply's signal ([a871473](https://github.com/espressif/esp-protocols/commit/a871473), [#110](https://github.com/espressif/esp-protocols/issues/110))
- Correct timeouts for certain commands ([1029078](https://github.com/espressif/esp-protocols/commit/1029078), [#129](https://github.com/espressif/esp-protocols/issues/129))
- Make get_operator_name() return also ACT value ([0015e54](https://github.com/espressif/esp-protocols/commit/0015e54), [#128](https://github.com/espressif/esp-protocols/issues/128))

### Updated

- update(esp_modem): Bump component version ([187ef76](https://github.com/espressif/esp-protocols/commit/187ef76))


## [0.1.21](https://github.com/espressif/esp-protocols/commits/d07237b)

### Updated

- Fix IDF version resolution ([d07237b](https://github.com/espressif/esp-protocols/commit/d07237b))
- Fix format warnings ([98bf3ef](https://github.com/espressif/esp-protocols/commit/98bf3ef), [#79](https://github.com/espressif/esp-protocols/issues/79))


## [0.1.20](https://github.com/espressif/esp-protocols/commits/ae8479c)

### Bug Fixes

- Correct exit of CMUX mode ([2099434](https://github.com/espressif/esp-protocols/commit/2099434), [#103](https://github.com/espressif/esp-protocols/issues/103))
- Add filename/line info to exception message ([89e1bd2](https://github.com/espressif/esp-protocols/commit/89e1bd2))

### Updated

- Expose set_error_cb method ([ae8479c](https://github.com/espressif/esp-protocols/commit/ae8479c))


## [0.1.19](https://github.com/espressif/esp-protocols/commits/469f953)

### Features

- Add optional ACT to operator-name ([a286634](https://github.com/espressif/esp-protocols/commit/a286634), [#80](https://github.com/espressif/esp-protocols/issues/80))

### Bug Fixes

- Add missing set_pdp_context() to C-API ([4980ac8](https://github.com/espressif/esp-protocols/commit/4980ac8))

### Updated

- updated package version to 0.1.19 ([469f953](https://github.com/espressif/esp-protocols/commit/469f953))
- CI: Fix build issues ([6e4e4fa](https://github.com/espressif/esp-protocols/commit/6e4e4fa))
- esp_err_to_name (IDFGH-7793) (#78) ([d1129f3](https://github.com/espressif/esp-protocols/commit/d1129f3))


## [0.1.18](https://github.com/espressif/esp-protocols/commits/e8145fc)

### Features

- Add CMUX mode to C-API ([3fd4391](https://github.com/espressif/esp-protocols/commit/3fd4391), [#41](https://github.com/espressif/esp-protocols/issues/41))
- Add support to CMUX exit ([a16aab6](https://github.com/espressif/esp-protocols/commit/a16aab6), [#37](https://github.com/espressif/esp-protocols/issues/37))

### Bug Fixes

- Update ap2ppp example to recover network on disconnection ([a243d7e](https://github.com/espressif/esp-protocols/commit/a243d7e), [#44](https://github.com/espressif/esp-protocols/issues/44))
- Implement movable unique_buffer to bundle data, size, ptr ([66e6d4c](https://github.com/espressif/esp-protocols/commit/66e6d4c))
- DTE should own both command and data terminal ([f3ff98b](https://github.com/espressif/esp-protocols/commit/f3ff98b))
- Update CMUX example to use CMUX mode automatically ([58a0b57](https://github.com/espressif/esp-protocols/commit/58a0b57))
- Improve PPP exit sequence ([1e0aefd](https://github.com/espressif/esp-protocols/commit/1e0aefd), [#47](https://github.com/espressif/esp-protocols/issues/47))

### Updated

- Ignore format warnings ([e8145fc](https://github.com/espressif/esp-protocols/commit/e8145fc))
- Allow to customize uart source_clk ([d723fb7](https://github.com/espressif/esp-protocols/commit/d723fb7))
- Add "at" api for custom commands ([0cf08fb](https://github.com/espressif/esp-protocols/commit/0cf08fb))


## [0.1.17](https://github.com/espressif/esp-protocols/commits/5addf9e)

### Features

- Bumped version number to 0.1.17 ([5addf9e](https://github.com/espressif/esp-protocols/commit/5addf9e))

### Bug Fixes

- Support 2 byte size packets ([128c0a2](https://github.com/espressif/esp-protocols/commit/128c0a2), [#46](https://github.com/espressif/esp-protocols/issues/46))
- Correction of switching to CMUX ([503218b](https://github.com/espressif/esp-protocols/commit/503218b), [#33](https://github.com/espressif/esp-protocols/issues/33))

### Updated

- Fix param description for get_operator_name() ([a661e51](https://github.com/espressif/esp-protocols/commit/a661e51))
- Update: Removed stringstream to decrease static sizes ([3ced2d9](https://github.com/espressif/esp-protocols/commit/3ced2d9))
- Update: Renamed list_in to be type-specific Update: Removed modem-specific comment ([ba88d7f](https://github.com/espressif/esp-protocols/commit/ba88d7f))
- Update: Changed tests to coincide with new initial esp_modem mode ([4f3c429](https://github.com/espressif/esp-protocols/commit/4f3c429))
- typo ([52de8f1](https://github.com/espressif/esp-protocols/commit/52de8f1))
- Update: Removed duplicate power down function Add: gnss power mode function ([e98cf16](https://github.com/espressif/esp-protocols/commit/e98cf16))
- Update: Formatting ([be3d2ec](https://github.com/espressif/esp-protocols/commit/be3d2ec))
- Add: Sim7600 extended support ([15ed885](https://github.com/espressif/esp-protocols/commit/15ed885))
- Add: Support for SIM7000 modules ([0733ea8](https://github.com/espressif/esp-protocols/commit/0733ea8))
- Update: modem_mode::UNDEF initially ([15cbc9b](https://github.com/espressif/esp-protocols/commit/15cbc9b))
- Add: sim7070 support ([bb7f198](https://github.com/espressif/esp-protocols/commit/bb7f198))
- Add: Expanded modem_command_library with more "standard" commands ([d3f7ea6](https://github.com/espressif/esp-protocols/commit/d3f7ea6))
- Fix DCE selection in ‘pppos_client’ example ([d191a72](https://github.com/espressif/esp-protocols/commit/d191a72))
- Fix AT command in ‘set_data_mode_sim8xx()’ for SIM800 modem device ([cc0d527](https://github.com/espressif/esp-protocols/commit/cc0d527))


## [0.1.16](https://github.com/espressif/esp-protocols/commits/74040cf)

### Updated

- Extend modem_console example with USB DTE ([74040cf](https://github.com/espressif/esp-protocols/commit/74040cf))


## [0.1.15](https://github.com/espressif/esp-protocols/commits/1f5eb39)

### Updated

- Fix IDFv5.0 include paths ([1f5eb39](https://github.com/espressif/esp-protocols/commit/1f5eb39))


## [0.1.14](https://github.com/espressif/esp-protocols/commits/745201b)

### Bug Fixes

- missing default statement in a switch in the modem_console example ([325a193](https://github.com/espressif/esp-protocols/commit/325a193))
- add virtual destructors to ModuleIf and CommandableIf ([face03e](https://github.com/espressif/esp-protocols/commit/face03e))

### Updated

- build: fix issue with passing cxx_std option, a common workaround ([745201b](https://github.com/espressif/esp-protocols/commit/745201b))
- add(esp_modem): Add unit test to check polymorphic delete ([e54b240](https://github.com/espressif/esp-protocols/commit/e54b240))
- Implement esp_modem_sync for the C API ([8b3d420](https://github.com/espressif/esp-protocols/commit/8b3d420))


## [0.1.13](https://github.com/espressif/esp-protocols/commits/2cb74cf)

### Bug Fixes

- Update the test to exersise CBC parser ([d879e82](https://github.com/espressif/esp-protocols/commit/d879e82))
- Fix battery status parse ([4f1d31f](https://github.com/espressif/esp-protocols/commit/4f1d31f))
- Read module name with AT commands ([8417e23](https://github.com/espressif/esp-protocols/commit/8417e23))
- linux port to work with lwip-2.1.2 ([8465b14](https://github.com/espressif/esp-protocols/commit/8465b14))

### Updated

- Update(esp_modem): Bump component version ([2cb74cf](https://github.com/espressif/esp-protocols/commit/2cb74cf))


## [0.1.12](https://github.com/espressif/esp-protocols/commits/5d9ad9c)

### Updated

- Update FreeRTOS EventQueueHandle_t forward declaration ([5d9ad9c](https://github.com/espressif/esp-protocols/commit/5d9ad9c))


## [0.1.11](https://github.com/espressif/esp-protocols/commits/9649876)

### Updated

- New version to publish to component registry ([9649876](https://github.com/espressif/esp-protocols/commit/9649876))
- Docs: Add links to GitHub pages ([cf990d1](https://github.com/espressif/esp-protocols/commit/cf990d1))
- Add missing AT commands to plain C-API ([5299b42](https://github.com/espressif/esp-protocols/commit/5299b42))
- CI/Docs: Generate docs locally before uploading to registry ([2c21aa1](https://github.com/espressif/esp-protocols/commit/2c21aa1))
- CI/Docs: Add jobs to deploy component docs ([b9ff1e4](https://github.com/espressif/esp-protocols/commit/b9ff1e4))
- Update Task handle and Event Group handle to match freertos v10.4.3 ([5888717](https://github.com/espressif/esp-protocols/commit/5888717))
- Examples: Fix print format for http_client() API ([01e2a9c](https://github.com/espressif/esp-protocols/commit/01e2a9c))
- Examples: Fix build error when SIM needs no PIN ([c8c24ed](https://github.com/espressif/esp-protocols/commit/c8c24ed))
- Examples: Use bigger 4M flash for OTA example ([b380ded](https://github.com/espressif/esp-protocols/commit/b380ded))


## [0.1.9](https://github.com/espressif/esp-protocols/commits/2a2d270)

### Updated

- Fix CMake3.5 build with no cxx_std_17 feature ([2a2d270](https://github.com/espressif/esp-protocols/commit/2a2d270))


## [0.1.8](https://github.com/espressif/esp-protocols/commits/9dd1bd5)

### Updated

- 0.1.8: Update comonent version ([9dd1bd5](https://github.com/espressif/esp-protocols/commit/9dd1bd5))
- Add pre upload script to deploy the docs before component upload ([918db0d](https://github.com/espressif/esp-protocols/commit/918db0d))
- Minor correction of the docs ([c1b1330](https://github.com/espressif/esp-protocols/commit/c1b1330))
- Examples/console: Remove unnecessary event-base check on IP handler ([fb7295e](https://github.com/espressif/esp-protocols/commit/fb7295e))
- Examples: Extend CMux client default stack size for the main task ([8236b3d](https://github.com/espressif/esp-protocols/commit/8236b3d))
- CMUX: Refactor the protocol decoder to multiple methods per cmux state ([fb6029b](https://github.com/espressif/esp-protocols/commit/fb6029b))
- Factory: Rename Builder class to Creator, since it's not 100% builder pattern ([6e34954](https://github.com/espressif/esp-protocols/commit/6e34954))
- Formal  updates per code review ([148a930](https://github.com/espressif/esp-protocols/commit/148a930))
- Docs: Update CMux collaboration diagram to resemble composite ([1fb9150](https://github.com/espressif/esp-protocols/commit/1fb9150))
- Applied astyle code formatting ([a61e9e2](https://github.com/espressif/esp-protocols/commit/a61e9e2))
- Minor corrections per code review ([dc64f86](https://github.com/espressif/esp-protocols/commit/dc64f86))
- Bulk replace header guards to use ([3332c27](https://github.com/espressif/esp-protocols/commit/3332c27))
- Host test fix: renamed Loopback inherited member ([84b0dcf](https://github.com/espressif/esp-protocols/commit/84b0dcf))
- Update documentation and minor fixes ([e0e6585](https://github.com/espressif/esp-protocols/commit/e0e6585))


## [0.1.7](https://github.com/espressif/esp-protocols/commits/28433de)

### Updated

- Update version to 0.1.7 -- support socket VFS ([28433de](https://github.com/espressif/esp-protocols/commit/28433de))
- Reworked decouple resources from the file system ([8f17a90](https://github.com/espressif/esp-protocols/commit/8f17a90))
- Make the component compatible with IDFv4.1, v4.2, v4.3 ([69ad3ea](https://github.com/espressif/esp-protocols/commit/69ad3ea))


## [0.1.6](https://github.com/espressif/esp-protocols/commits/a67d749)

### Updated

- Bump the version and upload the component ([a67d749](https://github.com/espressif/esp-protocols/commit/a67d749))
- Updated per review comments ([ab93c13](https://github.com/espressif/esp-protocols/commit/ab93c13))


## [0.1.5](https://github.com/espressif/esp-protocols/commits/90641c8)

### Updated

- Moved to component folder ([90641c8](https://github.com/espressif/esp-protocols/commit/90641c8))
