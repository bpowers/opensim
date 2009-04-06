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
import logging

from constants import *
from variable import Variable
import run
import io
import passes

log = logging.getLogger('opensim.sim')



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
    'output_type' :      (gobject.TYPE_STRING,
                          _('output type'),
                          _('type of output we\'re interested in'),
                          'interpret',
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
                          (gobject.TYPE_PYOBJECT,))
  }


  def __init__(self, model_name=None, file_name=None, **kwargs):
    gobject.GObject.__init__(self, **kwargs)

    self.__vars = {}
    self.__vars_list = []
    self.__vars_invalid = []
    self.__model = None
    self.__manager = None

    self.__model_name = ''
    self.__file_name = ''
    self.__output_type = 'interpret'
    self.__output_file_name = ''

    # io handler
    self.__handler = None

    # keep track of whether we should be incrementally updating
    # the model's AST representation upon changes.
    self.__incremental = True

    if model_name:
      self.props.model_name = model_name

    # we either want to load a model from disk, or make sure we
    # initialize time for a blank model.
    if file_name:
      self.load(file_name)
    else:
      self.__initialize_time()

    self.__manager = run.Manager(self.__vars, self.__vars_list, self)


  def _disable_incremental(self):
    '''
    Disable incremental model compilation.
    '''
    self.__incremental = False


  def _enable_incremental(self):
    '''
    Enable incremental model compilation.
    '''
    self.__incremental = True
    self.__update_model()


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

    self.new_var('time start', INITIAL_TIME_START)
    self.new_var('time end', INITIAL_TIME_END)
    self.new_var('time step', INITIAL_TIME_STEP)
    self.new_var('time savestep', INITIAL_TIME_SAVESTEP)
    self.new_var('time')

    return True


  def __update_model(self, var=None):

    # we won't have a generator when we're initializing a new
    # simulator object
    if self.__manager and self.__incremental:
      self.__manager.update(var)


  def __clean_model(self):
    '''
    When we load models, we want to be sure we clean the existing
    variables and structure out, so that we're starting pristine.
    '''

    self.__vars = {}
    self.__vars_list = []
    self.__manager = run.Manager(self.__vars, self.__vars_list, self)


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


  def do_set_property(self, prop, value):
    '''
    standard gobject setter.
    '''

    if not value:
      value = ''

    if prop.name == 'model-name':
      if not isinstance(value, str):
        raise AttributeError('model name is a string, not %s' % type(value))
      self.__model_name = value

    elif prop.name == 'file-name':
      if not isinstance(value, str):
        raise AttributeError('file name is a string, not %s' % type(value))
      self.__file_name = value

    elif prop.name == 'output-type':
      if not isinstance(value, str):
        raise AttributeError('output type is a string, not %s' % type(value))
      # TODO: move the check for a valid output type from run-time to here
      self.__output_type = value

    elif prop.name == 'output-file-name':
      if not isinstance(value, str):
        raise AttributeError('output file name is a string, not %s' % 
                             type(value))
      self.__output_file_name = value

    else:
      raise AttributeError('unknown prop: "%s" ("%s")' % (prop.name, value))


  def run(self):
    '''
    Run a model to give the desired type of output.

    With an output type of EMIT_OUTPUT, this will simulate the model.
    This will also transform the model into python or some such with
    the right output type (and corresponding backend).
    '''

    #if not self.props.valid_model:
    #  log.error('cannot run an invalid model')
    #  return -1

    walker = passes.get_output_pass(self.__output_type)
    if not walker:
      raise NotImplementedError('The output type (%s) is not supported.' %
                                self.__output_type)
    else:
      self.__manager.walk(walker)


  def load(self, model_path=None):
    '''
    Load a model from the specified path, or the file_name property.
    '''
    if model_path:
      self.props.file_name = model_path
    path = self.props.file_name
    self.__handler = io.get_handler(path)
    if not self.__handler:
      raise TypeError, "unable to load file, unknown type: '%s'" % path

    # get rid of any existing variables and structure
    self.__clean_model()

    self._disable_incremental()
    self.__handler.read_model(self, path)
    if not 'time' in self.__vars.keys():
      self.new_var('time')
    self._enable_incremental()


  def save(self, save_path=None):
    '''
    Saves a model to either the model's file name, or specified path.
    '''
    if save_path and not save_path == self.props.file_name:
      self.props.file_name = save_path
    path = self.props.file_name
    if not self.__handler:
      self.__handler = io.get_handler(path)
    if not self.__handler:
      raise TypeError, "unable to save file, unknown type: '%s'" % path

    self.__handler.write_model(self, path)


  def new_var(self, var_name, var_eqn=None):
    '''
    Creates a new variable as part of the current model. 
    
    This is the only safe way to create a new variable; they should not
    be created on their own and 'added' to the model somehow.
    '''
    # create the variable first, because we do name and equation
    # validation in the constructor
    var_name = var_name.replace(' ', '_')
    new_var = Variable(self, var_name, var_eqn)

    # make sure it is actually new
    if self.__vars.has_key(var_name):
      raise ValueError, 'variable \'%s\' already exists' % var_name

    # keep track of the new variable
    self.__vars[var_name] = new_var
    self.__vars_list.append(new_var)

    new_var.connect('equation_changed', self.var_changed)

    if not new_var.props.valid:
      self.__vars_invalid.append(new_var)

    self.__update_model(new_var)

    return new_var


  def get_var(self, var_name):
    '''
    Get a reference to a variable from the current model.
    '''

    var_name = var_name.replace(' ', '_')

    if not self.__vars.has_key(var_name):
      log.error('tried to get non-existant variable "%s"' % var_name)
      return None

    return self.__vars[var_name]


  def get_vars(self):
    '''
    Get a list of all of the variables in the model.
    '''

    # return a copy, so they can mutate it or do whatever
    return list(self.__vars_list)


  def remove_var(self, var_name):
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


  def var_changed(self, var, old_equation):
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
