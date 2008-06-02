//===--- SimAST.cpp - Root AST for models --------------------------------===//
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
// This class is the implementation root node for an OpenSim AST
//
//===---------------------------------------------------------------------===//

#include "SimAST.h"
#include "ASTConsumer.h"
#include "EulerAST.h"

OpenSim::SimAST::SimAST(OpenSim::EulerAST *intLoop, 
                        std::vector<OpenSim::VariableAST * > initialASTs,
                        std::map<std::string, OpenSim::VariableAST *> named)
{
  initial = initialASTs;
  integrator = intLoop;
  namedVars = named;
}



OpenSim::SimAST::~SimAST() {}



std::map<std::string, OpenSim::VariableAST *> 
OpenSim::SimAST::NamedVars()
{
  return namedVars;
}



double
OpenSim::SimAST::Codegen(OpenSim::ASTConsumer *owner)
{
  return owner->visit(this);
}


std::vector<OpenSim::VariableAST *> 
OpenSim::SimAST::Initial()
{
  return initial;
}
