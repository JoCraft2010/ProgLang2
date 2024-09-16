#include "tokenizer.h"

pl::Token pl::Token::from(std::string s) {
  if (s == "return") {
    return Token{ TokenType::RETURN, {} };
  } if (s == "int") {
    return Token{ TokenType::I32_T, {} };
  } if (std::string("1234567890").find(s.at(0)) != std::string::npos) {
    return Token{ TokenType::INT_LIT, { s } };
  }
  return Token{ TokenType::IDENTIFIER, { s } };
}

pl::Token pl::Token::from(char ch) {
  switch (ch) {
    case ';':
      return Token{ TokenType::SEMICOLON, {} };
    case '(':
      return Token{ TokenType::BR_OPEN, {} };
    case ')':
      return Token{ TokenType::BR_CLOSE, {} };
    case '{':
      return Token{ TokenType::CURL_OPEN, {} };
    case '}':
      return Token{ TokenType::CURL_CLOSE, {} };
    default:
      return Token{ TokenType::IDENTIFIER, { std::to_string(ch) } };
  }
}

bool pl::Token::is_identifier() { return token_type == IDENTIFIER; }
bool pl::Token::is_semicolon() { return token_type == SEMICOLON; }
bool pl::Token::is_br_open() { return token_type == BR_OPEN; }
bool pl::Token::is_br_close() { return token_type == BR_CLOSE; }
bool pl::Token::is_curl_open() { return token_type == CURL_OPEN; }
bool pl::Token::is_curl_close() { return token_type == CURL_CLOSE; }
bool pl::Token::is_return() { return token_type == RETURN; }
bool pl::Token::is_literal() { return token_type == INT_LIT; }
bool pl::Token::is_type() { return token_type == I32_T; }

std::string pl::Token::as_type() {
  switch (token_type) {
    case I32_T:
      return "i32";
    default:
      return "invalid";
  }
}

std::string pl::Token::to_string() {
  std::string s = token_type_names[token_type];
  for (std::string d : data) {
    s += " ; " + d;
  }
  return s;
}

pl::Tokenizer::Tokenizer(ParamData param_data) {
  std::ifstream in_file(param_data.in_path);

  if (!in_file.is_open()) {
    error("Input file couldn't be opened.\n");
  }

  char ch;
  std::string buf;
  while (in_file.get(ch)) {
    // Check for terminating characters
    if (std::string(" \t\n;(){}").find(ch) != std::string::npos) {
      if (!buf.empty()) {
        token_list.push_back(Token::from(buf));
      }
      if (std::string(";(){}").find(ch) != std::string::npos) {
        token_list.push_back(Token::from(ch));
      }
      buf = "";
    } else {
      buf += ch;
    }
  }

  in_file.close();
}

std::vector<pl::Token> pl::Tokenizer::get_token_list() {
  return token_list;
}
