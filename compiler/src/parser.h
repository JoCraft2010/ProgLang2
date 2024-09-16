#pragma once

#include "pch.h"

#include "llvm_model.h"
#include "preprocessor.h"

namespace pl {

  // Base class for all Parse Tree Elements
  class PTEBase {
  public:
    PTEBase(PTEBase*);
    virtual std::vector<Token> parse(std::vector<Token>) = 0;
    virtual void debug_tree(int) = 0;
    virtual void build_llvm(LlvmModel&) = 0;
  protected:
    PTEBase* parent;
  };

  // Root class hinting to non-contained code at root level
  class PTERoot : public PTEBase {
  public:
    PTERoot(PTEBase*);
    std::vector<Token> parse(std::vector<Token>) override;
    void debug_tree(int) override;
    void build_llvm(LlvmModel&) override;
  private:
    using super = PTEBase;

    std::vector<std::shared_ptr<PTEBase>> elements;
  };

  // Base class for executable bodies like functions
  class PTEExec : public PTEBase {
  public:
    PTEExec(PTEBase*);
  private:
    using super = PTEBase;
  };

  // Class for normal functions
  class PTEFunc : public PTEExec {
  public:
    PTEFunc(PTEBase*, std::string, std::string);
    std::vector<Token> parse(std::vector<Token>) override;
    void debug_tree(int) override;
    void build_llvm(LlvmModel&) override;
  private:
    using super = PTEExec;

    std::string name;
    std::string type;
    std::vector<std::shared_ptr<PTEBase>> elements;
  };

  // Base class for values
  class PTEVal : public PTEBase {
  public:
    PTEVal(PTEBase*);
    virtual std::string obtain_access(LlvmModel&) = 0;
  private:
    using super = PTEBase;
  };

  // Class for int literals
  class PTEIntLit : public PTEVal {
  public:
    PTEIntLit(PTEBase*);
    std::string obtain_access(LlvmModel&) override;
    std::vector<Token> parse(std::vector<Token>) override;
    void debug_tree(int) override;
    void build_llvm(LlvmModel&) override;
  private:
    using super = PTEVal;

    std::string value;
  };

  // Class for return statements
  class PTEReturn : public PTEBase {
  public:
    PTEReturn(PTEBase*);
    std::vector<Token> parse(std::vector<Token>) override;
    void debug_tree(int) override;
    void build_llvm(LlvmModel&) override;
  private:
    using super = PTEBase;

    std::shared_ptr<PTEVal> value;
  };

  // Class for parsing the preprocessed tokens
  class Parser {
  public:
    Parser(Preprocessor);

    PTERoot get_root();
  private:
    PTERoot root;
  };

}