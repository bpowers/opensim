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
#include "../model-variable.h"

#include <cstdlib>
#include <algorithm>
using std::max;
using std::string;
using std::vector;
using std::map;
using std::pair;

using OpenSim::VariableAST;


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
    ModelVariable *v = v_ast->Data();
    
    gchar *v_name = NULL;
    var_type v_type = var_undef;
    
    g_object_get(G_OBJECT(v), "name", &v_name, "type", &v_type, NULL);
    
    // define constants
    if (v_type == var_const)
      vals[v_name] = v_ast->AST()->Codegen(this);
    
    // calculate the initial values of stocks
    if (v_type == var_stock)
      vals[v_name] = v_ast->Initial()->Codegen(this);
    
    g_free(v_name);
  }
  
  string headers = "time";
  
  vector<VariableAST *> body = node->Integrator()->Body();
  for (vector<VariableAST *>::iterator itr = body.begin();
       itr != body.end(); ++itr)
  {
    VariableAST *v_ast = *itr;
    ModelVariable *v = v_ast->Data();
    gchar *v_name = NULL;
    var_type v_type = var_undef;
    
    g_object_get(G_OBJECT(v), "name", &v_name, "type", &v_type, NULL);

    if (v_type == var_stock || v_type == var_aux)
    {
      headers += ",";
      headers += v_name;
    }
    
    g_free(v_name);
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
  double savestep = vars["OS_savestep"]->Codegen(this);
  
  int save_count = 0;
  int save_iterations = savestep/timestep;
  bool do_save = true;
  
  string format = "";
  for (int i=0; i<node->Body().size(); ++i)
    format += "%d,";
  format += "\n";
  
  for (double time = start; time <= end; time += timestep)
  {
    vals["time"] = time;
    if (do_save)
      fprintf(simout, "%f", time);
    
    if (do_save)
    {
      vector<VariableAST *> body = node->Body();
      for (vector<VariableAST *>::iterator itr = body.begin();
           itr != body.end(); ++itr)
      {
        (*itr)->Codegen(this);
        
        gchar *v_name = NULL;
        g_object_get(G_OBJECT((*itr)->Data()), "name", &v_name, NULL);
        
        fprintf(simout, ",%f", vals[v_name]);
        
        g_free(v_name);
      }
    }
    
    if (do_save)
      fprintf(simout, "\n");
    
    for (map<string, VariableAST *>::iterator itr = vars.begin(); 
         itr != vars.end(); itr++) 
    {
      ModelVariable *v = itr->second->Data();
      gchar *v_name = NULL;
      var_type v_type = var_undef;
      
      g_object_get(G_OBJECT(v), "name", &v_name, "type", &v_type, NULL);
      
      // update stocks at end of the loop
      if (v_type == var_stock)
      {
        string name_next = v_name;
        name_next += "_NEXT";
        vals[v_name] = vals[name_next];
      }
      
      g_free(v_name);
    }
    
    // figure out if we should save things the next time around
    // save every n iterations, and also on the last iteration
    save_count++;
    if (save_count >= save_iterations || time + timestep > end)
    {
      do_save = true;
      save_count = 0;
    }
    else
      do_save = false;
  }
  
  return 0;
}



double
OpenSim::InterpreterModule::visit(OpenSim::VariableAST *node)
{
  ModelVariable *v = node->Data();
  
  gchar *v_name = NULL;
  var_type v_type = var_undef;
  
  g_object_get(G_OBJECT(v), "name", &v_name, "type", &v_type, NULL);
  
  string next = v_name;
  g_free(v_name);
  
  if (v_type == var_stock)
  {
    next += "_NEXT";
  }

  double value = node->AST()->Codegen(this);
  vals[next] = value;
  
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

