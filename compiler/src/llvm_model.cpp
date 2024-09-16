#include "llvm_model.h"

void pl::LlvmModel::register_public_func(LMPublicFunc f) {
  public_funcs.push_back(f);
}

pl::LMPublicFunc& pl::LlvmModel::get_last_registered_public_func() {
  return public_funcs.back();
}

std::string pl::LlvmModel::build_llvm() {
  std::string s;

  for (LMPublicFunc& public_func : public_funcs) {
    s += "define dso_" + public_func.dso + " " + public_func.return_type + " @" + public_func.name + "() {\n";
    for (std::string& line : public_func.contents) {
      s += "\t" + line + "\n";
    }
    s += "}\n";
  }

  return s;
}
