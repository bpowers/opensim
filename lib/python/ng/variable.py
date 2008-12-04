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
import simulator
import tokens

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

    if not name or name == '':
      raise AttributeError('missing name for variable')

    if not parent or type(parent) is not simulator.Simulator:
      AttributeError('variable\'s parent must be a simulator, not %s' %
                     type(parent))

    self.__parent = parent
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
      if not __tokens:
        self.__update_tokens()
        return self.__type
    elif prop.name == 'parent':
      return self.__parent
    elif prop.name == 'valid':
      if not __tokens:
        self.__update_tokens()
        return self.__valid
    else:
      raise AttributeError('unknown prop: "%s"' % prop.name)


  def do_set_property(self, prop,value):
    '''
    standart gobject setter.
    '''

    if not value:
      value = ''

    if prop.name == 'name':
      if type(value) is not str:
        raise AttributeError('names must be string, not %s' % type(value))
      value = value.strip()
      if value is '':
        raise AttributeError('variable names can\'t be blank')
      self.__name = value

    elif prop.name == 'equation':
      # accept ints or floats for the value as well, but cast them to 
      # strings, so that we can always count on having string values
      if type(value) is int or type(value) is float:
        value = str(value)
      elif type(value) is not str:
        raise AttributeError('equations must be strings or numbers, not %s' %
                             type(value))
      old_equation = self.__equation

      if old_equation != value:
        self.__equation = value
        self.__tokens = None
        self.emit('equation_changed', old_equation)

    elif prop.name == 'units':
      if type(value) is not str:
        raise AttributeError('units must be string, not %s' % type(value))
      value = value.strip()
      self.__units = value

    elif prop.name == 'comments':
      if type(value) is not str:
        raise AttributeError('comments must be string, not %s' % type(value))
      self.__comments = value

    else:
      raise AttributeError('unknown prop: "%s" ("%s")' % (prop.name, value))


  def __update_tokens(self):
    log.debug('updating tokens for "%s"' % self.props.name)

    self.__toks = tokens.tokenize(self.__equation)

    # if we have no tokens, we are certainly not valid
    if len(self.__toks) is 0:
      self.__type = UNDEF
      self.__valid = False
      return

    if self.__toks[0][0] is tokens.IDENTIFIER and \
       self.__toks[0][1] == tokens.IDEN_INTEGRAL:
      self.__type = STOCK

    if self.__toks[0][0] is tokens.IDENTIFIER and \
       self.__toks[0][1] == '[':
      self.__type = LOOKUP

    if len(self.__toks) is 1 and self.__toks[0][0] is tokens.NUMBER:
      self.__type = CONSTANT

    log.debug('%s\'s tokens:' % self.__name)
    for tok, dat in self.__toks:
      log.debug('  %s:\t%s' % (tokens.name_for_token_type(tok), dat))


  def get_influences(self):
    '''
    Return a list of references (not names) of the variables that
    influence this variable
    '''

    if not self.__tokens:
      self.__update_tokens()

    return []


  def get_tokens(self):
    '''
    Return a list of the tokens in the current equation.
    '''

    if not self.__tokens:
      self.__update_tokens()

    return self.__toks


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

