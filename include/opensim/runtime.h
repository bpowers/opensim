//===--- run.h - run manager for OpenSim mark 2 interface ---------------===//
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

#ifndef OPENSIM_RUN_H
#define OPENSIM_RUN_H

#include <string>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Casting.h>
using llvm::isa;
using llvm::dyn_cast;
using llvm::cast;
using llvm::outs;
using llvm::errs;


namespace opensim {

class Parser;


class Runtime {
  std::string modelFileName;

  friend class Parser;

public:
  Runtime();
  ~Runtime();

  int loadFile(std::string fileName);
  int simulate();
};

// put these in a nested namespace so they don't conflict with the
// c-api functions of the same name when we're using namespace opensim.
namespace startup {

  void init();
  void exit();

} // end namespace startup
} // end namespace opensim

#endif // OPENSIM_RUN_H

