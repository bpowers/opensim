# Copyright (C) 2009 Bobby Powers
# Licensed under the terms of the GNU GPL v3 or later;
# see COPYING for details. Written by Bobby Powers <bobbypowers@gmail.com>

CC=gcc -std=c99
CXX=g++
CFLAGS=-O3 -Wall -Wno-unused-variable -pthread -I../../include -g -D_GNU_SOURCE
CXXFLAGS=$(CFLAGS) -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
OPENSIM_SOURCE=$(`find ../../lib -name "*.h"`)
OPENSIM_HEADERS=$(`find ../../include -name "*.h"`)
OPENSIM_LIBDIR=../../lib/.libs
LLVM_LDFLAGS=`llvm-config --ldflags` `llvm-config --libs jit`



# if you invoke make as 'make V=1' it will verbosely list what it is doing,
# otherwise it defaults to pretty mode, which makes errors _much_ easier to see
ifneq ($V, 1)
MAKEFLAGS = -s
endif



TARGETS=types

all: $(TARGETS)

types: goats.cc $(OPENSIM_SOURCE) $(OPENSIM_HEADERS)
	@echo "  CXX   $@"
	$(CXX) $(CXXFLAGS) -o $@ $^ -lsim -L$(OPENSIM_LIBDIR) $(LLVM_LDFLAGS)

clean:
	$(RM) $(TARGETS)

