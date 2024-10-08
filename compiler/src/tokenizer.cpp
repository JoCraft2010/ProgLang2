#include "tokenizer.h"

pl::Token pl::Token::from(std::string s, size_t l, size_t c) {
  if (s == "return") {
    return Token{ TokenType::RETURN, {}, l, c };
  } if (s == "bool") {
    return Token{ TokenType::I1_T, {}, l, c };
  }  if (s == "char") {
    return Token{ TokenType::I8_T, {}, l, c };
  } if (s == "int") {
    return Token{ TokenType::I32_T, {}, l, c };
  } if (s == "true") {
    return Token{ TokenType::INT_LIT, { "1" }, l, c };
  } if (s == "false") {
    return Token{ TokenType::INT_LIT, { "0" }, l, c };
  } if (std::string("1234567890").find(s.at(0)) != std::string::npos) {
    return Token{ TokenType::INT_LIT, { s }, l, c };
  }
  return Token{ TokenType::IDENTIFIER, { s }, l, c };
}

pl::Token pl::Token::from(char ch, size_t l, size_t c) {
  switch (ch) {
    case '$':
      return Token{ TokenType::DOLLAR, {}, l, c };
    case ';':
      return Token{ TokenType::SEMICOLON, {}, l, c };
    case ',':
      return Token{ TokenType::COMMA, {}, l, c };
    case '=':
      return Token{ TokenType::EQ, {}, l, c };
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
    case '~':
      return Token{ TokenType::TILDE, {}, l, c };
    case '+':
      return Token{ TokenType::PLUS, {}, l, c };
    case '-':
      return Token{ TokenType::MINUS, {}, l, c };
    case '*':
      return Token{ TokenType::ASTERISK, {}, l, c };
    case '/':
      return Token{ TokenType::SLASH, {}, l, c };
    default:
      return Token{ TokenType::IDENTIFIER, { std::to_string(ch) }, l, c };
  }
}

bool pl::Token::is_identifier() { return token_type == IDENTIFIER; }
bool pl::Token::is_dollar() { return token_type == DOLLAR; }
bool pl::Token::is_semicolon() { return token_type == SEMICOLON; }
bool pl::Token::is_comma() { return token_type == COMMA; }
bool pl::Token::is_eq() { return token_type == EQ; }
bool pl::Token::is_br_open() { return token_type == BR_OPEN; }
bool pl::Token::is_br_close() { return token_type == BR_CLOSE; }
bool pl::Token::is_curl_open() { return token_type == CURL_OPEN; }
bool pl::Token::is_curl_close() { return token_type == CURL_CLOSE; }
bool pl::Token::is_at() { return token_type == AT; }
bool pl::Token::is_tilde() { return token_type == TILDE; }
bool pl::Token::is_operator() { return token_type == PLUS || token_type == MINUS || token_type == ASTERISK || token_type == SLASH; }
bool pl::Token::is_plus() { return token_type == PLUS; }
bool pl::Token::is_minus() { return token_type == MINUS; }
bool pl::Token::is_asterisk() { return token_type == ASTERISK; }
bool pl::Token::is_slash() { return token_type == SLASH; }
bool pl::Token::is_return() { return token_type == RETURN; }
bool pl::Token::is_literal() { return token_type == INT_LIT || token_type == STR_LIT; }
bool pl::Token::is_int_lit() { return token_type == INT_LIT; }
bool pl::Token::is_str_lit() { return token_type == STR_LIT; }
bool pl::Token::is_type() { return token_type == I1_T || token_type == I8_T || token_type == I32_T; }

int pl::Token::as_operator_priority() {
  switch (token_type) {
    case PLUS:
    case MINUS:
      return 0;
    case ASTERISK:
    case SLASH:
      return 1;
    default:
      return -1;
  }
}

std::string pl::Token::as_type() {
  switch (token_type) {
    case TILDE:
      return "...";
    case I1_T:
      return "i1";
    case I8_T:
      return "i8";
    case I32_T:
      return "i32";
    default:
      error(to_string_no_data() + " is not a type.");
      return "";
  }
}

int pl::Token::as_alignment() {
  switch (token_type) {
    case I32_T:
      return 4;
    default:
      return 1;
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

pl::Tokenizer::Tokenizer(std::filesystem::path ifpath) {
  std::ifstream in_file(ifpath);

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

    // Check for \ ... comments
    if (buf.size() > 0 && buf.at(0) == '\\') {
      buf += ch;
      if (ch == '\n') {
        buf = "";
      }
      continue;
    }

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
    if (std::string(" \t\n$;,=(){}@~+-*/").find(ch) != std::string::npos) {
      if (!buf.empty()) {
        if (ch == '\n') {
          token_list.push_back(Token::from(buf, line, character));
        } else {
          token_list.push_back(Token::from(buf, line + 1, character));
        }
      }
      if (std::string("$;,=(){}@~+-*/").find(ch) != std::string::npos) {
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
