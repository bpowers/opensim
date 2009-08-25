# Copyright (C) 2009 Bobby Powers
# Licensed under the terms of the GNU GPL v3 or later;
# see COPYING for details. Written by Bobby Powers <bobbypowers@gmail.com>

CC=gcc -std=c99
CXX=g++
CFLAGS=-O3 -Wall -Wno-unused-variable -pthread -I../../include -g -D_GNU_SOURCE
CXXFLAGS=$(CFLAGS) -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
RUNTIME=../../lib/c_runtime.c

TARGETS=rabbit infection types

all: $(TARGETS)

rabbit: rabbit.c $(RUNTIME)
	$(CC) $(CFLAGS) -o $@ $^

infection: infection.c $(RUNTIME)
	$(CC) $(CFLAGS) -o $@ $^

types: type_test.cc
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	$(RM) $(TARGETS)

