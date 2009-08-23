//===--- types.h - types for OpenSim type hierarchy ---------------------===//
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

#include <string>
#include <exception>

// forward declaration so that we don't have to #include a bunch of LLVM
// in our own headers, that should be done in the source files themselves.
namespace llvm {
  template<typename ValueT, typename AllocatorTy>
  class StringMap;
  struct MallocAllocator;
}


namespace opensim { namespace types {


class Object {
protected:
  llvm::StringMap<Object *, llvm::MallocAllocator> *members;
  std::string name;

public:
  Object();
  virtual ~Object();

  virtual bool isValueType() {
    throw "isValueType not implemented for base Object.";
  };
  virtual bool isVirtual() {
    throw "isVirtual() not implemented.";
  };
};

// end namespace opensim::types
} }

#endif // OPENSIM_TYPES_H

