#include "param_handler.h"

pl::ParamData pl::ParamData::build(int argc, char* argv[]) {
  ParamData param_data;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0) {
      i++;
      if (i < argc) {
        param_data.in_path = argv[i];
      }
    } else if (strcmp(argv[i], "-o") == 0) {
      i++;
      if (i < argc) {
        param_data.out_path = argv[i];
      }
    }
  }

  if (!std::filesystem::exists(param_data.in_path)) {
    error("Input file does not exist.\n");
  }
  if (param_data.out_path.empty()) {
    error("Output file not specified.\n");
  }

  return param_data;
}
