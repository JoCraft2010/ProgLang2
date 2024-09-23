#include "parser.h"

pl::PTEBase::PTEBase(PTEBase* p) : parent(p) {}

pl::PTERoot::PTERoot(PTEBase* p) : super(p) {}

std::vector<pl::Token> pl::PTERoot::parse(std::vector<Token> token_list) {
  std::vector<Token> buf;
  while (token_list.size() > 0) {
    buf.push_back(token_list.front());
    token_list.erase(token_list.begin());

    if (buf.size() == 3 && buf.at(0).is_type() && buf.at(1).is_identifier() && buf.at(2).is_br_open()) {
      PTEFunc child(this, buf.at(1).data.at(0), buf.at(0).as_type());
      token_list = child.parse(token_list);
      elements.push_back(std::make_shared<PTEFunc>(child));
      buf.clear();
    } else if (buf.size() == 4 && buf.at(0).is_at() && buf.at(1).is_type() && buf.at(2).is_identifier() && buf.at(3).is_br_open()) {
      PTEFuncDef child(this, buf.at(2).data.at(0), buf.at(1).as_type());
      token_list = child.parse(token_list);
      elements.push_back(std::make_shared<PTEFuncDef>(child));
      buf.clear();
    }
  }

  return {};
}

void pl::PTERoot::debug_tree(int level) {
  for (int i = 0; i < level; i++) {
    debug(" ");
  }
  debug("Root\n");
  for (std::shared_ptr<PTEBase>& e : elements) {
    e -> debug_tree(level + 1);
  }
}

void pl::PTERoot::build_llvm(LlvmModel& model) {
  for (std::shared_ptr<PTEBase> e : elements) {
    e -> build_llvm(model);
  }
}

pl::PTEExec::PTEExec(PTEBase* p) : super(p) {}

pl::PTEFunc::PTEFunc(PTEBase* p, std::string n, std::string t) : super(p), name(n), type(t) {}

std::vector<pl::Token> pl::PTEFunc::parse(std::vector<Token> token_list) {
  while (!token_list.at(0).is_br_close()) {
    token_list.erase(token_list.begin());
  }
  token_list.erase(token_list.begin());
  if (!token_list.at(0).is_curl_open()) {
    error("Unexpected " + token_list.at(0).to_string_no_data() + " after function declaration, expected {", token_list.at(0).line, token_list.at(0).character);
  }
  token_list.erase(token_list.begin());
  while (!token_list.at(0).is_curl_close()) {
    if (token_list.at(0).is_return()) {
      token_list.erase(token_list.begin());

      PTEReturn child(this);
      token_list = child.parse(token_list);
      elements.push_back(std::make_shared<PTEReturn>(child));
    } else {
      Token first = token_list.at(0);
      std::shared_ptr<PTEVal> child = PTEVal::eval(token_list, this);
      if (child == nullptr) {
        error("Unexpected " + first.to_string_no_data(), first.line, first.character);
      }
      elements.push_back(child);
    }
    token_list.erase(token_list.begin());
  }
  return token_list;
}

void pl::PTEFunc::debug_tree(int level) {
  for (int i = 0; i < level; i++) {
    debug(" ");
  }
  debug("Function: " + name + " with type " + type + "\n");
  for (std::shared_ptr<PTEBase>& e : elements) {
    e -> debug_tree(level + 1);
  }
}

void pl::PTEFunc::build_llvm(LlvmModel& model) {
  model.register_public_func(LMPublicFunc{ name, "local", type, { "%1 = alloca i32, align 4", "store i32 0, ptr %1, align 4" }, 2, { "noinline", "nounwind", "optnone", "sspstrong", "uwtable" } });
  for (std::shared_ptr<PTEBase> e : elements) {
    e -> build_llvm(model);
  }
}

pl::PTEFuncDef::PTEFuncDef(PTEBase* p, std::string n, std::string t) : super(p), name(n), type(t) {}

