/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#define CATCH_CONFIG_MAIN // This tells the catch header to generate a main
#include <memory>
#include <future>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include "cxx_include/esp_modem_api.hpp"
#include "LoopbackTerm.h"
#include <iostream>

using namespace esp_modem;

TEST_CASE("DTE command races", "[esp_modem]")
{
    auto term = std::make_unique<LoopbackTerm>(true);
    auto loopback = term.get();
    auto dte = std::make_shared<DTE>(std::move(term));
    CHECK(term == nullptr);
    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG("APN");
    esp_netif_t netif{};
    auto dce = create_BG96_dce(&dce_config, dte, &netif);
    CHECK(dce != nullptr);
    uint8_t resp[] = {'O', 'K', '\n'};
    // run many commands in succession with the timeout set exactly to the timespan of injected reply
    // (checks for potential exception, data races, recycled local variables, etc.)
    for (int i = 0; i < 1000; ++i) {
        loopback->inject(&resp[0], sizeof(resp), sizeof(resp), /* 1ms before injecting reply */1, 0);
        auto ret = dce->command("AT\n", [&](uint8_t *data, size_t len) {
            return command_result::OK;
        }, 1);
        // this command should either timeout or finish successfully
        CHECK((ret == command_result::TIMEOUT || ret == command_result::OK));
    }

    // Now we test the same, but with some garbage after the expected data and inject the reply in chunks by 3 bytes
    uint8_t resp2[] = {'O', 'K', '\n', '1', '2', '\n'};
    for (int i = 0; i < 1000; ++i) {
        loopback->inject(&resp2[0], sizeof(resp2), 3, /* 1ms before injecting reply */0, 0);
        auto ret = dce->command("check\n", [&](uint8_t *data, size_t len) {
            if (len > 0 && data[0] == 'O') { // expected reply only when it starts with '0'
                return command_result::OK;
            }
            return esp_modem::command_result::TIMEOUT;
        }, 1);
        // this command should either timeout or finish successfully
        CHECK((ret == command_result::TIMEOUT || ret == command_result::OK));
    }
}

TEST_CASE("Test polymorphic delete for custom device/dte", "[esp_modem]")
{
    auto term = std::make_unique<LoopbackTerm>(true);
    auto dte = std::make_shared<DTE>(std::move(term));
    CHECK(term == nullptr);
    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG("APN");

    // Create custom device and DTE manually to check for a potential undefined behaviour
    auto device = new GenericModule(std::move(dte), &dce_config);
    device->power_down();
    delete device;
    auto custom_dte = new DTE(std::make_unique<LoopbackTerm>(false));
    custom_dte->command("AT", nullptr, 0);
    delete custom_dte;
}

TEST_CASE("DCE AT parser", "[esp_modem]")
{
    auto term = std::make_unique<LoopbackTerm>(true);
    auto dte =  std::make_shared<DTE>(std::move(term));
    CHECK(term == nullptr);
    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG("APN");
    esp_netif_t netif{};
    auto dce = create_BG96_dce(&dce_config, dte, &netif);
    CHECK(dce != nullptr);

    CHECK(dce->set_command_mode() == command_result::OK);

    int milli_volt, bcl, bcs;
    CHECK(dce->get_battery_status(milli_volt, bcl, bcs) == command_result::OK);
    CHECK(milli_volt == 123456);
    CHECK(bcl == 1);
    CHECK(bcs == 20);

    int rssi, ber;
    CHECK(dce->get_signal_quality(rssi, ber) == command_result::OK);
    CHECK(rssi == 123);
    CHECK(ber == 456);

    bool pin_ok;
    CHECK(dce->read_pin(pin_ok) == command_result::OK);
    CHECK(pin_ok == false);
    CHECK(dce->set_pin("1234") == command_result::OK);
    CHECK(dce->read_pin(pin_ok) == command_result::OK);
    CHECK(pin_ok == true);

    std::string model;
    CHECK(dce->get_module_name(model) == command_result::OK);
    CHECK(model == "0G Dummy Model");

    std::string operator_name;
    int act = 99;
    CHECK(dce->get_operator_name(operator_name) == command_result::OK);
    CHECK(operator_name == "OperatorName");
    CHECK(dce->get_operator_name(operator_name, act) == command_result::OK);
    CHECK(operator_name == "OperatorName");
    CHECK(act == 5);
}


TEST_CASE("DTE send/receive command", "[esp_modem]")
{
    auto term = std::make_unique<LoopbackTerm>();
    auto dte =  std::make_unique<DTE>(std::move(term));

    const auto test_command = "Test\n";
    CHECK(term == nullptr);

    CHECK(dte->set_mode(esp_modem::modem_mode::COMMAND_MODE) == true);

    auto ret = dte->command(test_command, [&](uint8_t *data, size_t len) {
        std::string response((char *)data, len);
        CHECK(response == test_command);
        return command_result::OK;
    }, 1000);
    CHECK(ret == command_result::OK);
}


