#===--- canvas.py - OpenSim Canvas ----------------------------------------===#
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
#===-----------------------------------------------------------------------===#
#
# This file contains the implementation of the OpenSim canvas, where you can 
# graphically build and manipulate System Dynamics models.
#
#===-----------------------------------------------------------------------===#

import pygtk
pygtk.require("2.0")
import gobject, gtk
import cairo, pango

import math
import libxml2

import gaphas
import gaphas.tool as tool
from gaphas.tool import HandleTool, ItemTool

from opensim import engine
from opensim.engine import Simulator
from constants import *
import widgets
import tools
import model

import logging


class SimView(gaphas.GtkView):

  __gtype_name__ = 'SimView'

  def __init__(self, model, **kwargs):
    super(SimView, self).__init__(**kwargs)

    self.model = model
    self.model.connect('row-changed', self.__row_changed_cb)
    self.model.connect('row-deleted', self.__row_deleted_cb)

    self.canvas = gaphas.Canvas()
    self._widgets_by_id = {}

    # useful tools chained together
    self.tool = tool.ToolChain().             \
                append(tool.HandleTool()).    \
                append(tool.HoverTool()).     \
                append(tool.ItemTool()).      \
                append(tool.RubberbandTool())

    # dpi, although I'm not sure if we use it at this point
    display = gtk.gdk.display_get_default()
    screen = display.get_default_screen()
    self.dpi = screen.get_resolution()

    self.set_size_request(1440, 900)


  def widget_for_id(self, widget_id):
    '''
    Return the widget corresponding to the given integer ID.
    '''
    try:
      return self._widgets_by_id[widget_id]
    except KeyError:
      return None


  def __row_changed_cb(self, model, path, iter):
    row = model[iter]

    if not row[KIND]:
      return

    obj = self.widget_for_id(row[ID])
    if obj is None:
      logging.debug('__row_changed_cb %r' % ((row[ID], row[NAME], row[KIND], row[X], row[Y],),))
      kind = get_widget_kind_by_string(row[KIND])
      new_item = kind(row[NAME], row[X], row[Y], row[WIDTH], row[HEIGHT], row[ID])
      self._widgets_by_id[row[ID]] = new_item
      self.canvas.add(new_item)
    else:
      obj.name = row[NAME]
      obj.set_position(row[X], row[Y])
      obj.width = row[WIDTH]
      obj.height = row[HEIGHT]


  def __row_deleted_cb(self, model, path):
    logging.debug('__row_deleted_cb %r' % path)
    raise NotImplementedError()


class Canvas(gtk.ScrolledWindow):

  __gtype_name__ = 'Canvas'

  def __init__(self):
    super(Canvas, self).__init__()

    self.active_tool = UNDEFINED

    self.model = model.SimModel()

    self.view = SimView(self.model)
    self.add_with_viewport(self.view)
    self.view.show()


  def set_active_tool(self, tool_type):
    '''
    Sets the active tool; for drawing new things on the canvas.
    '''
    if tool_type is STOCK:
      widget_type = 'stock'
    elif tool_type is VARIABLE:
      widget_type = 'variable'
    else:
      raise ValueError, 'wtf.'

    self.view.tool.grab(tools.PlacementTool(self.model, widget_type))


  def get_active_tool(self):
    return self.active_tool


def get_widget_kind_by_string(kind_name):
  '''
  Return the appropriate widget type for a given string.
  '''
  if kind_name == 'stock':
    return widgets.StockItem
  elif kind_name == 'variable':
    return widgets.VariableItem
  else:
    raise ValueError

