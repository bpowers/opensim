//===--- IOxml.h - XML file IO ---------------------------------*- C++ -*-===//
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
// This inputs model data in XML form from a file and creates a vector
// of variables.
// TODO: implement saving through consuming the AST.
//
//===---------------------------------------------------------------------===//

#ifndef OSIM_IOXML_H
#define OSIM_IOXML_H

// libxml parsing
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

// openSim stuff
#include "IOInterface.h"


namespace OpenSim
{
  class Variable;
	    
  class IOxml : public IOInterface
  {
    std::map<std::string, OpenSim::Variable *> vars;
    std::string name;
    bool valid;
    
    void ParseInput(xmlDocPtr doc, xmlNodePtr cur);
    
  public:
    IOxml(std::string filePath);
    IOxml(std::string filePath, char read_write, bool partial, 
          std::map<std::string, OpenSim::Variable *> vars, 
          std::string model_name);
    ~IOxml();

    std::string Name() {return name;}
    bool ValidModelParsed() {return valid;}
    std::map<std::string, OpenSim::Variable *> Variables();
  };    
}

#endif // OSIM_IOXML_H