std::vector<pl::Token> pl::PTEFuncDef::parse(std::vector<Token> token_list) {
  while (!token_list.at(0).is_br_close()) {
    if ((!token_list.at(0).is_type()) && (!token_list.at(0).is_tilde())) {
      error("Unexpected " + token_list.at(0).to_string_no_data() + " in function declaration, expected type", token_list.at(0).line, token_list.at(0).character);
    }
    if (token_list.at(1).is_at()) {
      params.push_back(token_list.at(0).as_type() + "*");
      token_list.erase(token_list.begin(), token_list.begin() + 2);
    } else {
      params.push_back(token_list.at(0).as_type());
      token_list.erase(token_list.begin());
    }
    if (token_list.at(0).is_comma()) {
      token_list.erase(token_list.begin());
    } else if (!token_list.at(0).is_br_close()) {
      error("Unexpected " + token_list.at(0).to_string_no_data() + " in function declaration, expected comma or )", token_list.at(0).line, token_list.at(0).character);
    }
  }
  token_list.erase(token_list.begin());
  if (!token_list.at(0).is_semicolon()) {
    error("Expected semicolon after function signature, got " + token_list.at(0).to_string_no_data() + " instead", token_list.at(0).line, token_list.at(0).character);
  }
  token_list.erase(token_list.begin());
  return token_list;
}

void pl::PTEFuncDef::debug_tree(int level) {
  for (int i = 0; i < level; i++) {
    debug(" ");
  }
  debug("Function signature: " + name + " with type " + type + "\n");
}

void pl::PTEFuncDef::build_llvm(LlvmModel& model) {
  std::vector<LMPublicFuncDef::__params_t> p;
  for (std::string& param : params) {
    if (param.back() == '*' || param == "...") {
      p.push_back({ param, { } });
    } else {
      p.push_back({ param, { "noundef" } });
    }
  }
  model.register_public_func_def(LMPublicFuncDef{ name, type, { }, p });
}

pl::PTEVal::PTEVal(PTEBase* p) : super(p) {}

std::shared_ptr<pl::PTEVal> pl::PTEVal::eval(std::vector<Token>& token_list, PTEBase* parent) {
  std::vector<Token> buf;
  while (!token_list.at(0).is_semicolon()) {
    buf.push_back(token_list.front());
    token_list.erase(token_list.begin());
  }
  if (buf.size() == 1 && buf.at(0).is_int_lit()) {
    PTEIntLit child(parent);
    child.parse(buf);
    return std::make_shared<PTEIntLit>(child);
  } else if (buf.size() == 1 && buf.at(0).is_str_lit()) {
    PTEStrLit child(parent);
    child.parse(buf);
    return std::make_shared<PTEStrLit>(child);
  } else if (buf.at(0).is_identifier() && buf.at(1).is_br_open()) {
    PTEFuncCall child(parent, buf.at(0).data.at(0));
    child.parse(buf);
    return std::make_shared<PTEFuncCall>(child);
  } else {
    return nullptr;
  }
}

pl::PTEFuncCall::PTEFuncCall(PTEBase* p, std::string n) : super(p), name(n) {}

std::string pl::PTEFuncCall::obtain_access(LlvmModel& model) {
  build_llvm(model);
  return "%" + std::to_string(model.get_last_registered_public_func().cssa - 1);
}

std::string pl::PTEFuncCall::obtain_preferred_type(LlvmModel& model) {
  return model.obtain_function_type(name, line, character);
}

std::vector<pl::Token> pl::PTEFuncCall::parse(std::vector<Token> token_list) {
  line = token_list.at(0).line;
  character = token_list.at(0).character;
  if (token_list.size() == 3) {
    return { };
  }
  token_list.erase(token_list.begin(), token_list.begin() + 2);
  std::vector<Token> buf;
  int bracket_index = 0;
  while (token_list.size() > 0) {
    if (token_list.front().is_br_open()) { bracket_index++; }
    else if (token_list.front().is_br_close()) { bracket_index--; }
    if (token_list.front().is_comma() || bracket_index < 0) {
      buf.push_back({ SEMICOLON, { }, 0, 0 });
      Token first = buf.at(0);
      std::shared_ptr<PTEVal> child = PTEVal::eval(buf, this);
      if (child == nullptr) {
        error("Unexpected " + first.to_string_no_data() + " in function parameter", first.line, first.character);
      }
      elements.push_back(child);
      buf.clear();
    }
    if (!token_list.front().is_comma()) {
      buf.push_back(token_list.front());
    }
    token_list.erase(token_list.begin());
  }
  return token_list;
}

void pl::PTEFuncCall::debug_tree(int level) {
  for (int i = 0; i < level; i++) {
    debug(" ");
  }
  debug("Function call: " + name + "\n");
  for (std::shared_ptr<PTEVal>& e : elements) {
    e -> debug_tree(level + 1);
  }
}

