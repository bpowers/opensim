//===--- VariableAST.h - Base AST class for sim variables ------*- C++ -*-===//
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

#ifndef OSIM_LOOKUPAST_H
#define OSIM_LOOKUPAST_H

// openSim stuff
#include "../globals.h"
#include "../opensim-variable.h"
#include "General.h"
#include "ASTConsumer.h"
#include "../CodeGen/InterpreterModule.h"
#include <utility>

namespace OpenSim
{
  class LookupAST : public ExprAST
    {
    protected:
      OpensimVariable *var;
      std::vector< std::pair<double, double> > tuples;
      
    public:
      LookupAST(OpensimVariable *varData, 
                std::vector< std::pair<double, double> > lookup_data) 
      : var(varData), tuples(lookup_data) {}
      ~LookupAST() {}
      
      void SetData(OpensimVariable *newData);
      OpensimVariable *Data();
      
      const std::vector< std::pair<double, double> > Table();
      
      double Codegen(OpenSim::ASTConsumer *owner);
    };
}

#endif // OSIM_LOOKUPAST_H
