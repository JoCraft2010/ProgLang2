#pragma once

#include "pch.h"

#include "llvm_model.h"
#include "parser.h"

namespace pl {

  class LlvmBuilder {
  public:
    LlvmBuilder(Parser&);

    std::string build();
  private:
    Parser& parser;
  };

}
