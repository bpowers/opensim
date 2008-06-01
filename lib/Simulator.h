//===--- Simulator.h - Base class for interacting with models  -*- C++ -*-===//
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

#ifndef OSIM_SIMULATOR_H
#define OSIM_SIMULATOR_H

#include <string>

namespace OpenSim
{
  /// WalkType - This enum is for specifying the different types 
  /// of AST walking we support.
  ///
  enum WalkType 
  {
    walk_IR = 1,
    walk_Python = 2,
    walk_Fortran = 3,
    walk_Interpret = 4,
    walk_AS3 = 5
  };
  
  class SimBuilder;
  
  
  class Simulator
  {
    /// Name of the loaded model.
    std::string name;
    
    std::string file_name;
    std::string outputFile;
    
    /// Current type of AST walk to take
    WalkType outputType;
    
    /// Active SimBuilder instance which does the dirty work.
    OpenSim::SimBuilder *simBuilder;
    
    /// Private initialization function which is called from the 
    /// different constructors.
    void init(std::string fileName);
    
  public:
    Simulator(std::string fileName);
    ~Simulator();
      
    std::string Name() {return name;}
    int SetOutputType(WalkType newType);
    int SetOutputFile(std::string outputFileName);
    int Simulate();
  };
}

#endif // OSIM_SIMULATOR_H

