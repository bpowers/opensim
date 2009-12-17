# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR = /usr

SRC_DIR = ../..
# root of opensim build directory
BUILD_DIR = $(SRC_DIR)

LLVM_CXXFLAGS = $(shell llvm-config --cflags jit)
LLVM_LDFLAGS  = $(shell llvm-config --ldflags jit) \
                $(shell llvm-config --libs jit)

# Flags passed to the preprocessor.
CPPFLAGS += -I$(GTEST_DIR) -I$(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -I$(SRC_DIR)/include -I$(BUILD_DIR)/include \
            $(LLVM_CXXFLAGS)

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS := $(patsubst %.cc,%.o,$(wildcard *.cc))

BINARY = $(shell pwd | awk 'BEGIN {FS="/"}; {print tolower($$NF)}')-test

LIBSIM = $(BUILD_DIR)/lib/.libs/libsim.a

# Link against gtest if the test defines its own main function
GTEST_LIBS = -lgtest_main

.SUFFIXES:
.SUFFIXES: .o .cc

# build targets
all: $(BINARY)

clean:
	rm -f $(BINARY) *.o

$(LIBSIM):
	$(MAKE) -C $(BUILD_DIR)

$(BINARY): $(TESTS) $(LIBSIM)
	$(CXX) $^ $(GTEST_LIBS) $(LLVM_LDFLAGS) -o $@

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

.PHONY: libsim.a
