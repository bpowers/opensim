# Copyright (C) 2009 Bobby Powers
# Licensed under the terms of the GNU GPL v3 or later;
# see COPYING for details. Written by Bobby Powers <bobbypowers@gmail.com>

CC=gcc -std=c99
CFLAGS=-O3 -Wall -Wno-unused-variable -pthread -I../../include -g
RUNTIME=../../lib/c_runtime.c

all: rabbit infection

rabbit: rabbit.c $(RUNTIME)
	$(CC) $(CFLAGS) -o $@ $^

infection: infection.c $(RUNTIME)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) infection rabbit

