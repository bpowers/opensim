//===--- LookupAST.cpp - AST class for lookup variables -------------------===//
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

#include "LookupAST.h"
#include "General.h"
#include "ASTConsumer.h"
using OpenSim::ASTConsumer;

void 
OpenSim::LookupAST::SetData(OpensimVariable *newData)
{
  if (newData) var = newData;
}



OpensimVariable *
OpenSim::LookupAST::Data()
{
  return var;
}



const std::vector< std::pair<double, double> > 
OpenSim::LookupAST::Table()
{
  return tuples;
}

double
OpenSim::LookupAST::Codegen(ASTConsumer *owner)
{
  return owner->visit(this);
}
