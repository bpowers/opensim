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

import gobject
import gettext as _
import logging as log


# constants for the type of output we want
emit_ir      = 1
emit_python  = 2
emit_fortran = 3
emit_output  = 4
emit_as3     = 5


class Simulator(gobject.GObject):

  __gproperties__ = {
    'model_name' :       (gobject.TYPE_STRING,                  # object type
                          _('model name'),                      # nickname
                          _('the common name of the simulator') # description
                          None,                                 # minimum
                          None,                                 # maximum
                          _('unnamed model'),                   # default
                          gobject.PARAM_READWRITE)              # flags
    'file_name' :        (gobject.TYPE_STRING,
                          _('file name'),
                          _('the path to the model file')
                          None,
                          None,
                          None,
                          gobject.PARAM_READWRITE)
    'output_type' :      (gobject.TYPE_INT,
                          _('output type'),
                          _('type of output we\'re interested in')
                          1,
                          5,
                          4,
                          gobject.PARAM_READWRITE)
    'output_file_name' : (gobject.TYPE_STRING,
                          _('output file name'),
                          _('the path to the output file')
                          None,
                          None,
                          None,
                          gobject.PARAM_READWRITE)
    'valid_model' :      (gobject.TYPE_BOOLEAN,
                          _('valid model'),
                          _('True if model can be simulated as is')
                          None,
                          None,
                          False,
                          gobject.PARAM_READWRITE)
  }


  __init__(self, **kwargs):
    gobject.GObject.__init__(self, **kwargs)
    log.debug('created new simulator')


  def run(self):
    log.debug('run stub')


  def load(self, model_path):
    log.debug('load stub')


  def save(self, save_path=None):
    log.debug('save stub')


  def new_variable(self, var_name, var_eqn=None):
    log.debug('new_variable stub')


  def get_variable(self, var_name):
    log.debug('get_variable stub')


  def get_variables(self):
    log.debug('get_variables stub')


  def remove_variable(self, var_name):
    log.debug('remove_variable')



gobject.type_register(Simulator)

