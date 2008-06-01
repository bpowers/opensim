//===--- VariableAST.h - Base AST class for sim variables ------*- C++ -*-===//
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
// This class represents the AST nodes of Variables.
//
//===---------------------------------------------------------------------===//

#ifndef OSIM_VARIABLEAST_H
#define OSIM_VARIABLEAST_H

// openSim stuff
#include "../globals.h"
#include "../Variable.h"
#include "General.h"
#include "ASTConsumer.h"
#include "../CodeGen/InterpreterModule.h"


namespace OpenSim
{
  class VariableAST : public ExprAST
  {
  protected:
    OpenSim::ExprAST *node;
    OpenSim::ExprAST *initialNode;
    OpenSim::Variable *var;
    
  public:
    VariableAST(OpenSim::Variable *varData, OpenSim::ExprAST *nodeData) 
      : var(varData), node(nodeData) {}
    ~VariableAST() {}
    
    void SetData(OpenSim::Variable *newData);
    OpenSim::Variable *Data();
    
    void SetInitial(OpenSim::ExprAST *newInitial) {initialNode = newInitial;}
    OpenSim::ExprAST *Initial() {return initialNode;}
    
    void SetAST(OpenSim::ExprAST *newAST) {node = newAST;}
    ExprAST *AST() {return node;}
    
    double Codegen(OpenSim::ASTConsumer *owner);
  };
}

#endif // OSIM_VARIABLEAST_H
