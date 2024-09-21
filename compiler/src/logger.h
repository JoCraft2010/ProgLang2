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

  // Outputs a non-verbose error and flushes the buffer
  [[deprecated]]
  inline void error(std::string msg) {
    std::cerr << "\x1b[1;31m" << msg << "\x1b[0m" << std::flush;
    exit(1);
  }

  // Outputs a non-verbose error including position of occurance and flushes the buffer
  inline void error(std::string msg, size_t line, size_t character) {
    std::cerr << "\x1b[1;31mError: \x1b[39m" << msg << " in line " << line << ".\x1b[0m\n" << std::flush;
    exit(1);
  }

}
