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


Scanner::Scanner(std::string fName,
                 const char *fStart,
                 const char *fEnd) : fileName(fName),
                                     fileStart(fStart),
                                     fileEnd(fEnd) {

  pos = fStart;
  peek = ' ';
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
    peek = *pos++;
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

  do {
    if (peek == '\n') {
      ++line;
      lineStart = pos;
    }

    if (!isspace(peek))
      break;
  } while (getChar());

  if (peek == '\0')
    return NULL;

  uint32_t start = pos - lineStart;

  // XXX: additional two-char tokens like '==' should be matched here
  switch (peek) {
  case '=':
    if (getChar('='))
      return new Word("==", tag::Eq, fileName, line, start, start+1);
    else
      return new Token('=', fileName, line, start, start);
  }

  if (isdigit(peek) || peek == '.') {
    bool have_decimal = false;
    const char *startPos = pos;
    // iterate through while making sure we stay within bounds
    while (getChar()) {
      if (peek == '.')
        if (!have_decimal)
          have_decimal = true;
        else
          break;
      else if (!isdigit(peek))
        break;
    }

    char *end;
    real_t num = strtod(startPos, &end);
    if (!(pos-1 == end)) {
      cerr << "opensim: ERROR: Problem reading number: "
           << string(startPos, pos-startPos) << "(tod: " << num << ") pos:"
           << (uint64_t)(pos-fileStart) << " != end:"
           << (uint64_t)(end-fileStart) << ".\n";
      return NULL;
    }
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

  Token *tok = new Token(peek, fileName, line, start, start);
  getChar();

  return tok;
}


