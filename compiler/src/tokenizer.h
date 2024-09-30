#pragma once

#include "pch.h"

#include "logger.h"
#include "param_handler.h"

namespace pl {

  // Types of Tokens in the source code
  enum TokenType {
    // Identifier
    IDENTIFIER, // Data: Name
    // Punctuation
    DOLLAR,
    SEMICOLON,
    COMMA,
    EQ,
    BR_OPEN,
    BR_CLOSE,
    CURL_OPEN,
    CURL_CLOSE,
    AT,
    TILDE,
    // Operators
    PLUS,
    MINUS,
    ASTERISK,
    SLASH,
    // Keywords
    RETURN,
    // Literals
    INT_LIT, // Data: Value
    STR_LIT, // Data: Value
    // Primitive types
    I1_T,
    I8_T,
    I32_T
  };

  const std::string token_type_names[] = {
    "identifier",
    "dollar (preprocessor directive)",
    "semicolon",
    "comma",
    "equal sign",
    "opening bracket",
    "closing bracket",
    "opening curly bracket",
    "closing curly bracket",
    "at",
    "tilde",
    "plus",
    "minus",
    "asterisk",
    "slash",
    "return",
    "int literal",
    "string literal",
    "bool",
    "char",
    "int"
  };

  // Struct representing a single Token
  struct Token {
    TokenType token_type;
    std::vector<std::string> data;
    size_t line;
    size_t character;

    // Factory functions for Token
    static Token from(std::string, size_t, size_t);
    static Token from(char, size_t, size_t);

    // Functions for category checking
    bool is_identifier();
    bool is_dollar();
    bool is_semicolon();
    bool is_comma();
    bool is_eq();
    bool is_br_open();
    bool is_br_close();
    bool is_curl_open();
    bool is_curl_close();
    bool is_at();
    bool is_tilde();
    bool is_operator();
    bool is_plus();
    bool is_minus();
    bool is_asterisk();
    bool is_slash();
    bool is_return();
    bool is_literal();
    bool is_int_lit();
    bool is_str_lit();
    bool is_type();

    // Conversion functions
    int as_operator_priority();
    std::string as_type();
    int as_alignment();

    // Generation of string name for logging purposes
    std::string to_string();
    std::string to_string_no_data();
  };

  // Class responsible for tokenizing the code
  class Tokenizer {
  public:
    Tokenizer(std::filesystem::path);

    std::vector<Token> get_token_list();
  private:
    std::vector<Token> token_list;
  };

}
