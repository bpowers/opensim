SRC_DIR = ../..
# root of opensim build directory
BUILD_DIR = $(SRC_DIR)

LLVM_CXXFLAGS = $(shell llvm-config --cflags jit)
LLVM_LDFLAGS  = $(shell llvm-config --ldflags jit) \
                $(shell llvm-config --libs jit)

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -I$(SRC_DIR)/include -I$(BUILD_DIR)/include \
            $(LLVM_CXXFLAGS)

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS := $(patsubst %.cc,%.o,$(wildcard *.cc))

# gives us, for example, lex-test, depending on the directory we're in
BINARY := $(shell pwd | awk 'BEGIN {FS="/"}; {print tolower($$NF)}')-test

# work around libtool
LIBSIM = $(BUILD_DIR)/lib/.libs/libsim.a

GTEST_LIBS = -lgtest_main -lgtest

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
