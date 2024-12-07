/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Modem console example
*/

#include "console_helper.hpp"
#include "esp_log.h"

static const char *TAG = "modem_console_helper";

ConsoleCommand::ConsoleCommand(const char *command, const char *help, const std::vector<CommandArgs> &args, std::function<bool(ConsoleCommand *)> f):
    func(std::move(f))
{
    RegisterCommand(command, help, args);
}

ConsoleCommand::~ConsoleCommand()
{
    // Find this command in static list of commands and remove it
    auto cmd = std::find(console_commands.begin(), console_commands.end(), this);
    if (cmd != console_commands.end()) {
        console_commands.erase(cmd);
        last_command--;
    }
}

void ConsoleCommand::RegisterCommand(const char *command, const char *help, const std::vector<CommandArgs> &args)
{
    assert(last_command <= MAX_REPEAT_NR);
    arg_type common_arg = { };
    for (auto &it : args) {
        switch (it.type) {
        case ARG_END:
            break;
        case STR0:
            common_arg.str = arg_str0(it.shortopts, it.longopts, it.datatype, it.glossary);
            break;
        case STR1:
            common_arg.str = arg_str1(it.shortopts, it.longopts, it.datatype, it.glossary);
            break;
        case INT0:
            common_arg.intx = arg_int0(it.shortopts, it.longopts, it.datatype, it.glossary);
            break;
        case INT1:
            common_arg.intx = arg_int1(it.shortopts, it.longopts, it.datatype, it.glossary);
            break;
        case LIT0:
            common_arg.lit = arg_lit0(it.shortopts, it.longopts, it.glossary);
            break;
        }
        if (common_arg.is_null()) {
            arg_table.emplace_back(common_arg);
        } else {
            ESP_LOGE(TAG, "Creating argument parser failed for %s", it.glossary);
            abort();
        }
    }

    arg_type end = { .end = arg_end(1) };
    arg_table.emplace_back(end);
    const esp_console_cmd_t command_def = {
        .command = command,
        .help = help,
        .hint = nullptr,
        .func = command_func_pts[last_command],
        .argtable = &arg_table[0]
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&command_def));
    last_command++;
    console_commands.emplace_back(this);
}

int ConsoleCommand::get_count(int index)
{
    return (arg_table[index].str)->count;
}

std::string ConsoleCommand::get_string(int index)
{
    if (get_count(index) > 0) {
        return std::string(arg_table[index].str->sval[0]);
    }
    return std::string();
}

int ConsoleCommand::get_int(int index)
{
    if (get_count(index) > 0) {
        return *(arg_table[index].intx)->ival;
    }
    return -1;
}


int ConsoleCommand::command_func(int argc, char **argv)
{
    arg_type *plain_arg_array = &arg_table[0];
    int nerrors = arg_parse(argc, argv, (void **)plain_arg_array);
    if (nerrors != 0) {
        arg_print_errors(stderr, arg_table.back().end, argv[0]);
        return 1;
    }
    if (func) {
        return func(this);
    }
    return 0;
}

/**
 * @brief This class holds definitions of static functions, numbered from 0 index,
 * that call indexed methods of ConsoleCommand (used to bridge from static esp-console
 * to object oriented ConsoleCommand class)
 */
class StaticCommands {
    friend class ConsoleCommand;
#define ITEM_TO_REPEAT(index) \
    static inline int command_func_ ## index(int argc, char **argv) \
        { return ConsoleCommand::console_commands[index]->command_func(argc, argv); }

    _DO_REPEAT_ITEM()

#undef ITEM_TO_REPEAT
};

/**
 * @brief ConsoleCommand list of static callbacks used for getting object context to plain esp-console context
 */
const esp_console_cmd_func_t ConsoleCommand::command_func_pts[] = {

#define ITEM_TO_REPEAT(index) StaticCommands::command_func_ ## index ,

    _DO_REPEAT_ITEM()

#undef  ITEM_TO_REPEAT
};

/**
 * @brief Static members defined for ConsoleCommand
 */
std::vector<ConsoleCommand *> ConsoleCommand::console_commands;
int ConsoleCommand::last_command = 0;
