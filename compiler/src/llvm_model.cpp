#include "llvm_model.h"

std::string pl::LlvmModel::get_size_type() {
  return size_type;
}

pl::LMStringLit& pl::LlvmModel::register_string_lit(std::string s) {
  std::string d;
  size_t size = s.size();
  bool bs = false;
  for (char ch : s) {
    if (bs) {
      switch (ch) {
        case 'n':
          d += "\\0A";
          break;
        default:
          size++;
          d += "\\\\" + ch;
      }
      bs = false;
      continue;
    }
    if (ch == '\\') {
      size--;
      bs = true;
      continue;
    }
    d += ch;
  }
  string_lits.push_back({ d + "\\00", "@.str." + std::to_string(string_lits.size()), size + 1 });
  return string_lits.back();
}

void pl::LlvmModel::register_public_func(LMPublicFunc f) {
  public_funcs.push_back(f);
}

pl::LMPublicFunc& pl::LlvmModel::get_last_registered_public_func() {
  return public_funcs.back();
}

void pl::LlvmModel::register_public_func_def(LMPublicFuncDef f) {
  public_func_defs.push_back(f);
}

size_t pl::LlvmModel::register_attrs(LMAttrs a) {
  a.index = attrs.size();
  attrs.push_back(a);
  return attrs.size() - 1;
}

std::string pl::LlvmModel::obtain_function_type(std::string name) {
  for (LMPublicFuncDef& public_func_def : public_func_defs) {
    if (public_func_def.name == name) {
      return public_func_def.return_type;
    }
  }
  error("Unknown function.");
  return "";
}

std::vector<pl::LMPublicFuncDef::__params_t> pl::LlvmModel::obtain_function_param_types(std::string name) {
  for (LMPublicFuncDef& public_func_def : public_func_defs) {
    if (public_func_def.name == name) {
      return public_func_def.params;
    }
  }
  error("Unknown function.");
  return {};
}

std::string pl::LlvmModel::build_llvm() {
  std::string s;

  for (LMStringLit& string_lit : string_lits) {
    s += string_lit.id + " = private unnamed_addr constant [" + std::to_string(string_lit.size) + " x i8] c\"" + string_lit.data + "\", align 1\n";
  }

  s += "\n";

  for (LMPublicFunc& public_func : public_funcs) {
    s += "define dso_" + public_func.dso + " " + public_func.return_type + " @" + public_func.name + "() #" + std::to_string(register_attrs(LMAttrs{ public_func.attrs })) + " {\n";
    for (std::string& line : public_func.contents) {
      s += "\t" + line + "\n";
    }
    s += "}\n";
  }

  s += "\n";

  for (LMPublicFuncDef& public_func_def : public_func_defs) {
    s += "declare " + public_func_def.return_type + " @" + public_func_def.name + "(";
    for (size_t i = 0; i < public_func_def.params.size(); i++) {
      s += public_func_def.params.at(i).type;
      for (std::string& flag : public_func_def.params.at(i).flags) {
        s += " " + flag;
      }
      if (i < public_func_def.params.size() - 1) {
        s += ", ";
      }
    }
    s += ") #" + std::to_string(register_attrs(LMAttrs{ public_func_def.attrs })) + "\n";
  }

  s += "\n";

  for (LMAttrs& attr : attrs) {
    s += "attributes #" + std::to_string(attr.index) + " = { ";
    for (std::string& any_attr : attr.any_attrs) {
      s += any_attr + " ";
    }
    for (std::string& plat_attr : attr.plat_attrs) {
      s += plat_attr + " ";
    }
    s += "}\n";
  }

  return s;
}
