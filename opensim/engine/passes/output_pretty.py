#===--- output_pretty.py - pretty print an opensim model -----------------===#
#
# Copyright 2009 Bobby Powers
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
#===-----------------------------------------------------------------------===#
#
# This file contains the class to pretty print a model AST.
#
#===-----------------------------------------------------------------------===#

import logging

log = logging.getLogger('opensim.pretty')

CLASS_NAME = 'PrettyPrint'


class PrettyPrint:
  '''
  This class implements the visitor methods needed to pretty print a model.
  '''

  def visit_scope(self, node):
    '''
    Visiting a scope node.

    Eventually we will use this to implement namespaces and have stacks
    of variables to lookup in, but for now we just grab the vars from
    the root scope.
    '''
    if node.name == 'root':
      self.vars = node.vars

    node.child.gen(self)


  def visit_list(self, node):
    '''
    Visit a list of statements.
    '''
    for stmt in node.statements:
      stmt.gen(self)


  def visit_euler(self, node):
    '''
    Visit a euler integration node.

    This is basically a glorified loop, but is used to distinguish
    between a basic loop and a more complicated RK one.
    '''
    print('using euler integration\n')
    node.body.gen(self)

    node.stocks.gen(self)


  def visit_assign(self, node):
    '''
    Visiting an assignment statement.
    '''
    print node.var_name + ' = ',
    node.value.gen(self)
    print


  def visit_bin_expr(self, node):
    '''
    Visit a node represeting a binary expression.
    '''
    node.lval.gen(self)
    print node.op,
    node.rval.gen(self)


  def visit_var_ref(self, node):
    '''
    Visit a variable reference.

    Leaf node!
    '''
    print node.name,


  def visit_value(self, node):
    '''
    Visit a node representing a real number.

    Leaf node!
    '''
    print node.val,

