//===--- EulerAST.h - class for Euler integration --------------*- C++ -*-===//
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
// This class represents Euler integration when used as a node in an 
// abstract syntax tree.
// TODO: implement superclass and a RK2 integration class.
//
//===---------------------------------------------------------------------===//

#ifndef OSIM_EULERAST_H
#define OSIM_EULERAST_H

#include "../globals.h"
#include "VariableAST.h"
#include "../CodeGen/InterpreterModule.h"

namespace OpenSim
{
  class EulerAST : public ExprAST
  {
    std::string name;
    std::string equation;
    varType type;
    
    std::vector<OpenSim::VariableAST *> body;
    
  public:
    EulerAST(std::vector<OpenSim::VariableAST *> newBody);
    ~EulerAST();
    
    double Codegen(ASTConsumer *owner);

    std::vector<OpenSim::VariableAST *> Body();
  };
}
#endif // OSIM_EULERAST_H
