//===--- parse.h - Objective OpenSim parser interface -------------------===//
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
// This file contains the interface to the objective opensim parser.
//
//===--------------------------------------------------------------------===//

#ifndef OPENSIM_PARSE_H
#define OPENSIM_PARSE_H

#include <string>

namespace llvm {
class MemoryBuffer;
}

namespace opensim {

class Scanner;
struct Token;

class Parser {
  std::string fileName;
  llvm::MemoryBuffer *fileBuffer;
  Scanner *scanner;
  Token *curTok;

public:
  Parser(std::string fileName);
  ~Parser();

  int parse();
};

}


#endif // OPENSIM_PARSE_H

