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
    error("Expected opening curly bracket in function declaration.\n");
  }
  token_list.erase(token_list.begin());
  while (!token_list.at(0).is_curl_close()) {
    if (token_list.at(0).is_return()) {
      token_list.erase(token_list.begin());

      PTEReturn child(this);
      token_list = child.parse(token_list);
      elements.push_back(std::make_shared<PTEReturn>(child));
    } else {
      std::shared_ptr<PTEVal> child = PTEVal::eval(token_list, this);
      if (child == nullptr) {
        error("Some evaluation failed inside of a function.\n");
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
    token_list.erase(token_list.begin());
  }
  token_list.erase(token_list.begin());
  if (!token_list.at(0).is_semicolon()) {
    error("Expected semicolon after function signature.\n");
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
  model.register_public_func_def(LMPublicFuncDef{ name, type, { } });
}

pl::PTEVal::PTEVal(PTEBase* p) : super(p) {}

std::shared_ptr<pl::PTEVal> pl::PTEVal::eval(std::vector<Token>& token_list, PTEBase* parent) {
  std::vector<Token> buf;
  while (!token_list.at(0).is_semicolon()) {
    buf.push_back(token_list.front());
    token_list.erase(token_list.begin());
  }
  if (buf.size() == 1 && buf.at(0).is_literal()) {
    PTEIntLit child(parent);
    child.parse(buf);
    return std::make_shared<PTEIntLit>(child);
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

std::vector<pl::Token> pl::PTEFuncCall::parse(std::vector<Token> token_list) {
  return token_list;
}

void pl::PTEFuncCall::debug_tree(int level) {
  for (int i = 0; i < level; i++) {
    debug(" ");
  }
  debug("Function call: " + name + "\n");
}

void pl::PTEFuncCall::build_llvm(LlvmModel& model) {
  LMPublicFunc& func = model.get_last_registered_public_func();
  func.contents.push_back("%" + std::to_string(func.cssa++) + " = call " + model.obtain_function_type(name) + " () @" + name + "()");
}

pl::PTEIntLit::PTEIntLit(PTEBase* p) : super(p) {}

std::string pl::PTEIntLit::obtain_access(LlvmModel& model) {
  return value;
}

std::vector<pl::Token> pl::PTEIntLit::parse(std::vector<Token> token_list) {
  if (token_list.size() != 1 || !token_list.at(0).is_literal()) {
    error("Too much or invalid data for an int literal. This is a compiler bug.");
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

pl::PTEReturn::PTEReturn(PTEBase* p) : super(p) {}

std::vector<pl::Token> pl::PTEReturn::parse(std::vector<Token> token_list) {
  std::shared_ptr<PTEVal> child = PTEVal::eval(token_list, this);
  if (child == nullptr) {
    error("Unexpected stuff in return statement.");
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
