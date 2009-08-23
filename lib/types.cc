//===--- types.cc - types for OpenSim type hierarchy --------------------===//
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
// This file contains classes for forming the type hierarchy central to
// BOOSD.
//
//===--------------------------------------------------------------------===//

#include "opensim/types.h"

#include <iostream>

#include <llvm/ADT/StringMap.h>

using namespace opensim;
using namespace opensim::types;
using namespace llvm;
using namespace std;


Object::Object() {

  members = new StringMap<Object *>();
}


Object::~Object() {

}


