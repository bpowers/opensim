#===--- run.py - Classes and functions to manage running SD models -------===#
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
# This file contains classes and functions to build and rebuild ASTs,
# register passes and manage AST walks.
#
#===----------------------------------------------------------------------===#

import math, logging

from ast import *
import lex
import parse
import constants as sim

log = logging.getLogger('opensim.run')



class Manager:

  def __init__(self, variables, var_list):

    self.passes = []

    self.__ast = None
    self.__vars = None
    self.__var_list = None
    self.__errors = []
    self.__vars = variables
    self.__var_list = var_list

    self.__initialize()


  def __initialize(self):

    log.debug('initializing AST')

    self.__unparsed = list(self.__var_list)
    self.__in_progress = []
    self.__parsed = []
    self.__ast = ASTScope(None, 'root')
    self.__ast.tables = []
    self.__ast.vars = self.__vars
    self.__ast.child = ASTList(self.__ast)
    self.__ast_initial = ASTList(self.__ast.child)
    self.__ast_loop = ASTEuler(self.__ast.child)
    self.__ast.child.statements = [self.__ast_initial, self.__ast_loop]

    while len(self.__unparsed) > 0:
      var = self.__unparsed.pop()
      self._add(var)

    if len(self.__errors) > 0:
      log.error('the model has %d errors' % len(self.__errors))


  def _add(self, var):
    '''
    Add a variable to our model representation.

    Recursively deals with variables the current one depends on.
    '''
    self.__in_progress.append(var)
    parser = parse.Parser(var)
    parser.parse()

    if parser.valid:
      var._set_type(parser.kind)
      if parser.refs:
        for ref in parser.refs:
          ref = self.__vars[ref]
          if ref.props.type is sim.STOCK:
            continue
          if ref in self.__unparsed:
            self.__unparsed.remove(ref)
            self._add(ref)
          #elif ref in self.__in_progress:
          #  raise ValueError, 'circular ref (%s) for %s!' % (ref.props.name,
          #                    var.props.name)
      self.__parsed.append(var)

      if parser.kind is sim.LOOKUP:
        var.table = parser.table
        self.__ast.tables.append(var)
      elif parser.kind is sim.AUX or parser.kind is sim.FLOW:
        self.__ast_loop.body.append(parser.ast)
      elif parser.kind is sim.CONST:
        self.__ast_initial.append(parser.ast)
      else:
        nf_name = self._make_unique(var.props.name + '_net_flow')
        nf_ast = ASTAssignExpr(nf_name, parser.net_flow)
        nf_var = parse.TempVar(nf_name)
        up_stock_expr = self._create_stock_expr(var.props.name, nf_name)

        self.__vars[nf_name] = nf_var
        self.__ast_loop.body.append(nf_ast)
        self.__ast_initial.append(parser.initial)
        self.__ast_loop.stocks.append(up_stock_expr)

      # probably do some in_progress, unparsed, parsed
      # checking here
      self.__in_progress.remove(var)


  def _make_unique(self, name):
    '''
    Takes a name and makes sure it is unique.

    Used for netflows and function expansions to avoid namespace
    collisions.
    '''
    # TODO: actually implement this.
    return name


  def _create_stock_expr(self, var_name, nf_name):
    '''
    Create the AST structure representing an updating of a stock.

    has the form:
    stock = stock + net_flow * time_step
    '''
    mult = ASTBinExpr('*', ASTVarRef(nf_name), ASTVarRef('time_step'))
    add = ASTBinExpr('+', ASTVarRef(var_name), mult)
    return ASTAssignExpr(var_name, add)

  def update(self, var):
    '''
    Update the AST, as an equation changed or variable was added.
    '''

    # this can totally be optimized in the future
    self.__initialize()


  def rebase(self, variables, var_list):
    '''
    Replace the current AST with one based on a new set of variables.
    '''

    self.__vars = variables
    self.__var_list = var_list

    self.__initialize()


  def walk(self, output_walker):
    '''
    Produce some kind of output by walking the ast.
    '''
    self.__ast.gen(output_walker)

