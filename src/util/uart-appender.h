#pragma once

#include "logging.hpp"

class UARTAppender : public esp32m::FormattingAppender
{
public:
  UARTAppender(const UARTAppender &) = delete;
  static UARTAppender &instance();

private:
  UARTAppender();

protected:
  virtual bool append(const char *message);
};