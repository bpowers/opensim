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
  parent = None
  attrs = []


class ASTList(ASTNode):
  statements = []

  def __init__(self, parent):
    self.parent = parent


class ASTScope(ASTNode):
  name = ''
  vars = {}
  child = None

  def __init__(self, parent, name):
    self.parent = parent
    self.name = name


class ASTAssignExpr(ASTNode):
  var_name = None
  value = None

  def __init(self, parent, var_name, value):
    self.var_name = var_name
    self.value = value


class ASTBinExpr(ASTNode):
  op = ''
  lvalue = None
  rvalue = None

  def __init__(self, parent, op, lvalue, rvalue):
    self.parent = parent
    self.op = op
    self.lvalue = lvalue
    self.rvalue = rvalue


class ASTUnaryExpr(ASTNode):
  op = ''
  lvalue = None

  def __init__(self, parent, op, lvalue):
    self.parent = parent
    self.op = op
    self.lvalue = lvalue


class ASTIdentifier(ASTNode):
  name = ''
  var = None

  def __init__(self, parent, name, var=None):
    self.parent = parent
    self.name = name
    # var is optional, but probably a slight optimization from
    # recursively searching through scopes.  What are the downsides?
    self.var = var


class ASTValue(ASTNode):
  type = None
  val = 0
  
  def __init__(self, val, type=float):
    self.parent = parent
    self.val = val
    self.type = type


class ASTEuler(ASTNode):
  body = None
  stocks = None

  def __init__(self, parent, body=None, stocks=None):
    self.parent = parent
    self.body = body
    self.stocks = stocks


class ASTCall(ASTNode):
  name = ''
  args = []

  def __init__(self, parent, name, args=[]):
    self.parent = parent
    self.name = name
    self.args = args
