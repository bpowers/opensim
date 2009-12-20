//===--- parse.cc - parser for OpenSim mark 2 ---------------------------===//
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
// This file contains functions for parsing equations and generating ASTs.
//
//===--------------------------------------------------------------------===//


#include "opensim/parse.h"
#include "opensim/runtime.h"
#include "opensim/lex.h"
#include "opensim/token.h"

#include <llvm/Support/MemoryBuffer.h>

#include <iostream>
#include <cstdio>
#include <exception>


using namespace opensim;
using namespace std;


Parser::Parser(std::string fName) {

  fileName = fName;
  fileBuffer = llvm::MemoryBuffer::getFile(fName.c_str());
  if (!fileBuffer) {
    errs() << "opensim: ERROR: Problem mapping '" << fileName << "'\n";
    throw exception();
  }

  errs() << "opensim: DEBUG: Parser mapped '" << fileName << "' at 0x";
  errs().write_hex((unsigned long)fileBuffer->getBufferStart());
  errs() << " (len:" << fileBuffer->getBufferSize() << ").\n";

  scanner = new Scanner(fName, fileBuffer->getBufferStart(),
                        fileBuffer->getBufferEnd());
}


Parser::~Parser() {

  if (scanner)
    delete scanner;

  if (fileBuffer)
    delete fileBuffer;
}


int Parser::parse()
{
  fprintf(stderr, "opensim: DEBUG: Parsing...\n");
  while ((curTok = scanner->getToken()))
  {
    curTok->dump();
    delete curTok;
  }

  return 0;
}

