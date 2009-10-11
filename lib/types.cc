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

using namespace opensim;
using namespace opensim::types;


//===--- Object ---------------------------------------------------------===//
Object::Object() {

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


bool Object::classof(const Object *obj) {

  // XXX: implement
  return false;
}


//===--- Time -----------------------------------------------------------===//
Time::Time() {

}


Time::~Time() {

}


//===--- Namespace ------------------------------------------------------===//
Namespace::Namespace() {

}


Namespace::Namespace(Namespace *parent)
  : parent(parent) {

}


Namespace::Namespace(Namespace *parent, const StringRef name)
  : parent(parent) {

  this->name = name.str();
}


Namespace::~Namespace() {

}


Object* Namespace::get(const StringRef name) {

  return NULL;
}


bool Namespace::add(Object *) {

  return false;
}


//===--- Model ----------------------------------------------------------===//
Model::Model(StringRef name) {

  this->name = name;
}


Model::~Model() {

}


//===--- Value ----------------------------------------------------------===//
Kind *Value::getKind() {

  return kind;
}


Unit *Value::getUnits() {

  return units;
}


//===--- Stock ----------------------------------------------------------===//

