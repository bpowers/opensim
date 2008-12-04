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

import pygtk
pygtk.require("2.0")

import gobject
from gettext import gettext as _
import logging as log

from constants import *

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

  __tokens = None

  # private vars
  __name = ''
  __equation = ''
  __units = ''
  __comments = ''
  __type = 0
  __parent = None
  __valid = False


  def __init__(self, parent, name, equation=None, **kwargs):
    gobject.GObject.__init__(self, **kwargs)

    if not parent:
      log.error('missing parent simulator')
      raise ValueError

    if not name or name == '':
      log.error('missing name for variable')
      raise ValueError

    self.props.name = name

    # it should simplify things if we guaruntee a non-None, str equation
    if not equation:
      equation = ''
    self.props.equation = str(equation)


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
      raise NotImplementedError
    elif prop.name == 'parent':
      return self.__parent
    elif prop.name == 'valid':
      raise NotImplementedError
    else:
      raise AttributeError('unknown prop: "%s"' % prop.name)


  def do_set_property(self, prop,value):
    '''
    standart gobject setter.
    '''

    if not value:
      value = ''

    if prop.name == 'name':
      self.__name = value
    elif prop.name == 'equation':
      old_equation = self.__equation
      self.__equation = value
      self.emit('equation_changed', old_equation)
    elif prop.name == 'units':
      self.__units = value
    elif prop.name == 'comments':
      self.__comments = value
    else:
      raise AttributeError('unknown prop: "%s" ("%s")' % (prop.name, value))


  def get_influences(self):
    log.debug('get_influences stub')
    return []


  def get_tokens(self):
    log.debug('get_tokens stub')


gobject.type_register(Variable)



def name_for_type(var_type):
  '''
  A nice helper function to return the string of the
  variable type (mostly for debugging purposes I assume)
  '''

  if var_type < 0 and var_type > 5:
    log.error('variable is out of range!')
    return ''

  if var_type is STOCK:
    return 'stock'
  elif var_type is FLOW:
    return 'flow'
  elif var_type is AUX:
    return 'auxilliary'
  elif var_type is CONST:
    return 'constant'
  elif var_type is LOOKUP:
    return 'lookup'
  else:
    return 'undefined'

