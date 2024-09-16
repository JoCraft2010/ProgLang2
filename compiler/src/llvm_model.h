#pragma once

#include "pch.h"

namespace pl {

  // LLVM Model public function container
  struct LMPublicFunc {
    std::string name; // Name of the function
    std::string dso; // Dynamic shared object type, can be local, preemptable, preemptible and weak
    std::string return_type; // Return type of the function (as a llvm primitive e.g. i32)
    std::vector<std::string> contents; // The lines of generated llvm ir
    int cssa; // The index of the current static single assignment i.e. register
  };

  // Class to model the resulting LLVM in a semi-high level way
  class LlvmModel {
  public:
    void register_public_func(LMPublicFunc);
    LMPublicFunc& get_last_registered_public_func();

    std::string build_llvm();
  private:
    std::vector<LMPublicFunc> public_funcs;
  };

}
