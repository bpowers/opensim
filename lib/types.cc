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

//===--- Time -----------------------------------------------------------===//
Time::Time() {

}


Time::~Time() {

}


//===--- Namespace ------------------------------------------------------===//
Namespace::Namespace()
  : parent(NULL), children(), childrenList() {

}


Namespace::Namespace(Namespace *parent)
  : parent(parent), children(), childrenList() {

}


Namespace::Namespace(Namespace *parent, const StringRef name)
  : parent(parent), children(), childrenList() {

  // do this here because C++ doesn't like initializing
  // inherited member object in initialization list
  this->name = name.str();
}


Namespace::~Namespace() {

}


Object* Namespace::get(const StringRef name) {

  if (children.find(name) == children.end())
    return NULL;

  return children[name];
}


bool Namespace::add(Object *obj) {

  // if it already exists, fail
  if (children.find(obj->getName()) != children.end())
    return false;

  children[obj->getName()] = obj;
  childrenList.push_back(obj);
  return true;
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

