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
#include "../AST/LookupAST.h"
#include "../AST/General.h"

#include <cstdlib>
#include <algorithm>
using std::max;
using std::string;
using std::vector;
using std::map;
using std::pair;

using OpenSim::VariableAST;
using OpenSim::Variable;


OpenSim::InterpreterModule::InterpreterModule()
{
}



OpenSim::InterpreterModule::~InterpreterModule() 
{
}



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
  
  for (int i=0; i < node->Initial().size(); i++) 
  {
    VariableAST *v_ast = node->Initial()[i];
    Variable *v = v_ast->Data();
    
    // define constants at the top of the file
    if (v->Type() == var_const)
      vals[v->Name()] = v_ast->AST()->Codegen(this);
    
    // define constants at the top of the file
    if (v->Type() == var_stock)
      vals[v->Name()] = v_ast->Initial()->Codegen(this);
  }
  
  string headers = "time";
  vector<VariableAST *> body = node->Integrator()->Body();
  for (vector<VariableAST *>::iterator itr = body.begin();
       itr != body.end(); ++itr)
  {
    VariableAST *v_ast = *itr;
    Variable *v = v_ast->Data();

    if (v->Type() == var_stock || v->Type() == var_aux)
      headers += "," + v->Name();
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
    vals["time"] = time;
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
  LookupAST *lookup = (LookupAST *) vars[node->TableName()]->AST();
  
  const vector< pair<double, double> > table = lookup->Table();

  if (table.size() == 0) return 0;
  
  double index = node->ref->Codegen(this);
  
  // if the request is outside the min or max, then we return 
  // the nearest element of the array
  if (index < table[0].first)
    return table[0].second;
  else if (index > table[table.size()-1].first)
    return table[table.size()-1].second;
  
  for (int i=0; i < table.size(); i++)
  {
    if (index == table[i].first)
      return table[i].second;
    
    if (index < table[i].first)
    {
      // slope = deltaY/deltaX
      float slope = (table[i].second - table[i-1].second)
                    /(table[i].first - table[i-1].first);
      
      return (index-table[i-1].first)*slope + table[i-1].second;
    }
  }
  
  return 0;
}



double
OpenSim::InterpreterModule::visit(OpenSim::FunctionRefAST *node)
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
    
    return max(args[0]->Codegen(this), args[1]->Codegen(this));
  }
  
  fprintf(stderr, "Error: Unknown function '%s' referenced.\n", 
          node->FunctionName().c_str());
  
  return 0;
}
