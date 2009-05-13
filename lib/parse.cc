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

#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <stdio.h>
#include <stdbool.h>

#include <cstdio>
#include <iostream>
#include <exception>


using namespace opensim;
using namespace std;


Parser::Parser(std::string fName) {

  fileName = fName;

  int len;
  FILE *model_fp;

  // first we open the file read-only to make sure its there and to find
  // it's length
  model_fp = fopen(fName.c_str(), "r");
  if (!model_fp) {
    cerr << "opensim: error: Couldn't find or open '" << fName << "'.\n";
    throw exception();
  }

  // now seek to the end of the file
  uint32_t err;
  err = fseek(model_fp, 0, SEEK_END);
  if (err) {
    fclose(model_fp);
    cerr << "opensim: error: Problem seeking on input file.\n";
    throw exception();
  }

  // so we can get its length.
  len = ftell(model_fp);

  // we have to close this file, becuase mmap needs a file descriptor and
  // getting that from a FILE * seems to be undefined.
  fclose(model_fp);
  model_fp = NULL;

  // so now reopen it.
  file = open(fileName.c_str(), O_RDONLY);

  // and map it, read only, into memory.
  fileStart = (char *)mmap(NULL,
                           len,
                           PROT_READ,
                           MAP_SHARED,
                           file,
                           0);
  // now we have starting and ending char *, which we can use
  // like iterators.
  fileEnd = fileStart + len;

  cerr << "opensim: DEBUG:  Parser mapped '" << fileName << "' at "
       << &fileStart << " (len: " << len << ").\n";

  scanner = new Scanner(fName, fileStart, fileEnd);
}


Parser::~Parser() {

  if (scanner)
    delete scanner;

  if (fileStart)
    munmap(fileStart, fileEnd-fileStart);

  if (file)
    close(file);
}


int Parser::parse()
{
  cout << "opensim: DEBUG: Parsing...\n";
  while (curTok = scanner->getToken())
  {
    curTok->dump();
  }
}

