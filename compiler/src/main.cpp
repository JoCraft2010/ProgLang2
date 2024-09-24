#include "pch.h"

#include "llvm_builder.h"
#include "logger.h"
#include "param_handler.h"
#include "parser.h"
#include "preprocessor.h"
#include "tokenizer.h"

int main(int argc, char* argv[]) {
  pl::ParamData param_data = pl::ParamData::build(argc, argv);

  pl::Tokenizer tokenizer(param_data.in_path);

  pl::Preprocessor preprocessor(tokenizer, param_data.in_path.parent_path());

  pl::Parser parser(preprocessor);

  pl::LlvmBuilder llvm_builder(parser);

  pl::debug("\n----------\n\n");

  std::string llvm_ir = llvm_builder.build();

  pl::debug(llvm_ir);

  std::ofstream of(param_data.out_path);
  if (!of) {
    pl::error("Couldn't open output file.");
  }
  of << llvm_ir;
  of.close();

  return 0;
}
