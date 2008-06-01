//===--- PrettyPrintModule.h - Pretty AST printer --------------*- C++ -*-===//
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
// This class generates some pretty printing to the screen (for now) of the
// AST.  for now its primarily an AST walker test before I implement 
// the code generator.
//
//===---------------------------------------------------------------------===//

#ifndef OSIM_PYTHONPRINTMODULE_H
#define OSIM_PYTHONPRINTMODULE_H

#include "../globals.h"
#include "../AST/ASTConsumer.h"

namespace OpenSim
{
  class VariableAST;
  class SimAST;
  class EulerAST;
  class BinaryExprAST;
  class NumberExprAST;
  
  class PythonPrintModule : public ASTConsumer
  {
    std::string whitespace;
    std::map<std::string, OpenSim::VariableAST *> vars;

    FILE *simout;
    
  public:
    PythonPrintModule();
    ~PythonPrintModule();
    
    void Consume(OpenSim::SimAST *start, FILE *output_file);
    
    virtual double visit(OpenSim::SimAST *node);
    virtual double visit(OpenSim::EulerAST *node);
    virtual double visit(OpenSim::VariableAST *node);
    virtual double visit(OpenSim::NumberExprAST *node);
    virtual double visit(OpenSim::BinaryExprAST *node);
    virtual double visit(OpenSim::UnaryExprAST *node);
    virtual double visit(OpenSim::VarRefAST *node);
    virtual double visit(OpenSim::LookupAST *node);
    virtual double visit(OpenSim::LookupRefAST *node);
  };
}

#endif // OSIM_PYTHONPRINTMODULE_H
