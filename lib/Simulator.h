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
#include <map>

namespace OpenSim
{
  /// WalkType - This enum is for specifying the different types 
  /// of AST walking we support.
  ///
  enum sim_output 
  {
    sim_emit_IR = 1,      // not supported
    sim_emit_Python = 2,  // full Python implementation of model
    sim_emit_Fortran = 3, // not implemented yet
    sim_emit_Output = 4,  // results of interpreting model
    sim_emit_AS3 = 5,     // full AS3 implementation of model
  };
  
  class SimBuilder;
  class Variable;
  
  
  class Simulator
  {
    /// Name of the loaded model.
    std::string _model_name;
    
    std::string _file_name;
    std::string _output_file_name;
    
    /// Current type of AST walk to take
    sim_output _output_type;
    
    /// Active SimBuilder instance which does the dirty work.
    OpenSim::SimBuilder *_sim_builder;
    
    /// a map of all the variables in the model
    std::map<std::string, OpenSim::Variable *> _variables;
    
    /// Private initialization function which is called from the 
    /// different constructors.
    void init(std::string fileName);
    
  public:
    Simulator();
    Simulator(std::string fileName);
    ~Simulator();
      
    int set_name(std::string modelName);
    std::string name();
    
    int set_model_file(std::string modelFileName);
    std::string model_file();
    
    int set_output_type(sim_output newType);
    
    int set_output_file(std::string outputFileName);
    std::string output_file();
    
    int set_variable_equation(std::string varName, std::string varEqn);
    std::string variable_equation(std::string varName);
    
    int simulate();
  };
}

#endif // OSIM_SIMULATOR_H

