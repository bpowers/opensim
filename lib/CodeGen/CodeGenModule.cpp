//===--- CodeGenModule.cpp - LLVM IR creation from SimModules ------------===//
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

#include "CodeGenModule.h"
#include "../AST/SimAST.h"
#include "../AST/EulerAST.h"
#include "../AST/VariableAST.h"

#include <stdarg.h>

using std::string;
using std::vector;
using std::map;

using llvm::Value;
using llvm::Function;
using llvm::FunctionType;
using llvm::Type;
using llvm::AllocaInst;
using llvm::BasicBlock;
using llvm::ConstantFP;
using llvm::APFloat;


extern "C" void SimPrint(double v1, ...)
{
  fprintf(stdout, "%f\n", v1);
}

OpenSim::CodeGenModule::CodeGenModule()
{
  OurModule = new llvm::Module("openSim CL");
    
  FunctionType *voidVoid = FunctionType::get(Type::VoidTy, 
                                             std::vector<const Type*>(), 
                                             false);
    
  OurFunction = new Function(voidVoid, Function::ExternalLinkage, 
                             "simulate", OurModule);
  
  
  // Add extern reference for our SimPrint function;
  
  std::vector<const Type*> Doubles(1, Type::DoubleTy);
  
  FunctionType *FT = FunctionType::get(Type::DoubleTy, Doubles, true);
  
  Function *F = new Function(FT, Function::ExternalLinkage, "SimPrint", OurModule);
}



OpenSim::CodeGenModule::~CodeGenModule() {}



void 
OpenSim::CodeGenModule::Consume(OpenSim::SimAST *start)
{
    start->Codegen(this);
}



/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
llvm::AllocaInst *
OpenSim::CodeGenModule::CreateEntryBlockAlloca(const std::string &VarName) 
{
  llvm::LLVMBuilder TmpB(&OurFunction->getEntryBlock(),
                         OurFunction->getEntryBlock().begin());

  return TmpB.CreateAlloca(Type::DoubleTy, 0, VarName.c_str());
}



llvm::Value *
OpenSim::CodeGenModule::visit(OpenSim::SimAST *node)
{
  // get all of the named variables in the model
  vars = node->NamedVars();
  
  
  // create entry block (necessary for LLVM)
  BasicBlock *BB = new BasicBlock("entry", OurFunction);
  Builder.SetInsertPoint(BB);
  
  
  int numRows = 0;
  for (map<string, VariableAST *>::iterator itr = vars.begin(); 
       itr != vars.end(); itr++) 
  {
    VariableAST *v_ast = itr->second;
    Variable *v = v_ast->Data();
    
    
    if (v->Name() == "") 
    {
      fprintf(stdout, "Warning: there is a variable without a name.\n");
      continue;
    }
    
    
    // reserve space in memory for a double, and save the reference
    // to this variables space in the NamedValues map
    AllocaInst *Alloca = CreateEntryBlockAlloca(v->Name());
    NamedValues[v->Name()] = Alloca;
    
    // when we start mallocing a big block of memory for the variables,
    // this will become useful.
    if ((v->Type() == var_stock) || (v->Type() == var_const))
    {
      //++numRows;
      
      Value *initialValue = NULL;
      
      // get the value of constants
      if (v->Type() == var_const)
      {
        initialValue = v_ast->AST()->Codegen(this);
      }
      
      // get initial value of stocks
      if (v->Type() == var_stock)
      {
        // reserve space in memory for a double, and save the reference
        // to this variables space in the NamedValues map
        AllocaInst *Alloca = CreateEntryBlockAlloca(v->Name() + "_next");
        NamedValues[v->Name() + "_next"] = Alloca;
        
        initialValue = v_ast->Initial()->Codegen(this);
      }
      
      if (initialValue)
        Builder.CreateStore(initialValue, Alloca);
    }
  }
  
  // now generate the rest of the function
  node->Integrator()->Codegen(this);
  
  Builder.CreateRetVoid();
  
  return 0;
}



