#===--- output_interpret.py - interpret an opensim model -----------------===#
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
# This file contains the class to interpret a model AST.
#
#===-----------------------------------------------------------------------===#

import logging, sys, math
from common import *

log = logging.getLogger('opensim.interpret')

CLASS_NAME = 'Interpret'


class Interpret:
  '''
  This class implements the visitor methods needed to pretty print a model.

  fd is the file decriptor that we will write to, or any object that
  supports the write(str) method.
  '''
  def __init__(self, fd=sys.stdout):
    self.space = ''
    self.fd = fd


  def write(self, string, end='\n'):
    '''
    Writes a string to self.fd and appends a newline
    '''
    self.fd.write(string)
    self.fd.write(end)


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
    time_start = self.vars['time_start'].val
    time_end = self.vars['time_end'].val
    time_step = self.vars['time_step'].val
    time_savestep = self.vars['time_savestep'].val

    # variables related to outputting results
    save_count = 0
    save_iterations = time_savestep / time_step
    do_save = True

    vars_list = 'time'
    out_vars = []
    for stmt in node.body.statements:
      var = self.vars[stmt.var_name]
      vars_list += ',' + stmt.var_name
      out_vars.append(self.vars[stmt.var_name])
    for stmt in node.stocks.statements:
      vars_list += ',' + stmt.var_name
      out_vars.append(self.vars[stmt.var_name])

    # output headers for csv output
    self.write(vars_list)

    for time in frange(time_start, time_end, time_step):
      self.vars['time'].val = time

      # calculate flows
      node.body.gen(self)

      # output results:
      if do_save:
        self.write('%f' % self.vars['time'].val, end='')
        for v in out_vars:
          self.write(',%f' % v.val, end='')
        self.write('')

      # update stocks
      node.stocks.gen(self)

      # determining whether or not to save results next iteration
      save_count += 1
      if save_count >= save_iterations or time+time_step > time_end:
        do_save = True
        save_count = 0
      else:
        do_save = False

    self.space = self.space[:-4]


  def visit_assign(self, node):
    '''
    Visiting an assignment statement.
    '''
    self.vars[node.var_name].val = node.value.gen(self)


  def visit_bin_expr(self, node):
    '''
    Visit a node represeting a binary expression.

    We add parentheses becuase when we create the AST we respect
    parenthesis, but they are not represented in the tree
    '''
    lval = node.lval.gen(self)
    rval = node.rval.gen(self)

    if node.op == '+':
      return lval + rval
    elif node.op == '-':
      return lval - rval
    elif node.op == '/':
      return lval / rval
    elif node.op == '*':
      return lval * rval
    elif node.op == '^':
      return lval ** rval


  def visit_unary(self, node):
    '''
    Visit a unary (+ or -) node.
    '''
    if node.op == '-':
      return -node.lval.gen(self)
    elif node.op == '+':
      return node.lval.gen(self)


  def visit_var_ref(self, node):
    '''
    Visit a variable reference.

    Leaf node!
    '''
    return self.vars[node.name].val


  def visit_value(self, node):
    '''
    Visit a node representing a real number.

    Leaf node!
    '''
    return node.val

  def visit_call(self, node):
    '''
    Visit a node representing a function call.
    '''
    if node.name.lower() == 'max':
      if len(node.args) != 2:
        raise AttributeError, 'Incorrect # of args to max (%d).' % \
                              len(node.args)
      return max(node.args[0].gen(self), node.args[1].gen(self))
    else:
      raise ValueError, 'Unknown function call: %s' % node.name


  def visit_lookup(self, node):
    '''
    Visit a node representing a lookup reference.
    '''
    return lookup(self.vars[node.name].table, node.arg.gen(self))
