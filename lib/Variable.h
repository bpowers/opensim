//===--- Variable.h - Base class for representing sim variables ----------===//
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
// This class represents the AST nodes of Variables.
//
//===---------------------------------------------------------------------===//

#ifndef OSIM_VARIABLE_H
#define OSIM_VARIABLE_H

#include <string>
#include <vector>

namespace OpenSim
{
  enum varType
  {
    var_stock = 1,
    var_aux = 2,
    var_lookup = 3,
    var_const = 4,
    
    var_undef = -1
  };
  
  
  
  /// TokenType - This enum contiains definitions of the types of tokens 
  /// recognized in the equation field of variables.
  ///
  enum TokenType 
  {
    tok_eof = -1,
    
    // commands
    tok_def = -2, tok_extern = -3,
    
    // primary
    tok_identifier = -4, tok_number = -5,
    
    // control
    tok_if = -6, tok_then = -7, tok_else = -8,
    tok_for = -9, tok_in = -10,
    
    // operators
    tok_operator = -11,
    
    
    // var definition
    tok_var = -13
  };
  
  
  /// EquToken - This struct represents tokens found in variable equations.
  ///
  struct EquToken
  {
    std::string Identifier;
    TokenType Type;
    double NumVal;
    char Op;
  };
  
  
  
  class Variable
  {
    std::string name;
    std::string equation;
    OpenSim::varType type;
    
    std::vector<OpenSim::EquToken> tokens;
    
    int tokenize();
    
  public:
    Variable() {}
    Variable(std::string pName, std::string pEquation)
      : name(pName), equation(pEquation) { type = var_undef;}
    virtual ~Variable() {}
    
    void SetName(std::string newName) {name = newName;}
    std::string Name() {return name;}
    
    OpenSim::varType Type();
    
    void SetEquation(std::string newEquation);
    std::string Equation() {return equation;}
    
    std::vector<OpenSim::EquToken> EquationTokens();
  };
}

#endif // OSIM_VARIABLE_H