void pl::PTEFuncCall::build_llvm(LlvmModel& model) {
  LMPublicFunc& func = model.get_last_registered_public_func();
  std::string s = " = call " + model.obtain_function_type(name, line, character) + " (";
  std::vector<LMPublicFuncDef::__params_t> param_types = model.obtain_function_param_types(name, line, character);
  for (size_t i = 0; i < param_types.size(); i++) {
    s += param_types.at(i).type;
    if (i < param_types.size() - 1) {
      s += ", ";
    }
  }
  s += ") @" + name + "(";
  for (size_t i = 0; i < elements.size(); i++) {
    if (i >= param_types.size() || param_types.at(i).type == "...") {
      s += elements.at(i) -> obtain_preferred_type(model);
    } else {
      s += param_types.at(i).type;
      for (std::string& flag : param_types.at(i).flags) {
        s += " " + flag;
      }
    }
    s += " ";
    s += elements.at(i) -> obtain_access(model);
    if (i < elements.size() - 1) {
      s += ", ";
    }
  }
  s += ")";
  s = "%" + std::to_string(func.cssa++) + s;
  func.contents.push_back(s);
}

pl::PTEIntLit::PTEIntLit(PTEBase* p) : super(p) {}

std::string pl::PTEIntLit::obtain_access(LlvmModel& model) {
  return value;
}

std::string pl::PTEIntLit::obtain_preferred_type(LlvmModel& model) {
  return "i32";
}

std::vector<pl::Token> pl::PTEIntLit::parse(std::vector<Token> token_list) {
  if (token_list.size() != 1 || !token_list.at(0).is_literal()) {
    error("Compiler bug: (" + std::string{ __FILE__ } + ":" + std::to_string(__LINE__) + ")", token_list.at(0).line, token_list.at(0).character);
  }
  value = token_list.at(0).data.at(0);
  return token_list;
}

void pl::PTEIntLit::debug_tree(int level) {
  for (int i = 0; i < level; i++) {
    debug(" ");
  }
  debug("Int literal: " + value + "\n");
}

void pl::PTEIntLit::build_llvm(LlvmModel& model) {}

pl::PTEStrLit::PTEStrLit(PTEBase* p) : super(p) {}

std::string pl::PTEStrLit::obtain_access(LlvmModel& model) {
  LMStringLit lit = model.register_string_lit(value);
  LMPublicFunc& func = model.get_last_registered_public_func();
  func.contents.push_back("%" + std::to_string(func.cssa++) + " = getelementptr inbounds [" + std::to_string(lit.size) + " x i8], [" + std::to_string(lit.size) + " x i8]* " + lit.id + ", " + model.get_size_type() + " 0, " + model.get_size_type() + " 0");
  return "%" + std::to_string(func.cssa - 1);
}

std::string pl::PTEStrLit::obtain_preferred_type(LlvmModel& model) {
  return "i8*";
}

std::vector<pl::Token> pl::PTEStrLit::parse(std::vector<Token> token_list) {
  if (token_list.size() != 1 || !token_list.at(0).is_literal()) {
    error("Compiler bug: (" + std::string{ __FILE__ } + ":" + std::to_string(__LINE__) + ")", token_list.at(0).line, token_list.at(0).character);
  }
  value = token_list.at(0).data.at(0);
  return token_list;
}

void pl::PTEStrLit::debug_tree(int level) {
  for (int i = 0; i < level; i++) {
    debug(" ");
  }
  debug("String literal: " + value + "\n");
}

void pl::PTEStrLit::build_llvm(LlvmModel& model) {}

pl::PTEReturn::PTEReturn(PTEBase* p) : super(p) {}

std::vector<pl::Token> pl::PTEReturn::parse(std::vector<Token> token_list) {
  Token first = token_list.at(0);
  std::shared_ptr<PTEVal> child = PTEVal::eval(token_list, this);
  if (child == nullptr) {
    error("Unexpected " + first.to_string_no_data() + " in return statement", first.line, first.character);
  }
  value = child;
  return token_list;
}

void pl::PTEReturn::debug_tree(int level) {
  for (int i = 0; i < level; i++) {
    debug(" ");
  }
  debug("Return\n");
  value -> debug_tree(level + 1);
}

void pl::PTEReturn::build_llvm(LlvmModel& model) {
  LMPublicFunc& func = model.get_last_registered_public_func();
  func.contents.push_back("ret " + func.return_type + " " + value -> obtain_access(model));
}

pl::Parser::Parser(Preprocessor preprocessor) : root(nullptr) {
  root.parse(preprocessor.get_token_list());
  root.debug_tree(0);
}

pl::PTERoot pl::Parser::get_root() {
  return root;
}
