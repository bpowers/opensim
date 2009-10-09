//===--- types.h - types for OpenSim type hierarchy -----------*- C++ -*-===//
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

#ifndef OPENSIM_TYPES_H
#define OPENSIM_TYPES_H

#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/ilist.h>
#include <llvm/ADT/ilist_node.h>
using llvm::StringRef;

#include <string>
#include <exception>


/**
 * @mainpage BOOSD Oject hierarchy
 *
 * @section intro Introduction
 * BOOSD is an object-oriented System Dynamics implementation.
 */


namespace opensim {
namespace types {


/// Defines the types of integration methods OpenSim supports.
enum integrationKind {
  Euler,
  RK2,
  RK4
};


/// Base class for the BOOSD object hierarchy.
class Object: public llvm::ilist_node<Object> {
protected:
  std::string name;
  llvm::StringMap<Object *, llvm::MallocAllocator> *members;

  /// Basic Object Initialization.
  ///
  /// Initialization common to all BOOSD object types.
  void baseInit();

  /// Base Object Cleanup
  ///
  /// Cleanup/destruction common to all BOOSD object types
  virtual ~Object();
public:
  // no public constructor because you can't directly instantiate an
  // Object - it wouldn't make sense

  /// Returns the name of this object.
  const StringRef getName();

  /// True if the object can be used as an rval.
  ///
  /// Stocks, flows, auxiliary variables, tables, model functions and
  /// arrays are all value objects.  It doesn't make sense for objects
  /// such as Units to be used in equations.
  virtual bool isValueType();

  /// Determines whether a value type is virtual.
  ///
  /// Virtual types are value types which cannot be simulated or fully
  /// instantiated.  If any variables or submodels that are referenced
  /// (are members of the object in question) are themselves virtual,
  /// the object itself becomes virtual.
  virtual bool isVirtual();
};


/// Defines a namespace in which to store objects.
class Time: public Object {
public:
  Time();
  virtual ~Time();

  double startTime;
  double endTime;
  double timeStep;
  double saveStep;

  // XXX: I don't know if this is the right place for it, but I'm not
  //      sure of a better place.  Maybe 'Time' should be renamed?
  integrationKind integrationMethod;
};


// forward declaration, because Unit and Kind refer to each other.
class Unit;


/// Represents the units of a Value type.
class Kind: public Object {
protected:
  Unit *referenceUnit;

  /// This is for keeping track of all of the units, as well as aliases.
  ///
  /// Unit aliases are different strings which simply point to the same
  /// Unit object.
  llvm::StringMap<Unit *, llvm::MallocAllocator> *units;

public:
  Kind();
  virtual ~Kind();
};


/// Represents the units of a Value type.
class Unit: public Object {
protected:
  /// This units value, normalized against the reference unit of the Kind.
  ///
  /// So if this were 'feet' and the reference unit were 'meters', the
  /// value would be '.3048'.  This allows us to have constants specified
  /// in whatever units they were measured in, even if the units aren't
  /// consistent throughout the model.  While on one hand it might make the
  /// model seem more messy, it also reduces errors that could be caused by
  /// people converting things by hand, outside the model, which are VERY
  /// hard problems to find.
  double value;

  /// Reference to the kind of the unit we're defining.
  Kind *kind;

public:
  Unit();
  virtual ~Unit();
};


/// Defines a namespace in which to store objects.
class Namespace: public Object {
protected:
  Namespace *parent;

public:
  Namespace(Namespace *parent);
  Namespace(Namespace *parent, StringRef name);
  virtual ~Namespace();

  Object *getObject(StringRef qualifiedName);
};


/// Defines a model.
class Model: public Namespace {
protected:
  Time time;

  /// A list of the statements that define the model, in order of execution.
  llvm::ilist<Object> statements;

public:
  Model();
  virtual ~Model();

  virtual bool isValueType();
  virtual bool isVirtual();
};


/// Defines an object that can be used as a lval or rval
class Value: public Object {
protected:
  Kind *kind;
  Unit *units;

public:
  // no public constructor because you can't directly instantiate a
  // Value - it wouldn't make sense
  virtual ~Value();

  Kind *getKind();
  Unit *getUnits();
};


/// Defines a model function, such as smooth()
class ModelFunction : public Model, public Value {
protected:

  /// A pointer to the object whose value will be returned
  Object *returnObject;
public:
  ModelFunction();
  ~ModelFunction();

  virtual bool isValueType();
  // XXX: does it make any sense to have virtual model functions?
  virtual bool isVirtual();
};


/// Defines stock objects
class Stock: public Value {
protected:
  StringRef equation;

public:
  Stock();
  ~Stock();

  virtual StringRef getEquation();
  virtual void setEquation(StringRef newEquation);
};

// end namespace opensim::types
}
}

#endif // OPENSIM_TYPES_H

