#pragma once

#include "pch.h"

#include "logger.h"

namespace pl {

  // LLVM Model string literal container
  struct LMStringLit {
    std::string data; // The data the string literal holds
    std::string id; // The ID the string literal is assigned
    size_t size; // The size (length) the string literal is
  };

  // LLVM Model public function container
  struct LMPublicFunc {
    // struct representing a single entry in the stack ssa pool for local variables
    struct __stack_ssa_pool_entry {
      std::string type; // The type of the entry as a llvm primitive
      std::string name; // The name of the stack entry (e.g. %42)
      int alignment; // The alignment of the piece of stack memory
    };

    std::string name; // Name of the function
    std::string dso; // Dynamic shared object type, can be local, preemptable, preemptible and weak
    std::string return_type; // Return type of the function (as a llvm primitive e.g. i32)
    std::vector<std::string> contents; // The lines of generated llvm ir
    int cssa; // The index of the current static single assignment i.e. register
    std::vector<std::string> attrs; // Attributes declared for this method
    std::vector<__stack_ssa_pool_entry> stack_pool = { }; // Stack pool containing all stack-assigned variables
    std::unordered_map<std::string, size_t> stack_pool_assignments = {}; // Map ordering the assignment of variable name (freeing should be handled by scope PTE element) to stack pool entry

    std::string obtain_stack_mem(std::string, int, std::string);
    std::string get_mem_addr_ptr(std::string);
    std::string get_variable_type(std::string);
  };

  // LLVM Model public function signature container
  struct LMPublicFuncDef {
    // Struct representing a single parameter
    struct __params_t {
      std::string type; // Type of the parameter as a llvm primitive
      std::vector<std::string> flags; // Flags like noundef
    };

    std::string name; // Name of the function
    std::string return_type; // Return type of the function (as a llvm primitive e.g. i32)
    std::vector<std::string> attrs; // Attributes declared for this method
    std::vector<__params_t> params; // Parameters the function takes
  };

  // LLVM Model attributes container (LLVM IR: attributes #69 = { ... })
  struct LMAttrs {
    std::vector<std::string> any_attrs; // The normal arguments (e.g. nocallback, willreturn, etc.)
    std::vector<std::string> plat_attrs = { "\"frame-pointer\"=\"all\"", "\"min-legal-vector-width\"=\"0\"", "\"no-trapping-math\"=\"true\"", "\"stack-protector-buffer-size\"=\"8\"", "\"tune-cpu\"=\"generic\"" }; // The paltform-specific attributes
    size_t index = 0; // The index of the attrs. Will be generated automatically once registered.
  };

  // Class to model the resulting LLVM in a semi-high level way
  class LlvmModel {
  public:
    std::string get_size_type();

    LMStringLit& register_string_lit(std::string);

    void register_public_func(LMPublicFunc);
    LMPublicFunc& get_last_registered_public_func();

    void register_public_func_def(LMPublicFuncDef);

    size_t register_attrs(LMAttrs);

    std::string obtain_function_type(std::string, size_t, size_t);
    std::vector<LMPublicFuncDef::__params_t> obtain_function_param_types(std::string, size_t, size_t);

    std::string build_llvm();
  private:
    const std::string size_type = "i64";

    std::vector<LMStringLit> string_lits;
    std::vector<LMPublicFunc> public_funcs;
    std::vector<LMPublicFuncDef> public_func_defs;
    std::vector<LMAttrs> attrs;
  };

}
