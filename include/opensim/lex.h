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
#include <string>

#include <llvm/ADT/StringMap.h>


namespace opensim {

struct Token;
struct SourceLoc;

class Scanner {
  std::string _fileName;
  const char *_fileStart;
  const char *_fileEnd;

  const char *_pos;
  uint32_t _peek;
  uint32_t _line;
  const char *_lineStart;

  llvm::StringMap<uint32_t, llvm::MallocAllocator> *_reservedWords;

  bool getChar();
  bool getChar(const char c);

  uint32_t getReserved(std::string lexeme);
  void skipWhitespace();

  Token *lexIdentifier(SourceLoc startLoc);
  Token *lexNumber(SourceLoc startLoc);

public:
  Scanner(std::string fileName, const char *fileStart, const char *fileEnd);
  ~Scanner();

  void reserve(std::string lexeme, uint32_t t);
  Token *getToken();
};

}


#endif // OPENSIM_LEX_H

