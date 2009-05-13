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

#include <llvm/ADT/StringMap.h>

#include <cstdio>
#include <iostream>


using namespace opensim;
using std::cout;
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

  reservedWords = new StringMap<Token *>(16);

  reserve(new Word("if", tag::If));
}


Scanner::~Scanner() {

  for (StringMapIterator<Token *> i = reservedWords->begin();
       i != reservedWords->end(); ++i)
    delete i->getValue();

  delete reservedWords;
}


inline bool Scanner::getChar() {

  if (pos < fileEnd)
    peek = *pos++;
  else
    peek = '\0';
}


inline void Scanner::reserve(Token *tok) {

  (*reservedWords)[tok->iden] = tok;
}


inline Token *Scanner::getReserved(std::string lexeme) {

  return (*reservedWords)[lexeme];
}


Token *Scanner::getToken() {

  do {
    if (peek == ' ' || peek == '\t' || peek == '\r')
      continue;
    else if (peek == '\n')
      ++line;
    else if (peek == '\0')
      return NULL;
    else
      break;
  } while (getChar());

  cout << "getToken stub\n";
  return NULL;
}


