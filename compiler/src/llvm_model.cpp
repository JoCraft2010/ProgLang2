#include "llvm_model.h"

void pl::LlvmModel::register_public_func(LMPublicFunc f) {
  public_funcs.push_back(f);
}

pl::LMPublicFunc& pl::LlvmModel::get_last_registered_public_func() {
  return public_funcs.back();
}

size_t pl::LlvmModel::register_attrs(LMAttrs a) {
  a.index = attrs.size();
  attrs.push_back(a);
  return attrs.size() - 1;
}

std::string pl::LlvmModel::build_llvm() {
  std::string s;

  for (LMPublicFunc& public_func : public_funcs) {
    s += "define dso_" + public_func.dso + " " + public_func.return_type + " @" + public_func.name + "() #" + std::to_string(register_attrs(LMAttrs{ public_func.attrs })) + " {\n";
    for (std::string& line : public_func.contents) {
      s += "\t" + line + "\n";
    }
    s += "}\n";
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
