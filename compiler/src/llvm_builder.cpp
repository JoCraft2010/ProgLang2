#include "llvm_builder.h"

pl::LlvmBuilder::LlvmBuilder(pl::Parser& p) : parser(p) {}

std::string pl::LlvmBuilder::build() {
  LlvmModel model;

  parser.get_root().build_llvm(model);

  return model.build_llvm();
}