TEST_CASE("DCE commands", "[esp_modem]")
{
    auto term = std::make_unique<LoopbackTerm>();
    auto dte =  std::make_shared<DTE>(std::move(term));
    CHECK(term == nullptr);

    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG("APN");
    esp_netif_t netif{};
    auto dce = create_SIM7600_dce(&dce_config, dte, &netif);
    CHECK(dce != nullptr);

    const auto test_command = "Test\n";
    auto ret = dce->command(test_command, [&](uint8_t *data, size_t len) {
        std::string response((char *)data, len);
        CHECK(response == test_command);
        return command_result::OK;
    }, 1000);
    CHECK(ret == command_result::OK);
}
TEST_CASE("DCE AT commands", "[esp_modem]")
{
    auto term = std::make_unique<LoopbackTerm>();
    auto dte =  std::make_shared<DTE>(std::move(term));
    CHECK(term == nullptr);

    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG("APN");
    esp_netif_t netif{};
    auto dce = create_SIM7600_dce(&dce_config, dte, &netif);
    CHECK(dce != nullptr);

    int milli_volt, bcl, bcs;
    CHECK(dce->set_echo(false) == command_result::OK);
    CHECK(dce->set_echo(true) == command_result::OK);
    CHECK(dce->get_battery_status(milli_volt, bcl, bcs) == command_result::OK);
    CHECK(milli_volt == 123456);
    CHECK(dce->resume_data_mode() == command_result::FAIL);
}


TEST_CASE("DCE modes", "[esp_modem]")
{
    auto term = std::make_unique<LoopbackTerm>();
    auto dte =  std::make_shared<DTE>(std::move(term));
    CHECK(term == nullptr);

    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG("APN");
    esp_netif_t netif{};
    auto dce = create_SIM7600_dce(&dce_config, dte, &netif);
    CHECK(dce != nullptr);

    // UNDER -> CMD (OK)
    CHECK(dce->set_mode(esp_modem::modem_mode::COMMAND_MODE) == true);
    // CMD -> CMD (Fail)
    CHECK(dce->set_mode(esp_modem::modem_mode::COMMAND_MODE) == false);
    // CMD -> DATA (OK)
    CHECK(dce->set_mode(esp_modem::modem_mode::DATA_MODE) == true);
    // DATA -> CMUX (Fail)
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MODE) == false);
    // DATA back -> CMD (OK)
    CHECK(dce->set_mode(esp_modem::modem_mode::COMMAND_MODE) == true);
    // CMD -> CMUX (OK)
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MODE) == true);
    // CMUX -> DATA (Fail)
    CHECK(dce->set_mode(esp_modem::modem_mode::DATA_MODE) == false);
    // CMUX back -> CMD (OK)
    CHECK(dce->set_mode(esp_modem::modem_mode::COMMAND_MODE) == true);
}

TEST_CASE("DCE CMUX test", "[esp_modem]")
{
    auto term = std::make_unique<LoopbackTerm>();
    auto dte = std::make_shared<DTE>(std::move(term));
    CHECK(term == nullptr);

    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG("APN");
    esp_netif_t netif{};
    auto dce = create_SIM7600_dce(&dce_config, dte, &netif);
    CHECK(dce != nullptr);

    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MODE) == true);
    const auto test_command = "Test\n";
    auto ret = dce->command(test_command, [&](uint8_t *data, size_t len) {
        std::string response((char *) data, len);
        std::cout << "Response:" << response << std::endl;
        CHECK(response == test_command);
        return command_result::OK;
    }, 1000);
    CHECK(ret == command_result::OK);
}

TEST_CASE("Test CMUX protocol by injecting payloads", "[esp_modem]")
{
    auto term = std::make_unique<LoopbackTerm>();
    auto loopback = term.get();
    auto dte = std::make_shared<DTE>(std::move(term));
    CHECK(term == nullptr);

    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG("APN");
    esp_netif_t netif{};
    auto dce = create_SIM7600_dce(&dce_config, dte, &netif);
    CHECK(dce != nullptr);

    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MODE) == true);
    const auto test_command = "Test\n";
    // 1 byte payload size
    uint8_t test_payload[] = {0xf9, 0x09, 0xff, 0x0b, 0x54, 0x65, 0x73, 0x74, 0x0a, 0xbb, 0xf9 };
    loopback->inject(&test_payload[0], sizeof(test_payload), 1);
    auto ret = dce->command(test_command, [&](uint8_t *data, size_t len) {
        std::string response((char *) data, len);
        CHECK(response == test_command);
        return command_result::OK;
    }, 1000);
    CHECK(ret == command_result::OK);

    // 2 byte payload size
    uint8_t long_payload[453] = { 0xf9, 0x09, 0xef, 0x7c, 0x03, 0x7e }; // header
    long_payload[5]   = 0x7e;   // payload to validate
    long_payload[449] = 0x7e;
    long_payload[450] = '\n';
    long_payload[451] = 0x53;   // footer
    long_payload[452] = 0xf9;
    for (int i = 0; i < 5; ++i) {
        // inject the whole payload (i=0) and then per 1,2,3,4 bytes (i)
        loopback->inject(&long_payload[0], sizeof(long_payload), i == 0 ? sizeof(long_payload) : i);
        auto ret = dce->command("ignore", [&](uint8_t *data, size_t len) {
            CHECK(data[0]     == 0x7e);
            CHECK(data[len - 2] == 0x7e);
            CHECK(data[len - 1] == '\n');
            return command_result::OK;
        }, 1000);
        CHECK(ret == command_result::OK);
    }
}

