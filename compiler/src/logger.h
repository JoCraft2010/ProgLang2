#pragma once

#include "pch.h"

namespace pl {

  // Outputs verbose output info and flushes the buffer
  void debug(std::string msg);

  // Outputs non-verbose output info and flushes the buffer
  void info(std::string msg);

  // Outputs a non-verbose error, flushes the buffer and exits with code 1
  void error(std::string msg);

  // Outputs a non-verbose error including position of occurance, flushes the buffer and exits with code 1
  void error(std::string msg, size_t line, size_t character);

}
