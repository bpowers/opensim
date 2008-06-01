//===--- EulerAST.cpp - AST node for Euler integration -------------------===//
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
// This represents the root do/for loop in a model which uses a 
// Euler integration method.
//
//===---------------------------------------------------------------------===//

#include "EulerAST.h"
#include "ASTConsumer.h"
using std::vector;
using OpenSim::VariableAST;


OpenSim::EulerAST::EulerAST(std::vector<OpenSim::VariableAST *> newBody)
{
  body = newBody;
}



OpenSim::EulerAST::~EulerAST() {}


std::vector<OpenSim::VariableAST *> 
OpenSim::EulerAST::Body()
{
  return body;
}


double
OpenSim::EulerAST::Codegen(OpenSim::ASTConsumer *owner)
{
  return owner->visit(this);
}

