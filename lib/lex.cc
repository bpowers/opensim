//===--- lex.cc - lexer for OpenSim mark 2 ------------------------------===//
//
// Copyright 2009 Bobby Powers
//
// This file is part of OpenSim.
// 
// OpenSim is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// OpenSim is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with OpenSim.  If not, see <http://www.gnu.org/licenses/>.
//
//===--------------------------------------------------------------------===//
//
// This file contains data structures and functions for scanning equations
// and creating tokens from the input
//
//===--------------------------------------------------------------------===//


#include "opensim/lex.h"
#include "opensim/token.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#include <llvm/ADT/StringMap.h>

#include <cstdio>
#include <iostream>


using namespace opensim;
using std::cout;
using std::cerr;
using std::string;
using std::exception;
using llvm::StringMap;
using llvm::StringMapIterator;

// I imagine that it would be nice for some Europeans to have decimal
// represented as ',', but time will tell...
#define DECIMAL '.'


Scanner::Scanner(std::string fName,
                 const char *fStart,
                 const char *fEnd) : fileName(fName),
                                     fileStart(fStart),
                                     fileEnd(fEnd) {

  pos = fStart;
  peek = *pos;
  line = 1;

  reservedWords = new StringMap<Word *>(16);

  reserve(new Word("if", tag::If));
  reserve(new Word("integral", tag::Integral));
}


Scanner::~Scanner() {

  for (StringMapIterator<Word *> i = reservedWords->begin();
       i != reservedWords->end(); ++i)
    delete i->getValue();

  delete reservedWords;
}


inline bool Scanner::getChar() {

  if (pos < fileEnd)
    peek = *++pos;
  else
    peek = '\0';

  return !(peek == '\0');
}


inline bool Scanner::getChar(const char c) {

  getChar();
  return c == peek;
}


inline void Scanner::reserve(Word *tok) {

  (*reservedWords)[tok->iden] = tok;
}


inline Word *Scanner::getReserved(std::string lexeme) {

  return (*reservedWords)[lexeme];
}


Token *Scanner::getToken() {

  // skip any whitespace we might encounter, but keep track of our
  // line and where the curent line begins
  do {
    if (peek == '\n') {
      ++line;
      lineStart = pos;
    }
    // finally, break when we don't have anymore whitespace
    if (!isspace(peek)) break;
  } while (getChar());

  // if we've reached the end of the input, we should see a null character
  if (peek == '\0')
    return NULL;

  // keep track of the start of the token, relative to the start of
  // the current line
  uint32_t start = pos - lineStart;

  // XXX: additional two-char tokens like '==' should be matched here
  switch (peek) {
  case '=':
    if (getChar('='))
      return new Word("==", tag::Eq, fileName, line, start, start+2);
    else
      return new Token('=', fileName, line, start, start+1);
  }

  // match numbers first, which either begin with a digit or a decimal
  if (isdigit(peek) || peek == DECIMAL) {
    // we only want to match the first decimal, any subsequent one
    // is probably misplaced.
    bool have_decimal = false;
    const char *startPos = pos;
    // iterate through while making sure we stay within bounds
    while (getChar()) {
      if (peek == DECIMAL)
        if (!have_decimal)
          have_decimal = true;
        else
          break;
      else if (!isdigit(peek))
        break;
    }

    char *end;
    real_t num = strtod(startPos, &end);

    // make sure that strtod gets the same number we do.  perhaps we
    // only need to use strtod, we shall see.
    if (!(pos == end)) {
      fprintf(stderr, "opensim: ERROR: Problem reading number '%s' as '%f'. "
              "(pos:%d != end:%d)\n", string(startPos, pos-startPos).c_str(),
              num, pos-1-fileStart, end-fileStart);
      return NULL;
    }
    // finally, return our new number token.
    return new Number(string(startPos, pos-startPos), num, fileName,
                      line, start, pos-lineStart);
  }

  if (isalpha(peek) || peek == '_') {
    string s;
    do {
      s += peek;
      getChar();
    } while (isalnum(peek) || peek == '_' || peek == '.');

    // check if its a reserved word
    Word *w = getReserved(s);
    if (!w)
      w = new Word(s, tag::Id, fileName, line, start, pos-lineStart);

    return w;
  }

  Token *tok = new Token(peek, fileName, line, start, start+1);
  getChar();

  return tok;
}


