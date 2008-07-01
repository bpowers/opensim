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

#include <glibmm/init.h>

// openSim stuff
#include "globals.h"
#include "CodeGen/SimBuilder.h"
#include "AST/VariableAST.h"
#include "IO/IOxml.h"
#include "IO/IOVenText.h"
using OpenSim::IOxml;
using std::string;
using std::map;

void
OpenSim::force_cpp_glib_init()
{
  // IMPORTANT: glib needs to be initialized before the constructors are called
  Glib::init();
}

OpenSim::Simulator::Simulator()
{
  // important!
  //Glib::init();

  fprintf(stdout, "created new Simulator\n");
  _output_file_name = "";
  
  _output_type = sim_emit_Output;
}



OpenSim::Simulator::Simulator(std::string fileName)
{
  // important!
  //Glib::init();

  //fprintf(stdout, "created new Simulator from file\n");
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
}



std::string
OpenSim::Simulator::model_file() 
{
  return _file_name;
}



int
OpenSim::Simulator::simulate()
{
  // we'll want to produce a lot more warnings and checks in the future
  // but this should prevent the biggest segfaults...
  if (_sim_builder)
  {
    FILE *outputStream = NULL;
    if (_output_file_name != "") 
    {
      outputStream = fopen(_output_file_name.c_str(), "w+");
      
      if (!outputStream) 
      {
        fprintf(stderr, "Error: Could not open output file for writing.\n");
        return -1;
      }
    }
    else 
    {
      outputStream = stdout;
    }
    
    int parse_status = _sim_builder->Parse(_output_type, outputStream);
    
    if (_output_type == sim_emit_Output)
    {
      _results = _sim_builder->Results();
    }

    // if we opened it, close the output stream
    if (outputStream != stdout) fclose(outputStream);
    
    return parse_status;
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
    fprintf(stderr, "Error: Output file must be different from model file.\n");
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
  map<string, Variable *>::iterator v = _variables.find(varName); 
  
  // check to see if we didn't find anything
  if (v == _variables.end())
  {
    fprintf(stderr, 
            "Error: Variable '%s' doesn't exist, so can't set equation.\n", 
            varName.c_str());
    return -1;
  }
  
  v->second->SetEquation(varEqn);
  _sim_builder->Update();
  
  return 0;
}



std::string 
OpenSim::Simulator::variable_equation(std::string varName)
{
  map<string, Variable *>::iterator v = _variables.find(varName); 
  
  // check to see if we didn't find anything
  if (v == _variables.end())
  {
    fprintf(stderr, "Error: Variable '%s' doesn't exist, so no equation.\n", 
            varName.c_str());
    return "[null]";
  }
  
  return v->second->Equation();
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
