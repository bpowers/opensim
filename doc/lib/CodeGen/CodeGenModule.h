//===--- CodeGenModule.h - LLVM IR creation from SimModules ----*- C++ -*-===//
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
// This class generates code from an OpenSim AST using the visitor pattern.
//
//===---------------------------------------------------------------------===//

#ifndef OSIM_CODEGENMODULE_H
#define OSIM_CODEGENMODULE_H

#include "../globals.h"
#include "../AST/ASTConsumer.h"

namespace OpenSim
{
  class VariableAST;
  
  class CodeGenModule : public ASTConsumer
  {
    std::map<std::string, OpenSim::VariableAST *> vars;
    std::map<std::string, llvm::AllocaInst*> NamedValues;
    
    /// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
    /// the function.  This is used for mutable variables etc.
    llvm::AllocaInst *CreateEntryBlockAlloca(const std::string &VarName);
    
    llvm::Module *OurModule;
    llvm::Function *OurFunction;
    
    llvm::LLVMFoldingBuilder Builder;
    
  public:
    CodeGenModule();
    ~CodeGenModule();
    
    void Consume(OpenSim::SimAST *start);
    
    virtual llvm::Value *visit(OpenSim::SimAST *node);
    virtual llvm::Value *visit(OpenSim::EulerAST *node);
    virtual llvm::Value *visit(OpenSim::VariableAST *node);
    virtual llvm::Value *visit(OpenSim::NumberExprAST *node);
    virtual llvm::Value *visit(OpenSim::BinaryExprAST *node);
    virtual llvm::Value *visit(OpenSim::VarRefAST *node);
    
    llvm::Module *GeneratedModule() {return OurModule;}
    llvm::Function *GeneratedFunction() {return OurFunction;}
  };
}

#endif // OSIM_CODEGENMODULE_H

