//===--- IOInterface.h - OpenSim IO interface ------------------*- C++ -*-===//
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
// This lays out the interface that IO classes need to follow.
//
//===---------------------------------------------------------------------===//

#ifndef OSIM_IOINTERFACE_H
#define OSIM_IOINTERFACE_H

// openSim stuff
#include "../globals.h"


namespace OpenSim
{
  class Variable;
  
  class IOInterface
  {
  public:
    IOInterface() {}
    IOInterface(std::string filePath) {}
    IOInterface(std::string filePath, char read_write, bool partial, 
                std::map<std::string, OpenSim::Variable *> vars) {}
    virtual ~IOInterface() {}
      
    virtual std::string Name() = 0;
    virtual bool ValidModelParsed() = 0;
    virtual std::map<std::string, OpenSim::Variable *> Variables() = 0;
  };    
}

#endif // OSIM_IOINTERFACE_H
