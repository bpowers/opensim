#===--- Simulator.py - OpenSim simulation engine -------------------------===#
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


# constants for the type of output we want
EMIT_IR      = 1
EMIT_PYTHON  = 2
EMIT_FORTRAN = 3
EMIT_OUTPUT  = 4
EMIT_AS3     = 5


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
    Creates a new variable as part of the current model.
    '''
    log.debug('new_variable stub')

    if not var_name:
      log.Error('variables need a name at least')
      raise ValueError

    new_var = Variable(self, var_name, var_eqn)
    self.__vars[var_name] = new_var

    log.debug('added new variable "%s"' % new_var.name)

    return new_var


  def get_variable(self, var_name):
    log.debug('get_variable stub')


  def get_variables(self):
    log.debug('get_variables stub')


  def remove_variable(self, var_name):
    log.debug('remove_variable')



gobject.type_register(Simulator)

