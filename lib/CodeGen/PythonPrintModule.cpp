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

#include "PythonPrintModule.h"
#include "../AST/SimAST.h"
#include "../AST/EulerAST.h"
#include "../AST/VariableAST.h"
#include "../AST/General.h"
#include "../AST/LookupAST.h"

#include <cstdlib>
using std::pair;
using std::string;
using std::vector;
using std::map;

using OpenSim::VariableAST;
using OpenSim::Variable;

OpenSim::PythonPrintModule::PythonPrintModule()
{
}



OpenSim::PythonPrintModule::~PythonPrintModule() {}



void 
OpenSim::PythonPrintModule::Consume(OpenSim::SimAST *start, FILE *output_file)
{
  simout = output_file;
  start->Codegen(this);
}



double
OpenSim::PythonPrintModule::visit(OpenSim::SimAST *node)
{
  // define frange to allow for a range function with 
  // floating point arguments
  fprintf(simout, "#!/usr/bin/env python\n\
import math\n\
\n\
# standard python range function doesn't allow floating point\n\
# increments in ranges, so we define our own\n\
def frange(lim_start, lim_end, increment = 1.):\n\
  lim_start = float(lim_start)\n\
  count = int(math.ceil(lim_end - lim_start)/increment + 1)\n\
  return (lim_start + n*increment for n in range(count))\n\
\n\
\n\
# simple lookup table implementation\n\
def sim_lookup(table, index):\n\
\n\
  if len(table) is 0: return 0\n\
\n\
  # if the request is outside the min or max, then we return\n\
  # the nearest element of the array\n\
  if   index < table[0][0]:  return table[0][1]\n\
  elif index > table[-1][0]: return table[-1][1]\n\
\n\
  for i in range(0, len(table)):\n\
    x, y = table[i]\n\
\n\
    if index == x: return y\n\
    if index < x:\n\
      # slope = deltaY/deltaX\n\
      slope = (y - table[i-1][1])/(x - table[i-1][0])\n\
      return (index-table[i-1][0])*slope + table[i-1][1]\n\n\n");
  
  vars = node->NamedVars();
  
  for (int i=0; i < node->Initial().size(); i++) 
  {
    VariableAST *v_ast = node->Initial()[i];
    Variable *v = v_ast->Data();
    
    // define constants at the top of the file
    if (v->Type() == var_const || v->Type() == var_lookup)
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
  
  string headers = "print('time";
  vector<VariableAST *> body = node->Integrator()->Body();
  for (vector<VariableAST *>::iterator itr = body.begin();
       itr != body.end(); ++itr)
  {
    VariableAST *v_ast = *itr;
    Variable *v = v_ast->Data();
    
    if (v->Type() == var_stock || v->Type() == var_aux)
      headers += "," + v->Name();
  }
  headers += "')\n\n";
  fprintf(simout, headers.c_str());
  
  node->Integrator()->Codegen(this);
  
  fprintf(simout, "\n");
  
  return 0;
}



double
OpenSim::PythonPrintModule::visit(OpenSim::EulerAST *node)
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
OpenSim::PythonPrintModule::visit(OpenSim::VariableAST *node)
{
  Variable *v = node->Data();
  
  string message = whitespace + v->Name() + " = ";
  fprintf(simout, message.c_str());
  
  node->AST()->Codegen(this);
  
  fprintf(simout, "\n");
  
  return 0;
}



double
OpenSim::PythonPrintModule::visit(OpenSim::VarRefAST *node)
{
  fprintf(simout, node->Name().c_str());
  return 0;
}



double
OpenSim::PythonPrintModule::visit(OpenSim::NumberExprAST *node)
{
  fprintf(simout, "%f", node->Val());
  return 0;
}



double
OpenSim::PythonPrintModule::visit(OpenSim::BinaryExprAST *node)
{
  fprintf(simout, "(");
  node->LHS->Codegen(this);
  fprintf(simout, " %c ", node->Op);
  node->RHS->Codegen(this);
  fprintf(simout, ")");

  return 0;
}



double
OpenSim::PythonPrintModule::visit(OpenSim::UnaryExprAST *node)
{
  fprintf(simout, "%c", node->Op);
  
  node->RHS->Codegen(this);
  
  return 0;
}



double
OpenSim::PythonPrintModule::visit(OpenSim::LookupAST *node)
{
  const std::vector< std::pair<double, double> > table = node->Table();
  
  fprintf(simout, "[");
  
  for (int i=0; i<table.size(); i++)
  {
    fprintf(simout, "(%f, %f)", table[i].first, table[i].second);
    
    if (i<table.size()-1)
      fprintf(simout, ", ");
  }
  
  fprintf(simout, "]");
  
  return 0;
}



double
OpenSim::PythonPrintModule::visit(OpenSim::LookupRefAST *node)
{
  fprintf(simout, "sim_lookup(%s, ", node->TableName().c_str());
  node->ref->Codegen(this);
  fprintf(simout, ")");
  
  return 0;
}



double
OpenSim::PythonPrintModule::visit(OpenSim::FunctionRefAST *node)
{
  if (node->FunctionName() == "MAX")
  {
    const std::vector<ExprAST *> args = node->Args();
    
    if (args.size() != 2)
    {
      fprintf(stderr, "Error: MAX function takes 2, not %d, arguments.\n", 
              args.size());
      return 0;
    }
    
    fprintf(simout, "max(");
    args[0]->Codegen(this);
    fprintf(simout, ",");
    args[1]->Codegen(this);
    fprintf(simout, ")");
    
    return 0;
  }
  
  fprintf(stderr, "Error: Unknown function '%s' referenced.\n", 
          node->FunctionName().c_str());
  
  return 0;
}
