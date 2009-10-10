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
#include "opensim/runtime.h"

#include <llvm/Support/raw_ostream.h>
using namespace llvm;

using namespace opensim;
using namespace opensim::types;


//===--- Object ---------------------------------------------------------===//
void Object::baseInit() {

  members = new llvm::StringMap<Object *>();
}


Object::~Object() {

  delete members;
}


const llvm::StringRef Object::getName() {

  return name;
}


bool Object::isValueType() {

  throw "isValueType() not implemented for base Object.";
}


bool Object::isVirtual() {

  throw "isVirtual() only valid for Model and Value objects.";
}


//===--- Namespace ------------------------------------------------------===//
Namespace::Namespace() {

  baseInit();

  outs() << "namespace init\n";
}


Namespace::Namespace(Namespace *parent) {

  baseInit();

  this->parent = parent;
}


Namespace::Namespace(Namespace *parent, StringRef name) {

  baseInit();

  this->parent = parent;
  this->name = name.str();
}


Namespace::~Namespace() {

}


//===--- Model ----------------------------------------------------------===//
Model::Model(StringRef name) {

  baseInit();

  this->name = name;
}


//===--- Value ----------------------------------------------------------===//
Kind *Value::getKind() {

  return kind;
}


Unit *Value::getUnits() {

  return units;
}


//===--- Stock ----------------------------------------------------------===//

