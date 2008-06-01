//===--- IOVenText.h - Vensim text file IO ---------------------*- C++ -*-===//
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
// This imports model data from Vensim text-format models.
//
//===---------------------------------------------------------------------===//


#ifndef OSIM_IOVENTEXT_H
#define OSIM_IOVENTEXT_H

// openSim stuff
#include <cstdio>
#include "IOInterface.h"
#include "../Variable.h"

namespace OpenSim
{
  class Variable;
  
  class IOVenText : public IOInterface
  {
    std::map<std::string, OpenSim::Variable *> vars;
    std::string name;
    bool valid;
    FILE *venFile;

    OpenSim::EquToken cur_tok;
    
    TokenType get_token();
    
  public:
    IOVenText(std::string filePath);
    ~IOVenText();
    
    std::string Name() {return name;}
    bool ValidModelParsed() {return valid;}
    std::map<std::string, OpenSim::Variable *> Variables();
  };    
}

#endif // OSIM_IOVENTEXT_H
