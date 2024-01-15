#include <string.h>

#include "util/uart-appender.h"

char *format(const esp32m::LogMessage *msg)
{
    static const char *levels = "??EWIDV";
    if (!msg)
        return nullptr;
    auto stamp = msg->stamp();
    char *buf = nullptr;
    auto level = msg->level();
    auto name = msg->name();
    char l = level >= 0 && level <= 6 ? levels[level] : '?';
    if (stamp < 0)
    {
        stamp = -stamp;
        char strftime_buf[32];
        time_t now = stamp / 1000;
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%F %T", &timeinfo);
        buf = (char *)malloc(strlen(strftime_buf) + 1 /*dot*/ + 4 /*millis*/ + 1 /*space*/ + 1 /*level*/ + 1 /*space*/ + strlen(name) + 2 /*spaces*/ + msg->message_size() + 1 /*zero*/);
        if(l == 'E'){
            sprintf(buf, LOG_COLOR_E "%s.%04d %c %s  %s" LOG_RESET_COLOR, strftime_buf, (int)(stamp % 1000), l, name, msg->message());
        } else if (l == 'W') {
            sprintf(buf, LOG_COLOR_W "%s.%04d %c %s  %s" LOG_RESET_COLOR, strftime_buf, (int)(stamp % 1000), l, name, msg->message());
        } else if (l == 'I') {
            sprintf(buf, LOG_COLOR_I "%s.%04d %c %s  %s" LOG_RESET_COLOR, strftime_buf, (int)(stamp % 1000), l, name, msg->message());
        } else if (l == 'D') {
            sprintf(buf, LOG_COLOR(LOG_COLOR_PURPLE) "%s.%04d %c %s  %s" LOG_RESET_COLOR, strftime_buf, (int)(stamp % 1000), l, name, msg->message());
        } else if (l == 'V') {
            sprintf(buf, LOG_COLOR(LOG_COLOR_CYAN) "%s.%04d %c %s  %s" LOG_RESET_COLOR, strftime_buf, (int)(stamp % 1000), l, name, msg->message());
        } sprintf(buf, LOG_COLOR(LOG_COLOR_BLUE) "%s.%04d %c %s  %s" LOG_RESET_COLOR, strftime_buf, (int)(stamp % 1000), l, name, msg->message());
    }
    else
    {
        int millis = stamp % 1000;
        stamp /= 1000;
        int seconds = stamp % 60;
        stamp /= 60;
        int minutes = stamp % 60;
        stamp /= 60;
        int hours = stamp % 24;
        int days = stamp / 24;
        buf = (char *)malloc(8 /*color*/ + 6 /*days*/ + 1 /*colon*/ + 2 /*hours*/ + 1 /*colon*/ + 2 /*minutes*/ + 1 /*colon*/ + 2 /*seconds*/ + 1 /*colon*/ + 4 /*millis*/ + 1 /*space*/ + 1 /*level*/ + 1 /*space*/ + strlen(name) + 2 /*spaces*/ + msg->message_size() + 1 /*zero*/ + 5 /*reset color*/);
        if(l == 'E'){
            sprintf(buf, LOG_COLOR_E "%d:%02d:%02d:%02d.%04d %c %s  %s" LOG_RESET_COLOR, days, hours, minutes, seconds, millis, l, name, msg->message());
        } else if (l == 'W') {
            sprintf(buf, LOG_COLOR_W "%d:%02d:%02d:%02d.%04d %c %s  %s" LOG_RESET_COLOR, days, hours, minutes, seconds, millis, l, name, msg->message());
        } else if (l == 'I') {
            sprintf(buf, LOG_COLOR_I "%d:%02d:%02d:%02d.%04d %c %s  %s" LOG_RESET_COLOR, days, hours, minutes, seconds, millis, l, name, msg->message());
        } else if (l == 'D') {
            sprintf(buf, LOG_COLOR(LOG_COLOR_PURPLE) "%d:%02d:%02d:%02d.%04d %c %s  %s" LOG_RESET_COLOR, days, hours, minutes, seconds, millis, l, name, msg->message());
        } else if (l == 'V') {
            sprintf(buf, LOG_COLOR(LOG_COLOR_CYAN) "%d:%02d:%02d:%02d.%04d %c %s  %s" LOG_RESET_COLOR, days, hours, minutes, seconds, millis, l, name, msg->message());
        } else sprintf(buf, LOG_COLOR(LOG_COLOR_BLUE) "%d:%02d:%02d:%02d.%04d %c %s  %s" LOG_RESET_COLOR, days, hours, minutes, seconds, millis, l, name, msg->message());
    }
    return buf;
}

UARTAppender &UARTAppender::instance()
{
    static UARTAppender i;
    return i;
}

UARTAppender::UARTAppender(): FormattingAppender(&format) {};

bool UARTAppender::append(const char *message)
{
    if (message)
    {
        auto l = strlen(message);
        for (auto i = 0; i < l; i++)
            ets_write_char_uart(message[i]);
        ets_write_char_uart('\n');
    }

    return true;
}