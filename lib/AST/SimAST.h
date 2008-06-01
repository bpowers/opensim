//===--- SimAST.h - Root AST for models ------------------------*- C++ -*-===//
//
// Copyright 2008 Bobby Powers
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
//===---------------------------------------------------------------------===//
//
// This class is the root node for an OpenSim AST
//
//===---------------------------------------------------------------------===//

#ifndef OSIM_SIMAST_H
#define OSIM_SIMAST_H

#include "../globals.h"
#include "General.h"
#include "../CodeGen/InterpreterModule.h"

namespace OpenSim 
{
  class EulerAST;
  class VariableAST;
  
  class SimAST : ExprAST
  {
    OpenSim::EulerAST *integrator;
    std::map<std::string, OpenSim::VariableAST *> namedVars;
    
  public:
    SimAST(OpenSim::EulerAST *intLoop, 
           std::map<std::string, OpenSim::VariableAST *> named);
    
    
    ~SimAST();
    
    
    double Codegen(ASTConsumer *owner);
    
    OpenSim::EulerAST *Integrator() {return integrator;}
    
    std::map<std::string, OpenSim::VariableAST *> NamedVars();
  };
}

#endif // OSIM_SIMAST_H
