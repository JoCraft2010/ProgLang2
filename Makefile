CXX := g++
CC := gcc
LLC := llc

CXXFLAGS ?= -Wextra -Wshadow -Wconversion -Wsign-conversion -Wunreachable-code -pedantic -fomit-frame-pointer -fstack-protector-strong -fno-strict-aliasing -fno-exceptions -fstack-clash-protection -fno-plt -D_FORTIFY_SOURCE=2 -O3 -std=c++17 -pipe -march=native
CCFLAGS ?=
LLCFLAGS ?= -relocation-model=dynamic-no-pic

COMPILER_SRC_DIR := compiler/src
COMPILER_SRC := $(wildcard $(COMPILER_SRC_DIR)/*.cpp)
COMPILER_OBJ_DIR := compiler/obj
COMPILER_OBJ := $(patsubst $(COMPILER_SRC_DIR)/%.cpp,$(COMPILER_OBJ_DIR)/%.o,$(COMPILER_SRC))
COMPILER_MMD := $(patsubst $(COMPILER_SRC_DIR)/%.cpp,$(COMPILER_OBJ_DIR)/%.d,$(COMPILER_SRC))
COMPILER_BIN_DIR := compiler/bin
COMPILER_BIN := $(COMPILER_BIN_DIR)/compiler

LANGUAGE_SRC_DIR := language/src
LANGUAGE_SRC := $(LANGUAGE_SRC_DIR)/debug/main.pl2
LANGUAGE_TMP_DIR := language/tmp
LANGUAGE_LL := $(LANGUAGE_TMP_DIR)/main.ll
LANGUAGE_ASM := $(LANGUAGE_TMP_DIR)/main.s
LANGUAGE_BIN_DIR := language/bin
LANGUAGE_BIN := $(LANGUAGE_BIN_DIR)/main

MKDIR_P := mkdir -p
RM_RF := rm -rf

Q = @

all: fix_missing compiler

fix_missing:
	$(Q) $(MKDIR_P) $(COMPILER_OBJ_DIR) $(COMPILER_BIN_DIR) $(LANGUAGE_TMP_DIR) $(LANGUAGE_BIN_DIR)

$(COMPILER_OBJ_DIR)/%.o: $(COMPILER_SRC_DIR)/%.cpp | $(COMPILER_OBJ_DIR)
	$(Q) $(CXX) $(CXXFLAGS) -c -MMD $< -o $@

compiler: $(COMPILER_OBJ)
	$(Q) $(CXX) $(CXXFLAGS) $(COMPILER_OBJ) -o $(COMPILER_BIN)
	$(Q) $(COMPILER_BIN) -i $(LANGUAGE_SRC) -o $(LANGUAGE_LL)
	$(Q) $(LLC) $(LLCFLAGS) $(LANGUAGE_LL)
	$(Q) $(CC) $(CCFLAGS) $(LANGUAGE_ASM) -o $(LANGUAGE_BIN)
	$(Q) $(LANGUAGE_BIN)

clean:
	$(Q) $(RM_RF) $(COMPILER_OBJ) $(COMPILER_MMD) $(LANGUAGE_LL) $(LANGUAGE_ASM)

.PHONY: all fix_missing compiler clean
