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


  def append(self, item):
    '''
    Convience method for adding an item to a list.
    '''
    self.statements.append(item)


  def gen(self, visitor):
    return visitor.visit_list(self)


class ASTScope(ASTNode):
  '''
  AST Node representing scope
  '''

  def __init__(self, parent, name):
    self.parent = parent
    self.name = name
    self.vars = {}
    self.child = None

  def gen(self, visitor):
    return visitor.visit_scope(self)


class ASTAssignExpr(ASTNode):
  '''
  AST Node representing an assignment expression
  '''

  def __init__(self, var_name, value):
    self.parent = None
    self.var_name = var_name
    self.value = value

  def gen(self, visitor):
    return visitor.visit_assign(self)


class ASTBinExpr(ASTNode):
  '''
  AST Node represnting a binary equation
  '''

  def __init__(self, op, lvalue, rvalue):
    self.parent = None
    self.op = op
    self.lval = lvalue
    self.rval = rvalue

  def gen(self, visitor):
    return visitor.visit_bin_expr(self)


class ASTUnaryExpr(ASTNode):
  '''
  AST Node representing a unary operation
  '''

  def __init__(self, op, lvalue):
    self.parent = None
    self.op = op
    self.lval = lvalue

  def gen(self, visitor):
    return visitor.visit_unary(self)


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

  def gen(self, visitor):
    return visitor.visit_var_ref(self)


class ASTValue(ASTNode):
  '''
  AST Node representing a numeric constant
  '''
  
  def __init__(self, val, kind=float):
    self.parent = None
    self.val = val
    self.type = kind

  def gen(self, visitor):
    return visitor.visit_value(self)


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
    if body:
      self.body = body
    else:
      self.body = ASTList(self)

    if stocks:
      self.stocks = stocks
    else:
      self.stocks = ASTList(self)

  def gen(self, visitor):
    return visitor.visit_euler(self)


class ASTCallExpr(ASTNode):
  '''
  AST Node representing a function call
  '''

  def __init__(self, name, args=[]):
    self.parent = None
    self.name = name
    self.args = args

  def gen(self, visitor):
    return visitor.visit_call(self)


class ASTLookupRef(ASTNode):
  '''
  AST Node representing a lookup
  '''

  def __init__(self, name, arg):
    self.parent = None
    self.name = name
    self.arg = arg

  def gen(self, visitor):
    return visitor.visit_lookup(self)
