#pragma once

#include "pch.h"

namespace pl {

  // Outputs verbose output info and flushes the buffer
  inline void debug(std::string msg) {
    std::cout << msg << std::flush;
  }

  // Outputs non-verbose output info and flushes the buffer
  inline void info(std::string msg) {
    std::cout << msg << std::flush;
  }

  // Outputs non-verbose output info and flushes the buffer
  inline void error(std::string msg) {
    std::cerr << "\x1b[1;31m" << msg << "\x1b[0m" << std::flush;
    exit(1);
  }

}
