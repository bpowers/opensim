//===--- Simulator.cpp - Base class for interacting with models ----------===//
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
// This class represents models at a high level, suitible for inclusion
// in projects as part of a library.
// TODO: implement features for dynamically changing models.
//
//===---------------------------------------------------------------------===//

#include <cstdio>
#include "Simulator.h"

// openSim stuff
#include "globals.h"
#include <utility>
#include "CodeGen/SimBuilder.h"
#include "AST/VariableAST.h"
#include "IO/IOxml.h"
#include "IO/IOVenText.h"
using OpenSim::IOxml;
using std::string;
using std::map;


OpenSim::Simulator::Simulator()
{
  _sim_builder = NULL;
  _output_file_name = "";
  
  _output_type = sim_emit_Output;

  _variables["OS_start"] = new Variable("OS_start", "0");
  _variables["OS_end"] = new Variable("OS_end", "100");
  _variables["OS_savestep"] = new Variable("OS_savestep", "1");
  _variables["OS_timestep"] = new Variable("OS_timestep", ".125");
}



OpenSim::Simulator::Simulator(std::string fileName)
{
  // call the init function, load the model and construct its AST
  init(fileName);
  
  _file_name = fileName;
  _output_file_name = "";
  
  _output_type = sim_emit_Output;
}



OpenSim::Simulator::~Simulator()
{
  delete _sim_builder;
}



int
OpenSim::Simulator::set_name(std::string modelName)
{
  _model_name = modelName;
  
  return 0;
}



std::string
OpenSim::Simulator::name() 
{
  return _model_name;
}



int
OpenSim::Simulator::set_model_file(std::string modelFileName)
{
  _file_name = modelFileName;
  
  return 0;
}



int
OpenSim::Simulator::save()
{
  return save(false);
}



int
OpenSim::Simulator::save(bool partial)
{
  IOInterface *file = new IOxml(_file_name, 'w', true, 
                                _variables, _model_name);
  
  delete file;

  return 0;
}



std::string
OpenSim::Simulator::model_file() 
{
  return _file_name;
}



int
OpenSim::Simulator::info()
{
  fprintf(stdout, "# vars: %d\n", _variables.size());

  return 0;
}


void
OpenSim::Simulator::sim_thread()
{
  _parse_status = _sim_builder->Parse(_output_type, _output_stream);
  
  if (_output_type == sim_emit_Output)
  {
    _results = _sim_builder->Results();
  }
  
  return;
}


int
OpenSim::Simulator::simulate()
{
  // we'll want to produce a lot more warnings and checks in the future
  // but this should prevent the biggest segfaults...
  if (_sim_builder)
  {
    if (_output_file_name != "") 
    {
      _output_stream = fopen(_output_file_name.c_str(), "w+");
      
      if (!_output_stream) 
      {
        fprintf(stderr, _("Error: Could not open output file for writing.\n"));
        return -1;
      }
    }
    else 
    {
      _output_stream = stdout;
    }
    
    _parse_status = WALK_BAILED;

    this->sim_thread();

    // if we opened it, close the output stream
    if (_output_stream != stdout) fclose(_output_stream);
    
    if (_parse_status == WALK_BAILED)
    {
      fprintf(stderr, _("Error: a problem occured during simulation.\n"));
    }
    
    return _parse_status;
  }
  
  return -1;
}



int
OpenSim::Simulator::set_output_type(sim_output newType)
{
  // simple error checking that will have to be updated whenever we
  // add a new type of output
  int intWalk = (int) newType;
  if (intWalk < 1 || 5 < intWalk) return -1;
  
  _output_type = newType;
  
  return 0;
}



int
OpenSim::Simulator::set_output_file(std::string outputFileName)
{
  // don't let people write the data over the model
  if (outputFileName == _file_name)
  {
    fprintf(stderr, _("Error: Output file must be different from model file.\n"));
    return -1;
  }
  
  _output_file_name = outputFileName;
  
  return 0;
}



