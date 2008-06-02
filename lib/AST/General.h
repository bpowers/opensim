//===--- General.h - many general AST nodes --------------------*- C++ -*-===//
//
// Copyright 2008 Bobby Powers, large portions of General.h/cpp are 
//   Copyright Chris Lattner (LLVM Kaleidescope Tutorial)
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
// This file contains interfaces for many general AST nodes that are needed
//
//===---------------------------------------------------------------------===//

#ifndef OSIM_GENERAL_H
#define OSIM_GENERAL_H

#include "../globals.h"
#include "../CodeGen/InterpreterModule.h"
using std::vector;

namespace OpenSim
{
  class ASTConsumer;
  
  /// ExprAST - Base class for all expression nodes.
  class ExprAST {
  public:
    virtual ~ExprAST() {}
    
    virtual double Codegen(ASTConsumer *owner) = 0;
  };
  
  
  
  /// NumberExprAST - Expression class for numeric literals like "1.0".
  class NumberExprAST : public ExprAST {
    double val;
  public:
    double Val() {return val;}
    NumberExprAST(double value) : val(value) {}
    
    double Codegen(ASTConsumer *owner);
  };
  
  
  
  /// BinaryExprAST - Expression class for a binary operator.
  class BinaryExprAST : public ExprAST {
  public:
    char Op;
    ExprAST *LHS, *RHS;
    BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs) 
    : Op(op), LHS(lhs), RHS(rhs) {}
    
    double Codegen(ASTConsumer *owner);
  };
  
  
  
  /// UnaryExprAST - Expression class for a unary operator.
  class UnaryExprAST : public ExprAST {
  public:
    char Op;
    ExprAST *RHS;
    UnaryExprAST(char op, ExprAST *rhs) : Op(op), RHS(rhs) {}
    
    double Codegen(ASTConsumer *owner);
  };
  
  
  
  /// VarExprAST - Expression class for variable reference
  class VarRefAST : public ExprAST {
    std::string name;
  public:
    VarRefAST(const std::string refVar) : name(refVar) {}
    
    double Codegen(ASTConsumer *owner);
    std::string Name() {return name;}
  };
  
  
  
  /// VarExprAST - Expression class for var/in
  class LookupRefAST : public ExprAST {
    std::string name;
  public:
    ExprAST *ref;
    LookupRefAST(std::string tableVar, ExprAST *refVar) 
      : name(tableVar), ref(refVar) {}
    
    double Codegen(ASTConsumer *owner);
    std::string TableName() {return name;}
  };
}
#endif // OSIM_GENERAL_H
