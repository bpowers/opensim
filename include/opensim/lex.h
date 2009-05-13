//===--- lex.h - lexer for OpenSim mark 2 -------------------------------===//
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
// This file contains classes and functions to lex an input file.
//
//===--------------------------------------------------------------------===//

#ifndef OPENSIM_LEX_H
#define OPENSIM_LEX_H

#include <inttypes.h>
#include <cstdio>
#include <string>


namespace llvm {
  template<typename ValueT, typename AllocatorTy>
  class StringMap;
  struct MallocAllocator;
}


namespace opensim {

class Token;


class Scanner {
  std::string fileName;
  const char *fileStart;
  const char *fileEnd;

  const char *pos;
  uint32_t peek;
  uint32_t line;

  llvm::StringMap<Token *, llvm::MallocAllocator> *reservedWords;

  bool getChar();

public:
  Scanner(std::string fName, const char *fStart, const char *fEnd);
  ~Scanner();

  void reserve(Token *tok);
  Token *getReserved(std::string lexeme);

  Token *getToken();
};

}


#endif // OPENSIM_LEX_H

