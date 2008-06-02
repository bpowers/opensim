//===--- ASTConsumer.h - Virtual AST consumer to walk AST ------*- C++ -*-===//
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
// This is the virtual class for others that wish to walk the AST and 
// produce output.
//
//===---------------------------------------------------------------------===//

#ifndef OSIM_ASTCONSUMER_H
#define OSIM_ASTCONSUMER_H

#include "../globals.h"


namespace OpenSim
{
  class VariableAST;
  class SimAST;
  class EulerAST;
  class BinaryExprAST;
  class NumberExprAST;
  class UnaryExprAST;
  class VarRefAST;
  class LookupAST;
  class LookupRefAST;
  class FunctionRefAST;
  
  class ASTConsumer
  {
  public:
    ASTConsumer() {}
    virtual ~ASTConsumer() {}
    
    virtual void Consume(OpenSim::SimAST *start, FILE *output_file) = 0;
    
    virtual double visit(OpenSim::SimAST *node) = 0;
    virtual double visit(OpenSim::EulerAST *node) = 0;
    virtual double visit(OpenSim::VariableAST *node) = 0;
    virtual double visit(OpenSim::NumberExprAST *node) = 0;
    virtual double visit(OpenSim::BinaryExprAST *node) = 0;
    virtual double visit(OpenSim::UnaryExprAST *node) = 0;
    virtual double visit(OpenSim::VarRefAST *node) = 0;
    virtual double visit(OpenSim::LookupAST *node) = 0;
    virtual double visit(OpenSim::LookupRefAST *node) = 0;
    virtual double visit(OpenSim::FunctionRefAST *node) = 0;
  };
}

#endif // OSIM_ASTCONSUMER_H
