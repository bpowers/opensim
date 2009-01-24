#===--- ast.py - AST data structures -------------------------------------===#
#
# Copyright 2008 Bobby Powers
#
# This file is part of OpenSim.
# 
# OpenSim is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# OpenSim is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with OpenSim.  If not, see <http://www.gnu.org/licenses/>.
#
#===----------------------------------------------------------------------===#
#
# This file contains the data strutures and classes for building opensim
# abstract syntax trees
#
#===----------------------------------------------------------------------===#



class ASTNode:
  '''
  Base AST Node
  '''

  def __init__(self):
    parent = None
    attrs = []


class ASTList(ASTNode):
  '''
  AST Node representing a list of expressions
  '''

  def __init__(self, parent):
    self.parent = parent
    self.statements = []


class ASTScope(ASTNode):
  '''
  AST Node representing scope
  '''

  def __init__(self, parent, name):
    self.parent = parent
    self.name = name
    self.vars = {}
    self.child = None


class ASTAssignExpr(ASTNode):
  '''
  AST Node representing an assignment expression
  '''

  def __init(self, var_name, value):
    self.parent = None
    self.var_name = var_name
    self.value = value


class ASTBinExpr(ASTNode):
  '''
  AST Node represnting a binary equation
  '''

  def __init__(self, op, lvalue, rvalue):
    self.parent = None
    self.op = op
    self.lvalue = lvalue
    self.rvalue = rvalue


class ASTUnaryExpr(ASTNode):
  '''
  AST Node representing a unary operation
  '''

  def __init__(self, op, lvalue):
    self.parent = None
    self.op = op
    self.lvalue = lvalue


class ASTVarRef(ASTNode):
  '''
  AST Node representing a variable reference
  '''

  def __init__(self, name, var=None):
    self.parent = None
    self.name = name
    # var is optional, but probably a slight optimization from
    # recursively searching through scopes.  What are the downsides?
    self.var = var


class ASTValue(ASTNode):
  '''
  AST Node representing a numeric constant
  '''
  
  def __init__(self, val, kind=float):
    self.parent = None
    self.val = val
    self.type = kind


class ASTLookup(ASTNode):
  '''
  AST Node representing a lookup table
  '''

  def __init__(self, table):
    self.parent = None
    self.table = table


class ASTEuler(ASTNode):
  '''
  AST Node representing a Euler intergration loop
  '''

  def __init__(self, parent, body=None, stocks=None):
    self.parent = parent
    self.body = body
    self.stocks = stocks


class ASTCall(ASTNode):
  '''
  AST Node representing a function call
  '''

  def __init__(self, name, args=[]):
    self.parent = None
    self.name = name
    self.args = args

