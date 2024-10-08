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
    } else if (token_list.at(0).is_type() && token_list.at(1).is_identifier()) {
      std::string n = token_list.at(1).data.at(0);
      std::string t = token_list.at(0).as_type();
      int a = token_list.at(0).as_alignment();
      token_list.erase(token_list.begin(), token_list.begin() + 2);

      PTELocalVarDecl child(this, n, t, a);
      token_list = child.parse(token_list);
      elements.push_back(std::make_shared<PTELocalVarDecl>(child));
    } else if (token_list.at(0).is_identifier() && token_list.at(1).is_eq()) {
      std::string n = token_list.at(0).data.at(0);
      token_list.erase(token_list.begin(), token_list.begin() + 2);

      PTEVarAssign child(this, n);
      token_list = child.parse(token_list);
      elements.push_back(std::make_shared<PTEVarAssign>(child));
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
  std::vector<std::vector<Token>> buf{ { } };
  std::vector<Token> operators;
  size_t bracket_index = 0;
  while (!token_list.at(0).is_semicolon()) {
    if (bracket_index == 0 && token_list.at(0).is_operator()) {
      buf.push_back({ });
      operators.push_back(token_list.at(0));
      token_list.erase(token_list.begin());
      continue;
    }
    if (token_list.at(0).is_br_open()) { bracket_index++; }
    if (token_list.at(0).is_br_close()) { bracket_index--; }
    buf.back().push_back(token_list.front());
    token_list.erase(token_list.begin());
  }
  return _evala(buf, operators, parent);
}

std::shared_ptr<pl::PTEVal> pl::PTEVal::_evala(std::vector<std::vector<Token>> token_list, std::vector<Token> operators, PTEBase* parent) {
  if (token_list.size() - 1 != operators.size()) {
    error("Compiler bug: (" + std::string{ __FILE__ } + ":" + std::to_string(__LINE__) + ")", token_list.at(0).at(0).line, token_list.at(0).at(0).character);
  }
  if (token_list.size() == 1) {
    return _evalb(token_list.at(0), parent);
  }
  Token lowest = operators.at(0);
  size_t lowest_index = 0;
  for (size_t i = 1; i < operators.size(); i++) {
    if (operators.at(i).as_operator_priority() < lowest.as_operator_priority()) {
      lowest = operators.at(i);
      lowest_index = i;
    }
  }
  PTEOpCalc child(parent, lowest);
  // So basically this line slices the token_list and the operators vectors at the lowest_index position and calls _evala individually again for both sides of the slice, from which a PTEOpCalc instance is generated.
  // This might have like 10 possible errors caused by overflow and stuff like that
  child.put_parsed(_evala(std::vector<std::vector<Token>>(token_list.begin(), token_list.begin() + static_cast<ptrdiff_t>(lowest_index) + 1), std::vector<Token>(operators.begin(), operators.begin() + static_cast<ptrdiff_t>(lowest_index)), parent), _evala(std::vector<std::vector<Token>>(token_list.begin() + static_cast<ptrdiff_t>(lowest_index) + 1, token_list.end()), std::vector<Token>(operators.begin() + static_cast<ptrdiff_t>(lowest_index) + 1, operators.end()), parent));
  return std::make_shared<PTEOpCalc>(child);
}

std::shared_ptr<pl::PTEVal> pl::PTEVal::_evalb(std::vector<Token>& token_list, PTEBase* parent) {
  if (token_list.size() == 1 && token_list.at(0).is_int_lit()) {
    PTEIntLit child(parent);
    child.parse(token_list);
    return std::make_shared<PTEIntLit>(child);
  } else if (token_list.size() == 1 && token_list.at(0).is_str_lit()) {
    PTEStrLit child(parent);
    child.parse(token_list);
    return std::make_shared<PTEStrLit>(child);
  } else if (token_list.size() == 1 && token_list.at(0).is_identifier()) {
    PTEVarVal child(parent, token_list.at(0).data.at(0));
    child.parse(token_list);
    return std::make_shared<PTEVarVal>(child);
  } else if (token_list.size() >= 3 && token_list.at(0).is_identifier() && token_list.at(1).is_br_open()) {
    PTEFuncCall child(parent, token_list.at(0).data.at(0));
    child.parse(token_list);
    return std::make_shared<PTEFuncCall>(child);
  } else {
    return nullptr;
  }
}

pl::PTEOpCalc::PTEOpCalc(PTEBase* p, Token o) : super(p), op(o) {}

std::string pl::PTEOpCalc::obtain_access(LlvmModel& model) {
  build_llvm(model);
  return "%" + std::to_string(model.get_last_registered_public_func().cssa - 1);
}

std::string pl::PTEOpCalc::obtain_preferred_type(LlvmModel& model) {
  return first -> obtain_preferred_type(model);
}

std::vector<pl::Token> pl::PTEOpCalc::parse(std::vector<Token> token_list) {
  return token_list;
}

