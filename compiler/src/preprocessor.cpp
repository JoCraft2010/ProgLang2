#include "preprocessor.h"

pl::Preprocessor::Preprocessor(Tokenizer tokenizer, std::filesystem::path working_dir) {
  std::vector<Token> tl = tokenizer.get_token_list();

  for (size_t i = 0; i < tl.size(); i++) {
    if (tl.at(i).is_dollar()) {
      if (tl.at(i + 1).is_identifier() && tl.at(i + 1).data.at(0) == "include" && tl.at(i + 2).is_str_lit()) {
        std::vector<Token> ntl = Preprocessor(Tokenizer(working_dir / tl.at(i + 2).data.at(0)), working_dir)._get_token_list_silent();
        tl.erase(tl.begin(), tl.begin() + 2);
        token_list.insert(token_list.end(), ntl.begin(), ntl.end());
      }
      continue;
    }
    token_list.push_back(tl.at(i));
  }
}

std::vector<pl::Token> pl::Preprocessor::get_token_list() {
  for (Token& t : token_list) {
    debug(t.to_string() + "\n");
  }
  return token_list;
}

std::vector<pl::Token> pl::Preprocessor::_get_token_list_silent() {
  return token_list;
}