llvm::Value *
OpenSim::CodeGenModule::visit(OpenSim::EulerAST *node)
{
  AllocaInst *TimeAlloca = CreateEntryBlockAlloca("time");
  NamedValues["time"] = TimeAlloca;
  
  {
    Value *StartVar = Builder.CreateLoad(NamedValues["OS_start"], "OS_start");
    Builder.CreateStore(StartVar, TimeAlloca);
  }
  
  // Make the new basic block for the loop header, inserting after current
  // block.
  BasicBlock *PreheaderBB = Builder.GetInsertBlock();
  BasicBlock *ForCondBB = new BasicBlock("forcond", OurFunction);
  BasicBlock *ForBodyBB = new BasicBlock("forbody", OurFunction);
  BasicBlock *UpdateStockBB = new BasicBlock("updatestocks", OurFunction);
  BasicBlock *ForIncBB = new BasicBlock("forinc", OurFunction);
  BasicBlock *ForAfterBB = new BasicBlock("forafter", OurFunction);
  
  // Insert an explicit fall through from the current block to the LoopBB.
  Builder.CreateBr(ForCondBB);
  
  // Start insertion in the begining of the for loop.
  Builder.SetInsertPoint(ForCondBB);
  
  {
    Value *TimeVar = Builder.CreateLoad(NamedValues["time"], "time");
    //Value *TimestepVar = Builder.CreateLoad(NamedValues["OS_timestep"], "OS_timestep");
    Value *EndVar = Builder.CreateLoad(NamedValues["OS_end"], "OS_end");
    
    // Convert condition to a bool by comparing equal to 0.0.
    Value *EndCond = Builder.CreateFCmpOLT(TimeVar, EndVar, "forcond");
    
    // Insert the conditional branch into the end of LoopEndBB.
    Builder.CreateCondBr(EndCond, ForBodyBB, ForAfterBB);
  }
  
  Builder.SetInsertPoint(ForBodyBB);
  
  
  std::vector<Value*> ArgsV;
  
  // heres the meat and potatoes where we create all the variable and
  // stock assignments 
  vector<VariableAST *> body = node->Body();
  for (vector<VariableAST *>::iterator itr = body.begin();
       itr != body.end(); ++itr)
  {
    ArgsV.push_back((*itr)->Codegen(this));
  }
  
  
  Function *PrintF = OurModule->getFunction("SimPrint");
  if (PrintF == 0)
  {
    fprintf(stdout, "SimPrint unknown function referenced");
    return 0;
  }
  
  // add time for the first column
  ArgsV.push_back(Builder.CreateLoad(NamedValues["time"], "time"));
  
  // bah doesn't work again...
  //Builder.CreateCall(PrintF, ArgsV.rbegin(), ArgsV.rend(), "printout");
  
  
  // Insert an explicit fall through from the current block to the LoopBB.
  Builder.CreateBr(UpdateStockBB);
  Builder.SetInsertPoint(UpdateStockBB);
  
  
  for (map<string, VariableAST *>::iterator itr = vars.begin(); 
       itr != vars.end(); itr++) 
  {   
    Variable *v = itr->second->Data();
    // define constants at the top of the file
    if (v->Type() == var_stock)
    {
      string nextName = v->Name() + "_next";
      AllocaInst *inst = NamedValues[nextName];
      Value *newVal = Builder.CreateLoad(inst, nextName.c_str());
      
      Builder.CreateStore(newVal, NamedValues[v->Name()]);
    }
  }
  
  // Insert an explicit fall through from the current block to the LoopBB.
  Builder.CreateBr(ForIncBB);
  
  // Start insertion in the begining of the for loop.
  Builder.SetInsertPoint(ForIncBB);
  
  {
    Value *TimeVar = Builder.CreateLoad(NamedValues["time"], "time");
    Value *TimestepVar = Builder.CreateLoad(NamedValues["OS_timestep"], "OS_timestep");
    Value *NewTime = Builder.CreateAdd(TimeVar, TimestepVar, "new_time");
    
    Builder.CreateStore(NewTime, TimeAlloca);
  }
  
  // Insert an explicit fall through from the current block to the LoopBB.
  Builder.CreateBr(ForCondBB);
  
  // Start insertion in the begining of the for loop.
  Builder.SetInsertPoint(ForAfterBB);
  
  return 0;
}



llvm::Value *
OpenSim::CodeGenModule::visit(OpenSim::VariableAST *node)
{
  string storeName = node->Data()->Name();
  if (node->Data()->Type() == var_stock)
    storeName += "_next";
  
  Value *newValue = NULL;
  AllocaInst *newAlloca = NamedValues[storeName];
  
  newValue = node->AST()->Codegen(this);
  
  if (newValue == NULL)
  {
    fprintf(stdout, "Warning: unable to process variable '%s'",
            storeName.c_str());
    
    return 0;
  }
  
  return Builder.CreateStore(newValue, newAlloca);
}



llvm::Value *
OpenSim::CodeGenModule::visit(OpenSim::VarRefAST *node)
{
  string varName = node->Name();
  
  // Look this variable up in the function.
  Value *V = NamedValues[varName];
  if (V == 0) 
  {
    fprintf(stdout, "Unknown variable referenced: %s\n",
            varName.c_str());
  }

  // Load the value.
  return Builder.CreateLoad(V, varName.c_str());
}



llvm::Value *
OpenSim::CodeGenModule::visit(OpenSim::NumberExprAST *node)
{
  return ConstantFP::get(Type::DoubleTy, APFloat(node->Val()));
}



llvm::Value *
OpenSim::CodeGenModule::visit(OpenSim::BinaryExprAST *node)
{
  Value *L = node->LHS->Codegen(this);
  Value *R = node->RHS->Codegen(this);
  if (L == 0 || R == 0) return 0;
  
  switch (node->Op) 
  {
    case '+': return Builder.CreateAdd(L, R, "addtmp");
    case '-': return Builder.CreateSub(L, R, "subtmp");
    case '*': return Builder.CreateMul(L, R, "multmp");
    case '/': return Builder.CreateFDiv(L, R, "divtmp");
    case '<':
      L = Builder.CreateFCmpULT(L, R, "cmptmp");
      // Convert bool 0/1 to double 0.0 or 1.0
      return Builder.CreateUIToFP(L, Type::DoubleTy, "booltmp");
    default: break;
  }
  
  // we should have returned before here
  return 0;
}
