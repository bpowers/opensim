//===--- AS3PrintModule.cpp - Pretty AST printer ----------------------===//
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

#include "AS3PrintModule.h"
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

OpenSim::AS3PrintModule::AS3PrintModule()
{
}



OpenSim::AS3PrintModule::~AS3PrintModule() {}



void 
OpenSim::AS3PrintModule::Consume(OpenSim::SimAST *start, FILE *output_file)
{
  simout = output_file;
  start->Codegen(this);
}



double
OpenSim::AS3PrintModule::visit(OpenSim::SimAST *node)
{   
  vars = node->NamedVars();
  
  for (map<string, VariableAST *>::iterator itr = vars.begin(); 
       itr != vars.end(); itr++) 
  {
    VariableAST *v_ast = itr->second;
    Variable *v = v_ast->Data();
    
    // define constants at the top of the file
    if (v->Type() == var_const)
    {
      string constant = v->Name() + " = ";
      fprintf(simout, constant.c_str());
      
      v_ast->AST()->Codegen(this);
      
      fprintf(simout, "\n");
    }
    
    // define constants at the top of the file
    if (v->Type() == var_stock)
    {
      string stock = v->Name() + " = ";
      fprintf(simout, stock.c_str());
      
      v_ast->Initial()->Codegen(this);
      
      fprintf(simout, "\n");
    }
  }
  
  node->Integrator()->Codegen(this);
  
  fprintf(simout, "\n");
  
  return 0;
}



double
OpenSim::AS3PrintModule::visit(OpenSim::EulerAST *node)
{
  string message = whitespace + "#using euler integration\n";
  fprintf(simout, message.c_str());
  
  fprintf(simout, "for time in frange(OS_start, OS_end, OS_timestep):\n");
  
  whitespace += "  ";
  string format_statement = "%f";
  string variable_list = "time";
  
  vector<VariableAST *> body = node->Body();
  for (vector<VariableAST *>::iterator itr = body.begin();
       itr != body.end(); ++itr)
  {
    // only codegen non-stocks first
    if ((*itr)->Data()->Type() != var_stock) (*itr)->Codegen(this);
    
    // for print statements...
    format_statement += ",%f";
    variable_list += ", " + (*itr)->Data()->Name();
  }
  
  string prints = "\n" + whitespace + "#generally put print statements here\n";
  fprintf(simout, prints.c_str());
  string printout = whitespace + "print('" + format_statement + "' % ("
  + variable_list + "))\n";
  fputs(printout.c_str(), simout);
  
  string updateStocks = "\n" + whitespace + "#updating stocks\n";
  fprintf(simout, updateStocks.c_str());
  
  
  for (vector<VariableAST *>::iterator itr = body.begin();
       itr != body.end(); ++itr)
  {
    // now update the stocks at the end
    if ((*itr)->Data()->Type() == var_stock) (*itr)->Codegen(this);
  }
  
  return 0;
}



double
OpenSim::AS3PrintModule::visit(OpenSim::VariableAST *node)
{
  Variable *v = node->Data();
  
  string message = whitespace + v->Name() + " = ";
  fprintf(simout, message.c_str());
  
  node->AST()->Codegen(this);
  
  fprintf(simout, "\n");
  
  return 0;
}



double
OpenSim::AS3PrintModule::visit(OpenSim::VarRefAST *node)
{
  fprintf(simout, node->Name().c_str());
  return 0;
}



double
OpenSim::AS3PrintModule::visit(OpenSim::NumberExprAST *node)
{
  fprintf(simout, "%f", node->Val());
  return 0;
}



double
OpenSim::AS3PrintModule::visit(OpenSim::BinaryExprAST *node)
{
  fprintf(simout, "(");
  node->LHS->Codegen(this);
  fprintf(simout, " %c ", node->Op);
  node->RHS->Codegen(this);
  fprintf(simout, ")");
  
  return 0;
}



double
OpenSim::AS3PrintModule::visit(OpenSim::UnaryExprAST *node)
{
  fprintf(simout, "%c", node->Op);
  
  node->RHS->Codegen(this);
  
  return 0;
}



double
OpenSim::AS3PrintModule::visit(OpenSim::LookupAST *node)
{
  fprintf(stderr, "Warning: visit unimplemented for LookupAST\n");
  
  return 0;
}



double
OpenSim::AS3PrintModule::visit(OpenSim::LookupRefAST *node)
{
  fprintf(stderr, "Warning: visit unimplemented for LookupRefAST\n");
  
  return 0;
}



double
OpenSim::AS3PrintModule::visit(OpenSim::FunctionRefAST *node)
{
  fprintf(stderr, "Warning: visit unimplemented for FunctionRefAST\n");
  
  return 0;
}