std::string 
OpenSim::Simulator::output_file()
{
  return _output_file_name;
}



int 
OpenSim::Simulator::set_variable_equation(std::string varName, 
                                          std::string varEqn)
{
  string varClean = this->clean_name(varName);
  
  map<string, Variable *>::iterator v = _variables.find(varClean); 
  
  // check to see if we didn't find anything
  if (v == _variables.end())
  {
    fprintf(stderr, 
            _("Error: Variable '%s' doesn't exist, so can't set equation.\n"), 
            varClean.c_str());
    return -1;
  }
  
  v->second->SetEquation(varEqn);
  //_sim_builder->Update();
  
  return 0;
}



std::string 
OpenSim::Simulator::get_variable_equation(std::string varName)
{
  string varClean = this->clean_name(varName);
  
  map<string, Variable *>::iterator v = _variables.find(varClean); 
  
  // check to see if we didn't find anything
  if (v == _variables.end())
  {
    fprintf(stderr, _("Error: Variable '%s' doesn't exist, so no equation.\n"),
            varClean.c_str());
    return "[null]";
  }
  
  return v->second->Equation();
}



int 
OpenSim::Simulator::new_variable(std::string varName)
{
  //fprintf(stderr, "Debug: Simulator: creating new variable '%s'\n", 
  //        varName.c_str());

  string varClean = this->clean_name(varName);

  map<string, Variable *>::iterator v = _variables.find(varClean); 
  
  // check to make sure variable doesn't exist
  if (v != _variables.end())
  {
    fprintf(stderr, 
            _("Error: Variable '%s' allready exists, can't make a new one.\n"),
            varClean.c_str());
    return -1;
  }

  _variables[varClean] = new Variable(varClean, "");

  //_sim_builder->Update();

  return 0;
}



int 
OpenSim::Simulator::rename_variable(std::string varName, std::string newName)
{
  //fprintf(stderr, "Debug: Simulator: renaming variable '%s' to '%s'\n", 
  //        varName.c_str(), newName.c_str());


  string newClean = this->clean_name(newName);

  map<string, Variable *>::iterator v = _variables.find(newClean);
  if (v != _variables.end())
  {
    fprintf(stderr, "Error: Var with name '%s', already exists, no rename.\n",
            newClean.c_str());
    // return -2 so that we can tell the variable it is not allowed 
    // to rename itself
    return -2;
  }

  string varClean = this->clean_name(varName);

  v = _variables.find(varClean); 
  
  // check to see if we didn't find anything
  if (v == _variables.end())
  {
    fprintf(stderr, "Error: No variable '%s', so can't change name.\n", 
            varClean.c_str());
    return -1;
  }

  Variable *var = v->second;
  _variables.erase(v);
  var->SetName(newClean);
  _variables[newClean] = var;
  
  //_sim_builder->Update();

  return 0;
}



int 
OpenSim::Simulator::delete_variable(std::string varName)
{
  return 0;
}



std::string
OpenSim::Simulator::clean_name(std::string varName)
{
  string clean = varName;
  for (int i=varName.find(' '); i != -1; i=varName.find(' ', i+1))
    clean[i] = '_';

  return clean;
}



void
OpenSim::Simulator::init(std::string fileName)
{
  // initialize simBuilder to zero so we can test and make sure it was
  // created alright
  _sim_builder = NULL;

  IOInterface *file;
  
  
  // check if its a Vensim model.  if it isn't, assume XML.
  string extension = "";
  if (fileName.length() > 3)
    extension = fileName.substr(fileName.length()-3, 3);
  if (extension == "mdl")
  {
    file = new IOVenText(fileName);
  }
  else
  {
    file = new IOxml(fileName);
  }
  
  if (file->ValidModelParsed())
  {
    _variables = file->Variables();
    _sim_builder = new SimBuilder(_variables);
    _model_name = file->Name();
  }
  
  delete file;
}

