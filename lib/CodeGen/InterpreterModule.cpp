//===--- PythonPrintModule.cpp - Pretty AST printer ----------------------===//
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

#include "InterpreterModule.h"
#include "../AST/SimAST.h"
#include "../AST/EulerAST.h"
#include "../AST/VariableAST.h"
#include "../AST/General.h"

#include <cstdlib>
using std::string;
using std::vector;
using std::map;

using OpenSim::VariableAST;
using OpenSim::Variable;

OpenSim::InterpreterModule::InterpreterModule()
{
}



OpenSim::InterpreterModule::~InterpreterModule() {}



void 
OpenSim::InterpreterModule::Consume(OpenSim::SimAST *start, FILE *output_file)
{
  simout = output_file;
  start->Codegen(this);
}



double
OpenSim::InterpreterModule::visit(OpenSim::SimAST *node)
{
  vars = node->NamedVars();
  string headers = "time";
  
  for (map<string, VariableAST *>::iterator itr = vars.begin(); 
       itr != vars.end(); itr++) 
  {
    VariableAST *v_ast = itr->second;
    Variable *v = v_ast->Data();
    
    
    // define constants at the top of the file
    if (v->Type() == var_const)
    {
      vals[v->Name()] = v_ast->AST()->Codegen(this);
    }
    
    if (v->Type() == var_aux)
    {
      headers += "," + v->Name();
    }
    
    // define constants at the top of the file
    if (v->Type() == var_stock)
    {
      vals[v->Name()] = v_ast->Initial()->Codegen(this);
      headers += "," + v->Name();
    }
  }
  
  headers += "\n";
  fprintf(simout, headers.c_str());
  
  node->Integrator()->Codegen(this);
  
  return 0;
}



double
OpenSim::InterpreterModule::visit(OpenSim::EulerAST *node)
{
  double start = vars["OS_start"]->Codegen(this);
  double end = vars["OS_end"]->Codegen(this);
  double timestep = vars["OS_timestep"]->Codegen(this);
  
  string format = "";
  for (int i=0; i<node->Body().size(); ++i)
    format += "%d,";
  
  format += "\n";
  
  for (double time = start; time <= end; time += timestep)
  {
    fprintf(simout, "%f", time);
    
    vector<VariableAST *> body = node->Body();
    for (vector<VariableAST *>::iterator itr = body.begin();
         itr != body.end(); ++itr)
    {
      (*itr)->Codegen(this);
      fprintf(simout, ",%f", vals[(*itr)->Data()->Name()]);
    }
    
    fprintf(simout, "\n");
    
    for (map<string, VariableAST *>::iterator itr = vars.begin(); 
         itr != vars.end(); itr++) 
    {
      Variable *v = itr->second->Data();
      
      // update stocks at end of the loop
      if (v->Type() == var_stock)
      {
        vals[v->Name()] = vals[v->Name() + "_NEXT"];
      }
    }
  }
  
  return 0;
}



double
OpenSim::InterpreterModule::visit(OpenSim::VariableAST *node)
{
  Variable *v = node->Data();
  string next = "";
  
  if (v->Type() == var_stock)
  {
    next = "_NEXT";
  }

  double value = node->AST()->Codegen(this);
  vals[v->Name() + next] = value;
  
  return value;
}



double
OpenSim::InterpreterModule::visit(OpenSim::VarRefAST *node)
{
  return vals[node->Name()];
}



double
OpenSim::InterpreterModule::visit(OpenSim::NumberExprAST *node)
{
  return node->Val();
}



double
OpenSim::InterpreterModule::visit(OpenSim::UnaryExprAST *node)
{
  double R = node->RHS->Codegen(this);
  
  switch (node->Op) 
  {
    case '-': return -R;
    default: break;
  }
  
  // whoops.
  fprintf(simout, "Error: Unsupported unary op '%c'.\n", node->Op);

  return 0;
}



double
OpenSim::InterpreterModule::visit(OpenSim::BinaryExprAST *node)
{
  double L = node->LHS->Codegen(this);
  double R = node->RHS->Codegen(this);
  
  switch (node->Op) 
  {
    case '+': return L + R;
    case '-': return L - R;
    case '*': return L * R;
    case '/': return L / R;
    case '<': return L < R;
    default: break;
  }
  
  return 0;
}



double
OpenSim::InterpreterModule::visit(OpenSim::LookupAST *node)
{
  fprintf(stderr, "Warning: visit unimplemented for LookupAST\n");
  
  return 0;
}



double
OpenSim::InterpreterModule::visit(OpenSim::LookupRefAST *node)
{
  fprintf(stderr, "Warning: visit unimplemented for LookupRefAST\n");
  
  return 0;
}