void pl::PTEOpCalc::debug_tree(int level) {
  for (int i = 0; i < level; i++) {
    debug(" ");
  }
  debug("Calculation: " + op.to_string_no_data() + "\n");
  first -> debug_tree(level + 1);
  second -> debug_tree(level + 1);
}

void pl::PTEOpCalc::build_llvm(LlvmModel& model) {
  LMPublicFunc& func = model.get_last_registered_public_func();
  if (op.is_plus()) {
    func.contents.push_back("%" + std::to_string(func.cssa++) + " = add " + first -> obtain_preferred_type(model) + " " + first -> obtain_access(model) + ", " + second -> obtain_access(model));
    return;
  } else if (op.is_minus()) {
    func.contents.push_back("%" + std::to_string(func.cssa++) + " = sub " + first -> obtain_preferred_type(model) + " " + first -> obtain_access(model) + ", " + second -> obtain_access(model));
    return;
  } else if (op.is_asterisk()) {
    func.contents.push_back("%" + std::to_string(func.cssa++) + " = mul " + first -> obtain_preferred_type(model) + " " + first -> obtain_access(model) + ", " + second -> obtain_access(model));
    return;
  } else if (op.is_slash()) {
    error("Division isn't implemented yet because of signed and unsigned numbers.");
    return;
  } else {
    error("Compiler bug: (" + std::string{ __FILE__ } + ":" + std::to_string(__LINE__) + ")");
  }
}

void pl::PTEOpCalc::put_parsed(std::shared_ptr<PTEVal> f, std::shared_ptr<PTEVal> s) {
  first = f;
  second = s;
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

pl::PTEVarVal::PTEVarVal(PTEBase* p, std::string n) : super(p), name(n) {}

std::string pl::PTEVarVal::obtain_access(LlvmModel& model) {
  LMPublicFunc& func = model.get_last_registered_public_func();
  std::string type = obtain_preferred_type(model);
  func.contents.push_back("%" + std::to_string(func.cssa++) + " = load " + type + ", " + type + "* " + func.get_mem_addr_ptr(name));
  return "%" + std::to_string(func.cssa - 1);
}

std::string pl::PTEVarVal::obtain_preferred_type(LlvmModel& model) {
  return model.get_last_registered_public_func().get_variable_type(name);
}

std::vector<pl::Token> pl::PTEVarVal::parse(std::vector<Token> token_list) {
  return token_list;
}

void pl::PTEVarVal::debug_tree(int level) {
  for (int i = 0; i < level; i++) {
    debug(" ");
  }
  debug("Variable value: " + name + "\n");
}

void pl::PTEVarVal::build_llvm(LlvmModel& model) {}

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

pl::PTELocalVarDecl::PTELocalVarDecl(PTEBase* p, std::string n, std::string t, int a) : super(p), name(n), type(t), alignment(a) {}

std::vector<pl::Token> pl::PTELocalVarDecl::parse(std::vector<Token> token_list) {
  if (token_list.at(0).is_semicolon()) {
    return token_list;
  }
  if (!token_list.at(0).is_eq()) {
    error("Unexpected " + token_list.at(0).to_string_no_data() + " in variable declaration, expected = or ;", token_list.at(0).line, token_list.at(0).character);
  }
  token_list.erase(token_list.begin());
  value = PTEVal::eval(token_list, this);
  return token_list;
}

void pl::PTELocalVarDecl::debug_tree(int level) {
  for (int i = 0; i < level; i++) {
    debug(" ");
  }
  debug("Variable declaration: " + type + " " + name + "\n");
  if (value == std::nullopt) {
    return;
  }
  value.value() -> debug_tree(level + 1);
}

void pl::PTELocalVarDecl::build_llvm(LlvmModel& model) {
  LMPublicFunc& func = model.get_last_registered_public_func();
  std::string mem = func.obtain_stack_mem(type, alignment, name);
  if (value == std::nullopt) {
    return;
  }
  func.contents.push_back("store " + type + " " + value.value() -> obtain_access(model) + ", " + type + "* " + mem);
}

pl::PTEVarAssign::PTEVarAssign(PTEBase* p, std::string n) : super(p), name(n) {}

std::vector<pl::Token> pl::PTEVarAssign::parse(std::vector<Token> token_list) {
  value = PTEVal::eval(token_list, this);
  return token_list;
}

void pl::PTEVarAssign::debug_tree(int level) {
  for (int i = 0; i < level; i++) {
    debug(" ");
  }
  debug("Variable assignment: " + name + "\n");
  value -> debug_tree(level + 1);
}

void pl::PTEVarAssign::build_llvm(LlvmModel& model) {
  LMPublicFunc& func = model.get_last_registered_public_func();
  std::string t = func.get_variable_type(name);
  func.contents.push_back("store " + t + " " + value -> obtain_access(model) + ", " + t + "* " + func.get_mem_addr_ptr(name));
}

pl::Parser::Parser(Preprocessor preprocessor) : root(nullptr) {
  root.parse(preprocessor.get_token_list());
  debug("\n----------\n\n");
  root.debug_tree(0);
}

pl::PTERoot pl::Parser::get_root() {
  return root;
}
