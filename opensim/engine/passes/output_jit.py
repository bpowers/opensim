#===--- output_jit.py - jit-compile an opensim model ---------------------===#
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
# This file contains the class to JIT a model AST as Python.
#
#===-----------------------------------------------------------------------===#

import logging
import sys

from llvm import *
from llvm.core import *
from llvm.ee import *

import common

log = logging.getLogger('opensim.jit')

CLASS_NAME = 'JIT'

py_header = """
# simple lookup table implementation
def lookup(table, index):
  '''
  Simple lookup table implementation.

  Table takes the format of a list of 2-tuples.
  '''

  if len(table) is 0: return 0

  # if the request is outside the min or max, then we return
  # the nearest element of the array
  if   index < table[0][0]:  return table[0][1]
  elif index > table[-1][0]: return table[-1][1]

  for i in range(0, len(table)):
    x, y = table[i]

    if index == x: return y
    if index < x:
      # slope = deltaY/deltaX
      slope = (y - table[i-1][1])/(x - table[i-1][0])
      return (index-table[i-1][0])*slope + table[i-1][1]
"""

# pertinent llvm types
void_t = Type.void()
bool_t = Type.int(1)
real_t = Type.double()
func_t = Type.function(void_t, [])


class JIT:
  '''
  This class implements the visitor methods needed to pretty print a model.

  fd is the file decriptor that we will write to, or any object that
  supports the write(str) method.
  '''
  def __init__(self, fd=sys.stdout):
    self.space = ''
    self.fd = fd

  def _create_alloca(self, var_name, kind=real_t):
    '''
    Create entry block alloca instance.
    '''
    entry = self.fn_sim.entry_basic_block
    builder = Builder.new(entry)
    builder.position_at_beginning(entry)
    return builder.alloca(kind, var_name)


  def visit_scope(self, node):
    '''
    Visiting a scope node.

    Eventually we will use this to implement namespaces and have stacks
    of variables to lookup in, but for now we just grab the vars from
    the root scope.
    '''
    if node.name == 'root':
      self.name = node.sim.props.model_name.replace(' ', '_')
      self.vars = node.vars
      self.tables = node.tables

      self.module = Module.new(self.name + '_jit')
      self.fn_sim = Function.new(self.module, func_t, self.name + '_simulate')

      self.__create_init_functions(node)

      bb = self.fn_sim.append_basic_block('entry')
      self.bb_begin = self.fn_sim.append_basic_block('begin')
      builder = Builder.new(bb)
      builder.branch(self.bb_begin)

    node.child.gen(self)

    print self.module


  def __create_init_functions(self, root):
    '''
    Create the IR for several initialization functions.
    '''
    import opensim.engine as opensim
    # define our data structures first
    node = root.child.statements[1]

    sim_format = [real_t]
    sim_vars = ['time']
    for stmt in node.body.statements:
      if isinstance(self.vars[stmt.var_name], opensim.Variable):
        sim_vars.append(stmt.var_name)
        sim_format.append(real_t)
    for stmt in node.stocks.statements:
      sim_vars.append(stmt.var_name)
      sim_format.append(real_t)

    self.sim_data_vars = sim_vars
    self.sim_data_t = Type.struct(sim_format)
    self.sim_data_pt = Type.pointer(self.sim_data_t)

    node = root.child.statements[0]

    const_format = [self.sim_data_pt, self.sim_data_pt]
    const_vars = []
    for stmt in node.statements:
      const_vars.append(stmt.var_name)
      const_format.append(real_t)

    self.const_data_vars = const_vars
    self.data_t = Type.struct(const_format)
    self.data_pt = Type.pointer(self.data_t)

    init_fn_t = Type.function(void_t, [self.data_pt])
    self.fn_init = Function.new(self.module, init_fn_t, self.name + '_init')

    new_fn_t = Type.function(self.data_pt, [])
    self.fn_new = Function.new(self.module, new_fn_t, self.name + '_new')

    bb = self.fn_new.append_basic_block('entry')
    builder = Builder.new(bb)
    #curr = builder.malloc(self.sim_data_t, 'curr')
    #next = builder.malloc(self.sim_data_t, 'next')
    sim = builder.malloc(self.data_t, 'sim')
    builder.call(self.fn_init, [sim])
    builder.ret(sim)


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
    builder = Builder.new(self.bb_begin)

    #self.write('\n# variables related to outputting results')
    self._create_alloca('save_count')
    self._create_alloca('save_iterations')
    self._create_alloca('do_save', kind=bool_t)

    #self.write('save_count = 0')
    #self.write('save_iterations = time_savestep / time_step')
    #self.write('do_save = True')

    #self.write('\n# lookup tables:')
    #for v in self.tables:
      #self.write('%s = %s' % (v.props.name, v.table))

    format = '%f'
    vars_list = 'time'
    for stmt in node.body.statements:
      vars_list += ',' + stmt.var_name
      format += ',%f'
    for stmt in node.stocks.statements:
      vars_list += ',' + stmt.var_name
      format += ',%f'

    # output headers for csv output
    #self.write('\nprint \'%s\'' % vars_list)

    #self.write('\nfor time in frange(time_start, time_end, time_step):')

    # indent things!
    self.space = self.space + '  '
    #self.write('  # calculate flows:')
    ###node.body.gen(self)

    #self.write('\n  # output results:')
    #self.write('  if do_save:')
    #self.write("    print '" + format + "' % (" + vars_list + ')')

    #self.write('\n  # update stocks:')
    ###node.stocks.gen(self)

    # determine whether we need to write the next round of output to stdout
    #self.write('')
    #self.write('  # determining whether or not to save results next iteration')
    #self.write('  save_count += 1')
    #self.write('  if save_count >= save_iterations or time+time_step > time_end:')
    #self.write('    do_save = True')
    #self.write('    save_count = 0')
    #self.write('  else:')
    #self.write('    do_save = False')

    self.space = self.space[:-4]


  def visit_assign(self, node):
    '''
    Visiting an assignment statement.
    '''
    #self.write(self.space + node.var_name + ' = ', end='')
    #node.value.gen(self)
    #self.write('')


  def visit_bin_expr(self, node):
    '''
    Visit a node represeting a binary expression.

    We add parentheses becuase when we create the AST we respect
    parenthesis, but they are not represented in the tree
    '''
    self.write('(', end='')
    node.lval.gen(self)
    self.write(' %s ' % node.op, end='')
    node.rval.gen(self)
    self.write(')', end='')



  def visit_unary(self, node):
    self.write(node.op + '(', end='')
    node.lval.gen(self)
    self.write(')', end='')


  def visit_var_ref(self, node):
    '''
    Visit a variable reference.

    Leaf node!
    '''
    self.write(node.name, end='')


  def visit_value(self, node):
    '''
    Visit a node representing a real number.

    Leaf node!
    '''
    self.write(str(node.val), end='')

  def visit_call(self, node):
    '''
    Visit a node representing a function call.
    '''
    if node.name.lower() == 'max':
      if len(node.args) != 2:
        raise AttributeError, 'Incorrect # of args to max (%d).' % \
                              len(node.args)
      self.write('%s(' % node.name, end='')
      node.args[0].gen(self)
      self.write(', ', end='')
      node.args[1].gen(self)
      self.write(')', end='')
    else:
      raise ValueError, 'Unknown function call: %s' % node.name


  def visit_lookup(self, node):
    '''
    Visit a node representing a lookup reference.
    '''
    self.write('lookup(%s, ' % node.name, end='')
    node.arg.gen(self)
    self.write(')', end='')

