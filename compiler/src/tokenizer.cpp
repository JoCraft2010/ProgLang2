#include "tokenizer.h"

pl::Token pl::Token::from(std::string s, size_t l, size_t c) {
  if (s == "return") {
    return Token{ TokenType::RETURN, {}, l, c };
  } if (s == "char") {
    return Token{ TokenType::I8_T, {}, l, c };
  } if (s == "int") {
    return Token{ TokenType::I32_T, {}, l, c };
  } if (std::string("1234567890").find(s.at(0)) != std::string::npos) {
    return Token{ TokenType::INT_LIT, { s }, l, c };
  }
  return Token{ TokenType::IDENTIFIER, { s }, l, c };
}

pl::Token pl::Token::from(char ch, size_t l, size_t c) {
  switch (ch) {
    case ';':
      return Token{ TokenType::SEMICOLON, {}, l, c };
    case ',':
      return Token{ TokenType::COMMA, {}, l, c };
    case '(':
      return Token{ TokenType::BR_OPEN, {}, l, c };
    case ')':
      return Token{ TokenType::BR_CLOSE, {}, l, c };
    case '{':
      return Token{ TokenType::CURL_OPEN, {}, l, c };
    case '}':
      return Token{ TokenType::CURL_CLOSE, {}, l, c };
    case '@':
      return Token{ TokenType::AT, {}, l, c };
    default:
      return Token{ TokenType::IDENTIFIER, { std::to_string(ch) }, l, c };
  }
}

bool pl::Token::is_identifier() { return token_type == IDENTIFIER; }
bool pl::Token::is_semicolon() { return token_type == SEMICOLON; }
bool pl::Token::is_comma() { return token_type == COMMA; }
bool pl::Token::is_br_open() { return token_type == BR_OPEN; }
bool pl::Token::is_br_close() { return token_type == BR_CLOSE; }
bool pl::Token::is_curl_open() { return token_type == CURL_OPEN; }
bool pl::Token::is_curl_close() { return token_type == CURL_CLOSE; }
bool pl::Token::is_at() { return token_type == AT; }
bool pl::Token::is_return() { return token_type == RETURN; }
bool pl::Token::is_literal() { return token_type == INT_LIT || token_type == STR_LIT; }
bool pl::Token::is_int_lit() { return token_type == INT_LIT; }
bool pl::Token::is_str_lit() { return token_type == STR_LIT; }
bool pl::Token::is_type() { return token_type == I8_T || token_type == I32_T; }

std::string pl::Token::as_type() {
  switch (token_type) {
    case I8_T:
      return "i8";
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

std::string pl::Token::to_string_no_data() {
  return token_type_names[token_type];
}

pl::Tokenizer::Tokenizer(ParamData param_data) {
  std::ifstream in_file(param_data.in_path);

  if (!in_file.is_open()) {
    error("Input file couldn't be opened.");
  }

  char ch;
  std::string buf;
  size_t line = 0;
  size_t character = 0;
  while (in_file.get(ch)) {
    // Update line and character
    if (ch == '\n') {
      line++;
      character = 0;
    }
    character++;

    // Check for string literals
    if (buf.size() > 0 && buf.at(0) == '"') {
      if (ch == '"') {
        token_list.push_back(Token{ STR_LIT, { buf.substr(1) }, line, character });
        buf = "";
      } else {
        buf += ch;
      }
      continue;
    }

    // Check for terminating characters
    if (std::string(" \t\n;,(){}@").find(ch) != std::string::npos) {
      if (!buf.empty()) {
        if (ch == '\n') {
          token_list.push_back(Token::from(buf, line, character));
        } else {
          token_list.push_back(Token::from(buf, line + 1, character));
        }
      }
      if (std::string(";,(){}@").find(ch) != std::string::npos) {
        token_list.push_back(Token::from(ch, line, character));
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
