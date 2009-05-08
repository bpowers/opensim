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


#include "lex.h"
#include <cstdio>
#include <iostream>
#include <exception>


using namespace opensim;
using std::cout;
using std::exception;


Scanner::Scanner(std::string fName) {

  fileName = fName;
  file = fopen(fName.c_str(), "r");
  if (!file)
  {
    cout << "opensim: error: Couldn't find or open '" << fName << "'.\n";
    throw exception();
  }

  cout << "scanner stub\n";
}


Scanner::~Scanner() {

  if (file)
    fclose(file);
}


Token *Scanner::getToken()
{
  cout << "getToken stub\n";
  return NULL;
}


