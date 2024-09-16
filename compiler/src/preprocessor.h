#pragma once

#include "pch.h"

#include "tokenizer.h"

namespace pl {

  // Class responsible for preprocessing the code
  class Preprocessor {
  public:
    Preprocessor(Tokenizer);
    std::vector<Token> get_token_list();
  private:
    std::vector<Token> token_list;
  };

}
