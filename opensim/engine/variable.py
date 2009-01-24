#===--- variable.py - OpenSim simulation variable ------------------------===#
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
# This file contains the pure python implementation of a 
# variable in a system dynamics model
#
#===----------------------------------------------------------------------===#

import logging

import gobject
from gettext import gettext as _

from constants import *
import simulator

log = logging.getLogger('opensim.var')



class Variable(gobject.GObject):

  __gproperties__ = {
    'name' :       (gobject.TYPE_STRING,             # object type
                          _('variable name'),        # nickname
                          _('name of the variable'), # description
                          None,                      # default
                          gobject.PARAM_READWRITE),  # flags
    'equation' :        (gobject.TYPE_STRING,
                          _('equation'),
                          _('equation of this variable'),
                          None,
                          gobject.PARAM_READWRITE),
    'units' :            (gobject.TYPE_STRING,
                          _('units'),
                          _('units of the equation'),
                          None,
                          gobject.PARAM_READWRITE),
    'comments' :         (gobject.TYPE_STRING,
                          _('comments'),
                          _('notes on the variable'),
                          None,
                          gobject.PARAM_READWRITE),
    'type' :             (gobject.TYPE_INT,
                          _('variable type'),
                          _('The basic type of the variable'),
                          0,
                          5,
                          0,
                          gobject.PARAM_READABLE),
    'parent' :           (gobject.TYPE_OBJECT,
                          _('parent simulator'),
                          _('Reference to the parent simulation'),
                          gobject.PARAM_READWRITE),
    'valid' :            (gobject.TYPE_BOOLEAN,
                          _('valid equation'),
                          _('True if the equation is okay'),
                          False,
                          gobject.PARAM_READABLE)
  }

  __gsignals__ = {
    'equation_changed' : (gobject.SIGNAL_RUN_FIRST,
                          gobject.TYPE_NONE,
                          (gobject.TYPE_STRING,))
  }


  def __init__(self, parent, name, equation=None, **kwargs):
    '''
    Initialize a new variable object, should ONLY be done from Simulator

    The way I've designed this, variables should only be created as
    part of a simulation, never on their own.  That's why they have
    a non-optional 'parent' argument, as we need some functions from
    the parent simulation to provide some of our method's functionality.
    '''
    gobject.GObject.__init__(self, **kwargs)

    self.__tokens = None
    self.__name = ''
    self.__equation = ''
    self.__units = ''
    self.__comments = ''
    self.__type = UNDEF
    self.__parent = None
    self.__valid = False

    if not parent or not isinstance(parent, simulator.Simulator):
      raise AttributeError, 'variable\'s parent must be a simulator, not %s' \
                            % type(parent)

    self.__parent = parent
    self.props.name = name
    self.props.equation = equation


  def _set_type(self, kind):
    '''
    A PRIVATE method for the parser to set the type of variable we have.
    '''
    self.__type = kind


  def do_get_property(self, prop):
    '''
    standart gobject getter.
    '''

    if prop.name == 'name':
      return self.__name
    elif prop.name == 'equation':
      return self.__equation
    elif prop.name == 'units':
      return self.__units
    elif prop.name == 'comments':
      return self.__comments
    elif prop.name == 'type':
      return self.__type
    elif prop.name == 'parent':
      return self.__parent
    elif prop.name == 'valid':
      return self.__valid

    raise AttributeError('unknown prop: "%s"' % prop.name)


  def do_set_property(self, prop,value):
    '''
    standart gobject setter.
    '''

    if not value:
      value = ''

    if prop.name == 'name':
      if not isinstance(value, str):
        raise AttributeError('names must be string, not %s' % type(value))
      value = value.strip()
      if value is '':
        raise AttributeError('variable names can\'t be blank')
      self.__name = value

    elif prop.name == 'equation':
      # accept ints or floats for the value as well, but cast them to 
      # strings, so that we can always count on having string values
      if isinstance(value, int) or isinstance(value, float):
        value = str(value)
      elif not isinstance(value, str):
        raise AttributeError('equations must be strings or numbers, not %s' %
                             type(value))
      old_equation = self.__equation

      # if you assign the same equation, we don't change anything
      if old_equation != value:
        self.__equation = value
        self.__tokens = None
        self.__type = UNDEF
        self.emit('equation_changed', old_equation)

    elif prop.name == 'units':
      if not isinstance(value, str):
        raise AttributeError('units must be string, not %s' % type(value))
      value = value.strip()
      self.__units = value

    elif prop.name == 'comments':
      if not isinstance(value, str):
        raise AttributeError('comments must be string, not %s' % type(value))
      self.__comments = value

    else:
      raise AttributeError('unknown prop: "%s" ("%s")' % (prop.name, value))


  def get_influences(self):
    '''
    Return a list of references (not names) of the variables that
    influence this variable
    '''

    return None



gobject.type_register(Variable)


def name_for_type(var_type):
  '''
  A nice helper function to return the string of the
  variable type (mostly for debugging purposes I assume)
  '''

  if var_type < 0 and var_type > 5:
    log.error('variable is out of range!')
    return ''

  # determining flows require context; save that for later
  if var_type is STOCK:
    return 'stock'
  elif var_type is FLOW:
    return 'var'
  elif var_type is AUX:
    return 'var'
  elif var_type is CONST:
    return 'constant'
  elif var_type is LOOKUP:
    return 'lookup'

  return 'undefined'

