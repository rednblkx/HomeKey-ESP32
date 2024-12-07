/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Modem console example
*/

#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <esp_console.h>
#include "argtable3/argtable3.h"
#include "repeat_helper.inc"

#define MAX_REPEAT_NR 20

/**
 * @brief Argument types used for ConsoleCommand
 */
enum arg_type {
    STR0,
    STR1,
    INT0,
    INT1,
    LIT0,
    ARG_END,
};

/**
 * Command argument struct definition for list of arguments of one command
 */
struct CommandArgs {
    CommandArgs(arg_type t, const char *shopts, const char *lopts, const char *data, const char *glos):
        type(t), shortopts(shopts), longopts(lopts), datatype(data), glossary(glos) {}
    CommandArgs(arg_type t, const char *shopts, const char *lopts, const char *glos):
        type(t), shortopts(shopts), longopts(lopts), datatype(nullptr), glossary(glos) {}

    arg_type type;
    const char *shortopts;
    const char *longopts;
    const char *datatype;
    const char *glossary;
};

class StaticCommands;

/**
 * @brief This class simplifies console command definition in more object wise fashion
 */
class ConsoleCommand {
    /**
     * @brief Common argument types to be stored internally for parsing later
     */
    using arg_type =
        union {
                struct arg_int *intx;
                struct arg_str *str;
                struct arg_lit *lit;
                struct arg_end *end;
                void *__raw_ptr;
        bool is_null() const {
        return __raw_ptr;
    }
        };

    friend class StaticCommands;
public:
    /**
     * @brief This is how we define a generic Console command
     * @param command Textual console command
     * @param help Contextual help for the command
     * @param arg_struct List of argument struct
     * @param srg_struct_size Size of the argument struct
     * @param f Function callback for the command
     */
    template<typename T> explicit ConsoleCommand(const char *command, const char *help, const T *arg_struct, size_t srg_struct_size,
            std::function<bool(ConsoleCommand *)> f):   func(std::move(f))
    {
        size_t args_plain_size = srg_struct_size / sizeof(CommandArgs);
        auto first_arg = reinterpret_cast<const CommandArgs *>(arg_struct);
        std::vector<CommandArgs> args(first_arg, first_arg + args_plain_size);
        RegisterCommand(command, help, args);
    }

    /**
     * @brief Another method of Console command definitions using vector arg struct
     */
    explicit ConsoleCommand(const char *command, const char *help, const std::vector<CommandArgs> &args, std::function<bool(ConsoleCommand *)> f);

    /**
     * @brief Destructor of ConsoleCommand
     */
    ~ConsoleCommand();

    /**
     * @brief Utility getters of various params from the argument list
     */
    template<typename T> int get_count_of(CommandArgs T::*member)
    {
        return get_count(index_arg(member));
    }
    template<typename T> std::string get_string_of(CommandArgs T::*member)
    {
        return get_string(index_arg(member));
    }
    template<typename T> int get_int_of(CommandArgs T::*member)
    {
        return get_int(index_arg(member));
    }
    std::string get_string(int index);
    int get_int(int index);

private:
    int get_count(int index);
    void RegisterCommand(const char *command, const char *help, const std::vector<CommandArgs> &args);
    template<typename T> static constexpr size_t index_arg(CommandArgs T::*member)
    {
        return ((uint8_t *) & ((T *)nullptr->*member) - (uint8_t *)nullptr) / sizeof(CommandArgs);
    }
    std::vector<arg_type> arg_table;
    int command_func(int argc, char **argv);

    static int last_command;
    static std::vector<ConsoleCommand *> console_commands;
    std::function<bool(ConsoleCommand *)> func;
    const static esp_console_cmd_func_t command_func_pts[];
};
