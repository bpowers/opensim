#!/usr/bin/env make -rf
# Copyright (C) 2010 Bobby Powers
# Licensed under the terms of the GNU GPL v3 or later;
# see COPYING for details. Written by Bobby Powers <bobbypowers@gmail.com>

CC=clang
CXX=clang++
CFLAGS=-O3 -Wall -Wno-unused-variable -pthread -I../../include -g -D_GNU_SOURCE -I.
CXXFLAGS=$(CFLAGS) -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
OPENSIM_SOURCE=$(`find ../../lib -name "*.h"`)
OPENSIM_HEADERS=$(`find ../../include -name "*.h"`)
OPENSIM_LIBDIR=../../lib/.libs
LLVM_LDFLAGS=`llvm-config --ldflags` `llvm-config --libs jit`

SRC=src

# if you invoke make as 'make V=1' it will verbosely list what it is doing,
# otherwise it defaults to pretty mode, which makes errors _much_ easier to see
ifneq ($V, 1)
MAKEFLAGS = -s
endif


# clear out all suffixes
.SUFFIXES:
# list only those we use
.SUFFIXES: .c .o .cc

TARGETS=boosd

all: $(TARGETS)

.c.o:
	@echo "  CC  $@"
	$(CC) $(CFLAGS) $< -o $@

.cc.o:
	@echo "  CXX $@"
	$(CXX) $(CXXFLAGS) $< -o $@

boosd_lexer.c: boosd.g
	@echo "  ANTLR $^"
	antlr3 $^ && \
	mv boosdLexer.c boosd_lexer.c && \
	mv boosdParser.c boosd_parser.c && \
	mv boosdLexer.h boosd_lexer.h && \
	mv boosdParser.h boosd_parser.h && \
	perl -pi -e "s/boosdLexer/boosd_lexer/g" boosd_lexer.c && \
	perl -pi -e "s/boosdParser/boosd_parser/g" boosd_parser.c

boosd_parser.c: boosd.g
	@echo "  ANTLR $^"
	antlr3 $^ && \
	mv boosdLexer.c boosd_lexer.c && \
	mv boosdParser.c boosd_parser.c

grammar: boosd_lexer.c boosd_parser.c

boosd: grammar

types: main.c boosd_lexer.o boosd_parser.o
	@echo "  CXX   $@"
	$(CXX) $(CXXFLAGS) -o $@ $^ -lsim -L$(OPENSIM_LIBDIR) $(LLVM_LDFLAGS)

clean:
	$(RM) $(TARGETS)
	rm boosd*.c
	rm boosd*.h

