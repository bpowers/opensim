//===--- run.cc - run manager for OpenSim mark 2 ------------------------===//
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
// This file contains classes and functions to build and rebuild ASTs,
// register passes and manage AST walks.
//
//===--------------------------------------------------------------------===//

#include "run.h"
#include "opensim_runtime.h"
#include "parse.h"
#include <iostream>
#include <fstream>
#include <exception>


using namespace opensim;
using namespace std;


Runtime::Runtime() {

}


Runtime::~Runtime() {

}


int Runtime::parse(std::string fileName)
{
  modelFileName = fileName;

  try
  {
    Parser parser(fileName);
    parser.parse();
  }
  catch (exception& e)
  {
    cout << "opensim: error: Problem opening '" << fileName << "'\n";
  }
}


int
Runtime::simulate() {

  cout << "simulate stub\n";
}

