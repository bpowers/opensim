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
int_t = Type.int(32)


class JIT:
  '''
  This class implements the visitor methods needed to pretty print a model.

  fd is the file decriptor that we will write to, or any object that
  supports the write(str) method.
  '''
  def __init__(self, fd=sys.stdout, real_t=Type.double()):
    self.space = ''
    self.fd = fd
    self.real_t = real_t

  def _create_alloca(self, var_name, kind=None):
    '''
    Create entry block alloca instance.
    '''
    entry = self.fn_sim.entry_basic_block
    builder = Builder.new(entry)
    builder.position_at_beginning(entry)
    if not kind:
      kind = self.real_t
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

      self.__create_init_functions(node)

      bb = self.fn_sim.append_basic_block('entry')
      self.bb_begin = self.fn_sim.append_basic_block('begin')
      builder = Builder.new(bb)
      builder.branch(self.bb_begin)

      self.bb_end = self.fn_sim.append_basic_block('end')
      builder = Builder.new(self.bb_begin)
      builder.branch(self.bb_end)
      builder = Builder.new(self.bb_end)
      builder.ret(Constant.int(int_t, 0))

    node.child.gen(self)

    print self.module
    #mp = ModuleProvider.new(self.module)
    #ee = ExecutionEngine.new(mp)
    #sim = ee.run_function(self.fn_new, [])
    #ee.run_function(self.fn_sim, [sim])


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
    self.builder = Builder.new(self.bb_begin)

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
    return self.builder.node.val

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


  def __create_init_functions(self, root):
    '''
    Create the IR for several initialization functions.
    '''
    # we get circular import errors if we import this earlier.
    import opensim.engine as opensim

    # we need to probe to find out how many constants (and stock
    # initializations) we have, as well as how many stocks, flows
    # and auxilliary variables we have.  we need this to figure out
    # the size of our data structures for the simulation.  see
    # doc/jit.txt for more specific information

    # first find and record the variables in the main integration loop
    loop = root.child.statements[1]
    data_format = [int_t, self.real_t]
    data_vars = ['time']
    # keep track of the index in the struct a variable is stored at-
    # we start i at 2 becuase the 0th indexed element is size, 1 is
    # time, so we start after that.
    data_idx = {'time': 1}; i = 2
    for stmt in loop.body.statements: 
      # we create temporary variables for certain things, and we don't
      # want to keep track of them in the long term.
      if isinstance(self.vars[stmt.var_name], opensim.Variable):
        data_vars.append(stmt.var_name)
        data_format.append(self.real_t)
        data_idx[stmt.var_name] = i
        i += 1
    for stmt in loop.stocks.statements:
      data_vars.append(stmt.var_name)
      data_format.append(self.real_t)
      data_idx[stmt.var_name] = i
      i += 1

    # now record this stuff
    self.data = data_idx
    self.data_vars = data_vars
    # and define the llvm struct types that we're dealing with
    self.data_t = Type.struct(data_format)
    self.data_pt = Type.pointer(self.data_t)
    # FIXME: the name should be 'data_' + self.name, but for now
    # its just sim, because it makes for more readable IR
    self.module.add_type_name('data', self.data_t)

    # next go through and do the same for our constants.
    init = root.child.statements[0]
    const_format = [int_t, self.data_pt, self.data_pt]
    const_vars = []
    # initialize i to 3 becuase the first element is the size, second
    # element is data *curr, third is data *next
    const_idx = {}; i = 3
    for stmt in init.statements:
      const_vars.append(stmt.var_name)
      const_format.append(self.real_t)
      const_idx[stmt.var_name] = i
      i += 1

    # record these in instance variables as well
    self.sim = const_idx
    self.sim_vars = const_vars
    # and again define the corresponding llvm types
    self.sim_t = Type.struct(const_format)
    self.sim_pt = Type.pointer(self.sim_t)
    # FIXME: the name should be 'sim_' + self.name, but for now
    # its just sim, because it makes for more readable IR
    self.module.add_type_name('sim', self.sim_t)

    # now that we know the kind of structs we're dealing with, we can
    # create the init and new functions to work with these.

    # we have to declare the init function before the new function
    # because we call into it.
    init_fn_t = Type.function(int_t, [self.sim_pt])
    self.fn_init = Function.new(self.module, init_fn_t, self.name + '_init')

    # the new function allocates space on the heap for the data object,
    # calls init() on the struct (which malloc's space for two sim 
    # structs and initializes constant values), and returns the initialized
    # structure.
    new_fn_t = Type.function(self.sim_pt, [])
    self.fn_new = Function.new(self.module, new_fn_t, self.name + '_new')
    bb = self.fn_new.append_basic_block('entry')
    builder = Builder.new(bb)
    sim = builder.malloc(self.sim_t, 'sim')
    builder.call(self.fn_init, [sim])
    builder.ret(sim)

    # now we're going to build the initialization function
    bb = self.fn_init.append_basic_block('entry')
    builder = Builder.new(bb)
    sim = self.fn_init.args[0]
    # we need to allocate memory for the next and curr structs, and store
    # a pointer to them in the const data (sim) struct.
    curr = builder.malloc(self.data_t, 'curr')
    sim_curr_p = builder.gep(sim, [Constant.int(int_t, 0), Constant.int(int_t, 1)])
    builder.store(curr, sim_curr_p)
    next = builder.malloc(self.data_t, 'next')
    sim_next_p = builder.gep(sim, [Constant.int(int_t, 0), Constant.int(int_t, 2)])
    builder.store(next, sim_next_p)

    # finally return 0 for no error
    builder.ret(Constant.int(int_t, 0))

    # declare, but don't define, our simulute function as well
    sim_fn_t = Type.function(int_t, [self.sim_pt])
    self.fn_sim = Function.new(self.module, sim_fn_t, self.name + '_simulate')

