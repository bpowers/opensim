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


import logging
import ast
import lex
import constants as sim

log = logging.getLogger('opensim.parse')

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

  def __init__(self, vars, var_list):

    self.__ast = None
    self.__vars = None
    self.__var_list = None
    self.__errors = []
    self.__vars = vars
    self.__var_list = var_list


  def __initialize(self):

    log.debug('initializing AST')

    self.__top_level_vars = list(self.__var_list)
    self.__ast = ast.ASTScope(None, 'root')
    self.__ast.vars = self.__vars
    self.__ast.child = ast.ASTList(self.__ast)
    self.__ast_initial = ast.ASTList(self.__ast.child)
    self.__ast_loop = ast.ASTEuler(self.__ast.child)
    self.__ast.child.statements = [self.__ast_initial, self.__ast_loop]

    while len(self.__top_level_vars) > 0:
      var = self.__top_level_vars.pop()
      #self._process_var(var)

    if len(self.__errors) > 0:
      log.error('the model has %d errors' % len(self.__errors))


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



class Parser:
  '''
  Class to parse variable expressions and create well formed ASTs.
  '''

  def __init__(self, var):
    '''
    Initialize a parser for a specific variable and it's equation.
    '''
    self.__var = var
    self.__scanner = lex.Scanner(var)
    self.__cur_tok = None
    self.__ast = None

    self.kind = sim.UNDEF

    self._get_next_tok()
    self._parse_primary()


  def _get_next_tok(self):
    self.__cur_tok = self.__scanner.get_tok()


  def _get_tok_precedence(self):
    if self.__cur_tok.kind is not lex.OPERATOR:
      return -1

    return _precedence[tok.iden]


  def _parse_primary(self):
    if self.__cur_tok is None:
      return

    if self.__cur_tok.kind is lex.INTEG:
      self._parse_integ()
    elif self.__cur_tok.iden == '[':
      self._parse_lookup()


  def _parse_lookup(self):
    '''
    Handle the equation as a lookup table.

    We're going to do this through python's built in 'eval' function,
    since our syntax for declaring lookup tables is equivolent to the
    Python for declaring lists of two-tuples.
    '''
    try:
      self.table = eval(self.__var.props.equation)
      assert isinstance(self.table, list)
    except:
      log.error('lookup table was incorrectly formatted:\n%s' %
                self.__var.props.equation)
    else:
      self.kind = sim.LOOKUP

