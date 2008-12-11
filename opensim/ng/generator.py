#===--- generator.py - Control for AST manipulation ----------------------===#
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
# This file contains the generator class, which manages all AST operations.
#
#===----------------------------------------------------------------------===#


import logging as log
import ast

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

    self._top_level_vars = list(self.__var_list)
    self.__ast_initial = ast.ASTList()
    self.__ast_loop = ast.ASTEuler()
    self.__ast = ast.ASTScope()
    self.__ast.vars = self.__vars
    self.__ast.child = ast.ASTList()
    self.__ast.child.statements = [self.__ast_initial, self.__ast_loop]

    while len(self._top_level_vars) > 0:
      var = self._top_level_vars.pop()
      self._process_var(self, var)

    if len(self.__errors) > 0:
      logging.error('the model has %d errors' % len(self.__errors))


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
