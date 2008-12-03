#===--- simulator.py - OpenSim simulation engine -------------------------===#
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
# This file contains the pure python implementation of a system dynamics 
# simulator.
#
#===----------------------------------------------------------------------===#

import pygtk
pygtk.require("2.0")

import gobject
from gettext import gettext as _
import logging as log

# opensim variables...
from variable import Variable
from constants import *

class Simulator(gobject.GObject):

  __gproperties__ = {
    'model_name' :       (gobject.TYPE_STRING,                   # object type
                          _('model name'),                       # nickname
                          _('the common name of the simulator'), # description
                          _('unnamed model'),                    # default
                          gobject.PARAM_READWRITE),              # flags
    'file_name' :        (gobject.TYPE_STRING,
                          _('file name'),
                          _('the path to the model file'),
                          None,
                          gobject.PARAM_READWRITE),
    'output_type' :      (gobject.TYPE_INT,
                          _('output type'),
                          _('type of output we\'re interested in'),
                          1,
                          5,
                          4,
                          gobject.PARAM_READWRITE),
    'output_file_name' : (gobject.TYPE_STRING,
                          _('output file name'),
                          _('the path to the output file'),
                          None,
                          gobject.PARAM_READWRITE),
    'valid_model' :      (gobject.TYPE_BOOLEAN,
                          _('valid model'),
                          _('True if model can be simulated as is'),
                          False,
                          gobject.PARAM_READWRITE)
  }

  __gsignals__ = {
    'saving' :           (gobject.SIGNAL_RUN_FIRST,
                          gobject.TYPE_NONE,
                          (gobject.TYPE_OBJECT,))
  }

  __vars = {}
  __vars_list = []



  def __init__(self, model_name=None, **kwargs):
    gobject.GObject.__init__(self, **kwargs)

    if model_name:
      self.model_name = model_name

    log.debug('created new simulator "%s"' % self.model_name)


  def run(self):
    log.debug('run stub')


  def load(self, model_path):
    log.debug('load stub')


  def save(self, save_path=None):
    log.debug('save stub')


  def new_variable(self, var_name, var_eqn=None):
    '''
    Creates a new variable as part of the current model. This is the
    only safe way to create a new variable; they should not be created
    on their own and 'added' to the model somehow.
    '''

    # validate input; it doesn't make sense to have an unnamed variable
    if not var_name or var_name == '':
      log.error('variables need a name at least')
      raise ValueError

    # make sure it is actually new
    if self.__vars.has_key(var_name):
      log.error('new variable already exists')
      return None

    new_var = Variable(self, var_name, var_eqn)

    if not new_var:
      log.error('couldn\'t create new variable')
      return None

    # keep track of the new variable
    self.__vars[var_name] = new_var
    self.__vars_list.append(new_var)

    log.debug('added new variable "%s" (%s) to simulation' %
              (new_var.name, new_var.equation))

    return new_var


  def get_variable(self, var_name):
    '''
    Get a reference to a variable from the current model.
    '''

    if not self.__vars.has_key(var_name):
      log.error('tried to get non-existant variable "%s"' % var_name)
      return None

    return self.__vars[var_name]


  def get_variables(self):
    '''
    Get a list of all of the variables in the model.
    '''

    # return a copy, so they can mutate it or do whatever
    return list(self.__vars_list)


  def remove_variable(self, var_name):
    '''
    Remove a variable from the model, returning True if successful,
    false if not.
    '''

    if not self.__vars.contains_key(var_name):
      log.error('tried to get non-existant variable "%s"' % var_name)
      return False

    # keep track of the variable so that after we remove it from
    # the dictionary we can remove it from the list and clear its
    # reference to the simulator
    var = self.__vars[var_name]

    del self.__vars[var_name]
    self.__vars_list.remove(var)
    var.parent = None



gobject.type_register(Simulator)

