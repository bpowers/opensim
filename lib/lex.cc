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


static inline bool isNumberStart(uint32_t character) {
  return isdigit(character) || character == DECIMAL;
}

static inline bool isNumberBody(uint32_t character) {
  return isdigit(character) || character == DECIMAL;
}


static inline bool isIdentifierStart(uint32_t character) {
  return isalpha(character) || character == '_';
}

static inline bool isIdentifierBody(uint32_t character) {
  return isalnum(character) || character == '_';
}


Scanner::Scanner(std::string fName,
                 const char *fStart,
                 const char *fEnd) : fileName(fName),
                                     fileStart(fStart),
                                     fileEnd(fEnd) {

  pos = fStart;
  peek = *pos;
  line = 1;

  reservedWords = new StringMap<uint32_t>(16);

  reserve("if", Tag::If);
  reserve("integral", Tag::Integral);
  reserve("Model", Tag::Model);
}


Scanner::~Scanner() {

  delete reservedWords;
}


inline bool Scanner::getChar() {

  if (pos < fileEnd)
    peek = *++pos;

  return peek;
}


inline bool Scanner::getChar(const char c) {

  getChar();
  return c == (char)peek;
}


inline void Scanner::reserve(std::string lexeme, uint32_t t) {

  (*reservedWords)[lexeme] = t;
}


inline uint32_t Scanner::getReserved(std::string lexeme) {

  return (*reservedWords)[lexeme];
}


inline void Scanner::skipWhitespace() {

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
}


Token *Scanner::getToken() {

  skipWhitespace();

  // if we've reached the end of the input, peek is null
  if (!peek)
    return NULL;

  // keep track of the start of the token, relative to the start of
  // the current line
  uint32_t start = pos - lineStart;
  SourceLoc startLoc = SourceLoc(line, start);

  // XXX: additional two-char tokens like '==' should be matched here
  switch (peek) {
  case '=':
    if (getChar('='))
      return new Word("==", Tag::Eq, fileName,
                      startLoc, SourceLoc(line, start+2));
    else
      return new Token('=', fileName,
                       startLoc, SourceLoc(line, start+2));
  }

  // match numbers first, which either begin with a digit or a decimal
  if (isNumberStart(peek)) {
    return LexNumber(startLoc);
  }

  if (isIdentifierStart(peek)) {
    return LexIdentifier(startLoc);
  }

  Token *tok = new Token(peek, fileName,
                         SourceLoc(line, start),
                         SourceLoc(line, start+1));
  getChar();

  return tok;
}


inline Token *Scanner::LexIdentifier(SourceLoc startLoc) {

  string s;
  do {
    s += peek;
    getChar();
  } while (isalnum(peek) || peek == '_' || peek == '.');

  // check if its a reserved word
  uint32_t t = getReserved(s);
  if (!t)
    t = Tag::Id;

  return new Word(s, t, fileName, startLoc, SourceLoc(startLoc.line,
                                                      pos-lineStart));
}


inline Token *Scanner::LexNumber(SourceLoc startLoc) {

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
                    startLoc, SourceLoc(startLoc.line, pos-lineStart));
}

