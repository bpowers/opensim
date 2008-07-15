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
#include "../AST/LookupAST.h"

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

  char *c_paths = getenv("XDG_DATA_DIRS");

  // I know mallocing sucks, but is there a better way?!?
  int buf_size = 255;
  char *c_cwd = (char *)malloc(buf_size*sizeof(char));
  getcwd(c_cwd, buf_size);

  string paths = "/usr/local/share/:/usr/share/";
  if (c_paths)
    paths += ":" + string(c_paths);
  if (c_cwd)
    paths += ":" + string(c_cwd);
  else
    fprintf(stderr, "Warning: getcwd failed.\n");

  free(c_cwd);

  fprintf(stderr, "paths: '%s'\n", paths.c_str());

  //start->Codegen(this);
}



double
OpenSim::AS3PrintModule::visit(OpenSim::SimAST *node)
{   
  vars = node->NamedVars();

  Bootstrap();
  
  for (map<string, VariableAST *>::iterator itr = vars.begin(); 
       itr != vars.end(); itr++) 
  {
    VariableAST *v_ast = itr->second;
    Variable *v = v_ast->Data();
    
    // define constants at the top of the file
    if (v->Type() == var_const)
    {
      string constant = "      data[\"" + v->Name() + "\"] = [";
      fprintf(simout, constant.c_str());
      
      v_ast->AST()->Codegen(this);
      
      fprintf(simout, "]\n");
    }
    
    // define lookups, but don't make them part of an array.
    if (v->Type() == var_lookup)
    {
      string constant = "      data[\"" + v->Name() + "\"] = new SimData(";
      fprintf(simout, constant.c_str());
      
      v_ast->AST()->Codegen(this);
      
      fprintf(simout, ")\n");
    }
    
    // define constants at the top of the file
    if (v->Type() == var_stock)
    {
      string stock = "      data[\"" + v->Name() + "\"] = [";
      fprintf(simout, stock.c_str());
      
      v_ast->Initial()->Codegen(this);
      
      fprintf(simout, "]\n");
    }
  }

  fprintf(simout, "      data[\"time\"] = [data[\"OS_start\"][0]]\n");
  
  node->Integrator()->Codegen(this);

  Tailstrap();  

  fprintf(simout, "\n");
  
  return 0;
}



double
OpenSim::AS3PrintModule::visit(OpenSim::EulerAST *node)
{
  string message = whitespace + "#using euler integration\n";
  fprintf(simout, message.c_str());
  
  fprintf(simout, "for time in frange(OS_start, OS_end, OS_timestep):\n");
  
  whitespace += "        ";
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
  
  string updateStocks = "\n" + whitespace + "//updating stocks\n";
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
  
  string index = "[i]";
  if (v->Type() == OpenSim::var_stock)
    index = "[j]";

  string message = whitespace + "data[\"" + v->Name() + "\"]" + index + " = ";
  fprintf(simout, message.c_str());
  
  node->AST()->Codegen(this);
  
  fprintf(simout, "\n");
  
  return 0;
}



double
OpenSim::AS3PrintModule::visit(OpenSim::VarRefAST *node)
{
  string index = "[i]";
  if (vars[node->Name()]->Data()->Type() == OpenSim::var_const)
    index = "[0]";

  fprintf(simout, "data[\"%s\"]%s", node->Name().c_str(), index.c_str());
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
  const std::vector< std::pair<double, double> > table = node->Table();
  
  fprintf(simout, "[");
  
  for (int i=0; i<table.size(); i++)
  {
    fprintf(simout, "[%f, %f]", table[i].first, table[i].second);
    
    if (i<table.size()-1)
      fprintf(simout, ", ");
  }
  
  fprintf(simout, "]");
  
  return 0;
}



double
OpenSim::AS3PrintModule::visit(OpenSim::LookupRefAST *node)
{
  fprintf(simout, "lookup(data[\"%s\"], ", node->TableName().c_str());
  node->ref->Codegen(this);
  fprintf(simout, ")");
  
  return 0;
}



double
OpenSim::AS3PrintModule::visit(OpenSim::FunctionRefAST *node)
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
    
    fprintf(simout, "Math.max(");
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



double
OpenSim::AS3PrintModule::Bootstrap()
{
  fprintf(simout, "\
package opensim\n\
{\n\
  public class OpenSim\n\
  {\n");

  return 0;
}



double
OpenSim::AS3PrintModule::Tailstrap()
{
  fprintf(simout, "\
  }\n\
}\n");

  return 0;
}
