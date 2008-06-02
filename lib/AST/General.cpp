//===--- General.cpp - many general AST nodes ----------------------------===//
//
// Copyright 2008 Bobby Powers, large portions of General.h/cpp are 
//   Copyright Chris Lattner (LLVM Kaleidescope Tutorial)
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
// This file contains implementations for many general AST nodes 
// that are needed
//
//===---------------------------------------------------------------------===//

#include "General.h"
#include <cstdio>
#include "ASTConsumer.h"
using OpenSim::ASTConsumer;


double
OpenSim::NumberExprAST::Codegen(ASTConsumer *owner)
{
  return owner->visit(this);
}



double
OpenSim::BinaryExprAST::Codegen(ASTConsumer *owner) 
{
  return owner->visit(this);
}



double
OpenSim::UnaryExprAST::Codegen(ASTConsumer *owner) 
{
  return owner->visit(this);
}



double
OpenSim::VarRefAST::Codegen(ASTConsumer *owner)
{
  return owner->visit(this);
}



double
OpenSim::LookupRefAST::Codegen(ASTConsumer *owner)
{
  return owner->visit(this);
}



double
OpenSim::FunctionRefAST::Codegen(ASTConsumer *owner)
{
  return owner->visit(this);
}
