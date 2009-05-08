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

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <stdio.h>
#include <stdbool.h>

#include <cstdio>
#include <iostream>
#include <exception>


using namespace opensim;
using std::cout;
using std::exception;


Scanner::Scanner(std::string fName) {

  FILE *model_fp;
  fileName = fName;
  model_fp = fopen(fName.c_str(), "r");
  if (!model_fp)
  {
    cout << "opensim: error: Couldn't find or open '" << fName << "'.\n";
    throw exception();
  }

  uint32_t err;
  err = fseek(model_fp, 0, SEEK_END);
  if (err)
  {
    cout << "opensim: error: problem seeking 1\n";
    goto clean_seek;
  }

  len = ftell(model_fp);
  err = fseek(model_fp, 0, SEEK_SET);
  if (err)
    goto clean_seek;
  fclose(model_fp);
  model_fp = NULL;

  file = open(fileName.c_str(), O_RDONLY);

  mappedFile = (char *)mmap(NULL,
                            len,
                            PROT_READ,
                            MAP_SHARED,
                            file,
                            0);

  cout << "scanner stub\n";
  return;

clean_seek:

  if (model_fp)
    fclose(model_fp);
  cout << "opensim: error: problem seeking on input file\n";
  throw exception();
}


Scanner::~Scanner() {

  if (mappedFile)
    munmap(mappedFile, len);

  if (file)
    close(file);
}


Token *Scanner::getToken()
{
  cout << "getToken stub\n";
  return NULL;
}


