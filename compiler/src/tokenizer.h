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
    SEMICOLON,
    BR_OPEN,
    BR_CLOSE,
    CURL_OPEN,
    CURL_CLOSE,
    // Keywords
    RETURN,
    // Literals
    INT_LIT, // Data: Value
    // Primitive types
    I32_T
  };

  const std::string token_type_names[] = {
    "identifier",
    "semicolon",
    "opening bracket",
    "closing bracket",
    "opening curly bracket",
    "closing curly bracket",
    "return",
    "int literal",
    "int"
  };

  // Struct representing a single Token
  struct Token {
    TokenType token_type;
    std::vector<std::string> data;

    // Factory functions for Token
    static Token from(std::string);
    static Token from(char);

    // Functions for category checking
    bool is_identifier();
    bool is_semicolon();
    bool is_br_open();
    bool is_br_close();
    bool is_curl_open();
    bool is_curl_close();
    bool is_return();
    bool is_literal();
    bool is_type();

    // Conversion functions
    std::string as_type();

    // Generation of string name for logging purposes
    std::string to_string();
  };

  // Class responsible for tokenizing the code
  class Tokenizer {
  public:
    Tokenizer(ParamData);

    std::vector<Token> get_token_list();
  private:
    std::vector<Token> token_list;
  };

}
