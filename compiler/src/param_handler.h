#pragma once

#include "pch.h"

#include "logger.h"

namespace pl {

  // Struct storing all data that was decoded from CLI-arguments
  struct ParamData {

    std::filesystem::path in_path;
    std::filesystem::path out_path;

    // Factory function for ParamData
    static ParamData build(int, char*[]);

  };

}
