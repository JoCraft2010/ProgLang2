#pragma once

#include "pch.h"

#include "tokenizer.h"

namespace pl {

  // Class responsible for preprocessing the code
  class Preprocessor {
  public:
    Preprocessor(Tokenizer, std::filesystem::path);
    std::vector<Token> get_token_list();
    std::vector<Token> _get_token_list_silent();
  private:
    std::vector<Token> token_list;
  };

}
