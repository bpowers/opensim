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
#include "CodeGen/SimBuilder.h"
#include "AST/VariableAST.h"
#include "IO/IOxml.h"
#include "IO/IOVenText.h"
using OpenSim::IOxml;
using std::string;



OpenSim::Simulator::Simulator(std::string fileName)
{
  // call the init function, load the model and construct its AST
  init(fileName);
  
  file_name = fileName;
  outputFile = "";
  
  outputType = walk_Interpret;
}



OpenSim::Simulator::~Simulator()
{
  delete simBuilder;
}



int
OpenSim::Simulator::Simulate()
{
  // we'll want to produce a lot more warnings and checks in the future
  // but this should prevent the biggest segfaults...
  if (simBuilder)
  {
    FILE *outputStream = NULL;
    if (outputFile != "") 
    {
      outputStream = fopen(outputFile.c_str(), "w+");
      
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
    
    int parse_status = simBuilder->Parse(outputType, outputStream);
    
    // if we opened it, close the output stream
    if (outputStream != stdout) fclose(outputStream);
    
    return parse_status;
  }
  
  return -1;
}



int
OpenSim::Simulator::SetOutputType(WalkType newType)
{
  // simple error checking that will have to be updated whenever we
  // add a new type of output
  int intWalk = (int) newType;
  if (intWalk < 1 || 5 < intWalk) return -1;
  
  outputType = newType;
  
  return 0;
}



int
OpenSim::Simulator::SetOutputFile(std::string outputFileName)
{
  // don't let people write the data over the model
  if (outputFileName == file_name)
  {
    fprintf(stderr, "Error: Output file must be different from model file.\n");
    return -1;
  }
  
  outputFile = outputFileName;
  
  return 0;
}



void
OpenSim::Simulator::init(std::string fileName)
{
  // initialize simBuilder to zero so we can test and make sure it was
  // created alright
  simBuilder = NULL;

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
    simBuilder = new SimBuilder(file->Variables());
    name = file->Name();
  }
  
  delete file;
}
