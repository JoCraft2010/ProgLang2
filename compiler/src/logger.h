#pragma once

#include "pch.h"

namespace pl {

  // Outputs verbose output info and flushes the buffer
  void debug(std::string msg);

  // Outputs non-verbose output info and flushes the buffer
  void info(std::string msg);

  // Outputs a non-verbose error and flushes the buffer
  [[deprecated]]
  void error(std::string msg);

  // Outputs a non-verbose error including position of occurance and flushes the buffer
  void error(std::string msg, size_t line, size_t character);

}
