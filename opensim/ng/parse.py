#===--- parse.py - Parser for SD equations -------------------------------===#
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
# This file contains functions for parsing equations and creating ASTs.
#
#===----------------------------------------------------------------------===#


import logging as log
import ast
import lex
from constants import *

_precedence = {'=': 2,
               '<': 10,
               '>': 10,
               '+': 20,
               '-': 20,
               '*': 40,
               '/': 40,
               '^': 60,
              }


class Generator:

  __ast = None
  __vars = None
  __var_list = None
  __errors = []

  def __init__(self, vars, var_list):
    self.__vars = vars
    self.__var_list = var_list


  def __initialize(self):

    log.debug('initializing AST')

    self.__cur_tok = None
    self._top_level_vars = list(self.__var_list)
    self.__ast = ast.ASTScope(None, 'root')
    self.__ast.vars = self.__vars
    self.__ast.child = ast.ASTList(self.__ast)
    self.__ast_initial = ast.ASTList(self.__ast.child)
    self.__ast_loop = ast.ASTEuler(self.__ast.child)
    self.__ast.child.statements = [self.__ast_initial, self.__ast_loop]

    while len(self._top_level_vars) > 0:
      var = self._top_level_vars.pop()
      self._process_var(var)

    if len(self.__errors) > 0:
      log.error('the model has %d errors' % len(self.__errors))


  def _get_tok_precedence(self):
    if not self.__cur_tok[0] is lex.OPERATOR:
      return -1

    return _precedence[cur_tok[1]]


  def _get_next_tok(self):
    if len(self.__cur_toks) <= self.__toks_index:
      self.__cur_tok = None
      log.debug('end of tok stream (%d/%d)' %
                (len(self.__cur_toks), self.__toks_index))
      return False

    self.__cur_tok = self.__cur_toks[self.__toks_index]
    self.__toks_index += 1
    return True


  def _push_tokens(self):
    self.__index_stack.append(self.__toks_index-1)
    self.__var_stack.append(self.__cur_var)


  def _pop_tokens(self):
    self.__toks_index = self.__index_stack.pop()
    self.__cur_var = self.__var_stack.pop()

    self._get_next_tok()


  def _process_var(self, var):
    self.__toks_index = 0
    self.__cur_var = var
    self.__cur_toks = var._get_tokens()

    if len(self.__cur_toks) is 0:
      return

    # prime cur_tok
    self._get_next_tok()

    trees = self._parse_equation()


  def _parse_equation(self):
    if self.__cur_var.props.type is STOCK:
      return self._parse_stock()
    elif self.__cur_var.props.type is CONST:
      return (('initial', self._parse_expression()))
    else:
      return (('flows', self._parse_expression()))


  def _parse_expression(self):
    log.debug('parsing expression')


  def _parse_stock(self):
    # we know the first token is an INTEG, so eat it.
    self._get_next_tok()
    initial, change = self._get_args(2)
    log.debug('parsing stock')


  def _get_args(self, num_args=-1):
    if self.__cur_tok.iden != '(':
      raise AttributeError

    # eat '('
    self._get_next_tok()

    # now we need to split at the ',' token and parse the
    # two arguments, as they are not necessarily simple constants

    return (None,None)

  def update(self, var):
    '''
    Update the AST, as an equation changed or variable was added.
    '''

    # this can totally be optimized in the future
    self.__initialize()


  def rebase(self, vars, var_list):
    '''
    Replace the current AST with one based on a new set of variables.
    '''

    self.__vars = vars
    self.__var_list = var_list

    self.__initialize()
