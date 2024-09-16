#include "preprocessor.h"

pl::Preprocessor::Preprocessor(Tokenizer tokenizer) {
  token_list = tokenizer.get_token_list();

  for (Token t : token_list) {
    debug(t.to_string() + "\n");
  }
}

std::vector<pl::Token> pl::Preprocessor::get_token_list() {
  return token_list;
}
