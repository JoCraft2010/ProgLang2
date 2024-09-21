#include "logger.h"

void pl::debug(std::string msg) {
  std::cout << msg << std::flush;
}

void pl::info(std::string msg) {
  std::cout << msg << std::flush;
}

[[deprecated]]
void pl::error(std::string msg) {
  std::cerr << "\x1b[1;31m" << msg << "\x1b[0m" << std::flush;
  exit(1);
}

void pl::error(std::string msg, size_t line, size_t character) {
  std::cerr << "\x1b[1;31mError: \x1b[39m" << msg << " in line " << line << ".\x1b[0m\n" << std::flush;
  exit(1);
}
