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
from parse import Generator
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


  def __init__(self, model_name=None, file_name=None, **kwargs):
    gobject.GObject.__init__(self, **kwargs)

    self.__vars = {}
    self.__vars_list = []
    self.__vars_invalid = []
    self.__model = None
    self.__generator = None

    self.__model_name = ''
    self.__file_name = ''
    self.__output_type = 4
    self.__output_file_name = ''

    if model_name:
      self.props.model_name = model_name

    if file_name:
      self.load(file_name)
    else:
      self.__initialize_time()

    self.__generator = Generator(self.__vars, self.__vars_list)


  def __initialize_time(self):
    '''
    If this model isn't being loaded from a file, we should
    initialize the time constants to sane defaults, as
    specified in the constants file.  We return true if it
    was a success, and false if there are already existing
    variables.
    '''

    if len(self.__vars_list) is not 0:
      raise StandardError, 'we can only initialize time on new models'
      return False

    self.new_variable('time start', INITIAL_TIME_START)
    self.new_variable('time end', INITIAL_TIME_END)
    self.new_variable('time step', INITIAL_TIME_STEP)
    self.new_variable('time savestep', INITIAL_TIME_SAVESTEP)
    self.new_variable('time')

    return True


  def __clean_model(self):
    '''
    When we load models, we want to be sure we clean the existing
    variables and structure out, so that we're starting pristine.
    '''

    var_dict = self.__vars
    var_list = self.__vars_list

    self.__vars = {}
    self.__vars_list = []

    del var_dict
    del var_list


  def do_get_property(self, prop):
    '''
    standart gobject getter.
    '''

    if prop.name == 'model-name':
      return self.__model_name
    elif prop.name == 'file-name':
      return self.__file_name
    elif prop.name == 'output-type':
      return self.__output_type
    elif prop.name == 'output-file-name':
      return self.__output_file_name
    elif prop.name == 'valid-model':
      if not self.__model:
        self.__update_model()
      if len(self.__vars_invalid) is 0:
        return True
      else:
        return False
    else:
      raise AttributeError('unknown prop: "%s"' % prop.name)


  def do_set_property(self, prop,value):
    '''
    standart gobject setter.
    '''

    if not value:
      value = ''

    if prop.name == 'model-name':
      if type(value) is not str:
        raise AttributeError('model name is a string, not %s' % type(value))
      self.__model_name = value

    elif prop.name == 'file-name':
      if type(value) is not str:
        raise AttributeError('file name is a string, not %s' % type(value))
      self.__file_name = value

    elif prop.name == 'output-type':
      if type(value) is not int:
        raise AttributeError('output type is an int, not %s' % type(value))
      elif value < EMIT_RANGE_MIN or value > EMIT_RANGE_MAX:
        raise AttributeError('output type out of range with %d' % value)
      self.__output_type = value

    elif prop.name == 'output-file-name':
      if type(value) is not str:
        raise AttributeError('output file name is a string, not %s' % 
                             type(value))
      self.__output_file_name = value

    else:
      raise AttributeError('unknown prop: "%s" ("%s")' % (prop.name, value))


  def __update_model(self, var=None):

    # we won't have a generator when we're initializing a new
    # simulator object
    if self.__generator:
      self.__generator.update(var)

    log.debug('update module stub')


  def run(self):
    '''
    Run a model to give the desired type of output.

    With an output type of EMIT_OUTPUT, this will simulate the model.
    This will also transform the model into python or some such with
    the right output type (and corresponding backend).
    '''

    if not self.props.valid_model:
      log.error('cannot run an invalid model')
      return -1

    if self.__output_type is EMIT_OUTPUT:
      pass
    else:
      raise NotImplementedError('The output type (%d) is not supported.' %
                                self.__output_type)


  def load(self, model_path=None):
    self.props.file_name = file_name
    log.debug('load stub')


  def save(self, save_path=None):
    log.debug('save stub')


  def new_variable(self, var_name, var_eqn=None):
    '''
    Creates a new variable as part of the current model. 
    
    This is the only safe way to create a new variable; they should not
    be created on their own and 'added' to the model somehow.
    '''

    # validate input; it doesn't make sense to have an unnamed variable
    if not var_name or var_name == '':
      raise AttributeError('variables need a name at least')

    # make sure it is actually new
    if self.__vars.has_key(var_name):
      log.error('variable \'%s\' already exists' % var_name)
      return None

    new_var = Variable(self, var_name, var_eqn)

    if not new_var:
      log.error('couldn\'t create new variable')
      return None

    # keep track of the new variable
    self.__vars[var_name] = new_var
    self.__vars_list.append(new_var)

    new_var.connect('equation_changed', self.variable_changed)

    if not new_var.props.valid:
      self.__vars_invalid.append(new_var)

    self.__update_model(new_var)

    return new_var


  def get_variable(self, var_name):
    '''
    Get a reference to a variable from the current model.
    '''

    var_name = var_name.replace('_', ' ')

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
    
    if self.__vars_invalid.count(var):
      self.__vars_invalid.remove(var)

    # delete it so we're sure its not holding on to a reference
    # to the simulator.  i think it pays to be explicit here.
    del var


  def variable_changed(self, var, old_equation):
    '''
    Callback function so that we know to update the model AST.
    '''

    # if we delete variables, I suppose it is possible this could happen
    if not self.__vars.has_key(var.props.name):
      log.error('crud!  getting callbacks from "nonexistant" vars (%s)' %
                var.props.name)

    was_invalid = self.__vars_invalid.count(var)
    if was_invalid:
      if var.props.valid:
        self.__vars_invalid.remove(var)
    else:
      if not var.props.valid:
        self.__vars_invalid.append(var)

    self.__update_model(var)


gobject.type_register(Simulator)