TEST_CASE("Command and Data mode transitions", "[esp_modem][transitions]")
{
    auto term = std::make_unique<LoopbackTerm>();
    auto loopback = term.get();
    auto dte =  std::make_shared<DTE>(std::move(term));
    CHECK(term == nullptr);

    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG("APN");
    esp_netif_t netif{};
    auto dce = create_SIM7600_dce(&dce_config, dte, &netif);
    CHECK(dce != nullptr);

    // UNDEF -> CMD (OK)
    uint8_t resp[] = "DISCONNECTED\n";
    loopback->inject(&resp[0], sizeof(resp), sizeof(resp), /* 10ms before injecting reply */100, 0);
    loopback->write(nullptr, 0); /* this triggers sending the injected response */
    CHECK(dce->set_mode(esp_modem::modem_mode::COMMAND_MODE) == true);
    loopback->inject(nullptr, 0, 0, 0, 0); /* reset injection, use synchronous replies now */
    // CMD -> CMD (Fail)
    CHECK(dce->set_mode(esp_modem::modem_mode::COMMAND_MODE) == false);

    // Forcing transition to CMD (via UNDEF)
    // CMD -> UNDEF (OK)
    CHECK(dce->set_mode(esp_modem::modem_mode::UNDEF) == true);
    // UNDEF -> CMD (OK)
    CHECK(dce->set_mode(esp_modem::modem_mode::COMMAND_MODE) == true);

    // CMD -> DATA (OK)
    CHECK(dce->set_mode(esp_modem::modem_mode::DATA_MODE) == true);
    // DATA -> CMD (OK)
    CHECK(dce->set_mode(esp_modem::modem_mode::COMMAND_MODE) == true);
}

TEST_CASE("CMUX mode transitions", "[esp_modem][transitions]")
{
    auto term = std::make_unique<LoopbackTerm>();
    auto dte = std::make_shared<DTE>(std::move(term));
    CHECK(term == nullptr);

    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG("APN");
    esp_netif_t netif{};
    auto dce = create_SIM7600_dce(&dce_config, dte, &netif);
    CHECK(dce != nullptr);
    // UNDEF -> CMUX (OK)
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MODE) == true);
    // CMUX -> DATA (Fail)
    CHECK(dce->set_mode(esp_modem::modem_mode::DATA_MODE) == false);
    // CMUX back -> CMD (OK)
    CHECK(dce->set_mode(esp_modem::modem_mode::COMMAND_MODE) == true);
}

TEST_CASE("CMUX manual mode transitions", "[esp_modem][transitions]")
{
    auto term = std::make_unique<LoopbackTerm>();
    auto dte = std::make_shared<DTE>(std::move(term));
    CHECK(term == nullptr);

    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG("APN");
    esp_netif_t netif{};
    auto dce = create_SIM7600_dce(&dce_config, dte, &netif);
    CHECK(dce != nullptr);

    // Happy flow transitions of Manual CMUX transitions
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_MODE) == true);
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_EXIT) == true);
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_MODE) == true);
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_SWAP) == true);
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_DATA) == true);
    // Cannot test CMUX_MANUAL_DATA -> CMUX_MANUAL_COMMAND with our mocked terminal for now
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_EXIT) == true);

    // Check some out of order manual transitions, most of them are allowed,
    // but some fail as modem layers report issues with specific steps
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_SWAP) == false); // cannot go directly to SWAP
    CHECK(dce->set_mode(esp_modem::modem_mode::UNDEF) == true);
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_SWAP) == true);  // can go via UNDEF
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_EXIT) == false); // EXIT is allowed, but CMUX terms don't exist
    CHECK(dce->set_mode(esp_modem::modem_mode::UNDEF) == true);
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_MODE) == true);  // Enter CMUX (via UNDEF)
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_DATA) == true);  // Go directly to DATA mode
    CHECK(dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_EXIT) == true);  // Exit CMUX
    CHECK(dce->set_mode(esp_modem::modem_mode::UNDEF) == true);             // Succeeds from any state

}

#define CATCH_CONFIG_RUNNER
extern "C" int app_main(void)
{
    // Define the argument count and arguments for Catch2, including JUnit reporting
    int argc = 5;
    const char *argv[] = {"esp_modem", "-r", "junit", "-o", "junit.xml", nullptr};

    // Run the Catch2 session and store the result
    int result = Catch::Session().run(argc, argv);

    // Use more descriptive error handling
    if (result != 0) {
        printf("Test failed with result %d.\n", result);
    } else {
        printf("All tests passed successfully.\n");
    }

    // Check for the junit.xml file in the current working directory
    // Exit the application with the test result as the status code
    std::exit(result);
}
