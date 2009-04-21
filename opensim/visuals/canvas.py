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
import gobject
import gtk
import cairo

import math
import libxml2

import gaphas
import gaphas.tool as tool
from gaphas.tool import HandleTool, ItemTool

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

    self.canvas = self.model.canvas
    self._widgets_by_id = {}

    self.placement_tool = tools.PlacementTool(self.model)
    # useful tools chained together
    self.tool = tool.ToolChain().             \
                append(tools.HandleTool()).   \
                append(tool.HoverTool()).     \
                append(tool.ItemTool()).      \
                append(self.placement_tool).  \
                append(tool.RubberbandTool())

    # dpi, although I'm not sure if we use it at this point
    display = gtk.gdk.display_get_default()
    screen = display.get_default_screen()
    self.dpi = screen.get_resolution()

    self.set_size_request(1440, 900)


class Canvas(gtk.ScrolledWindow):

  __gtype_name__ = 'OpensimCanvas'

  def __init__(self):
    super(Canvas, self).__init__()

    self.active_tool = UNDEFINED

    self.model = model.SimModel(gaphas.Canvas())
    self.view = SimView(self.model)
    self.add_with_viewport(self.view)
    self.view.show()

    self.view.connect('focus-changed', self._focus_changed)

  def _focus_changed(self, view, old_item):
    '''
    Called when the focus changes.
    '''
    if old_item and old_item.new:
      self.model.remove(old_item)

  def set_active_tool(self, tool_type):
    '''
    Sets the active tool; for drawing new things on the canvas.
    '''
    self.active_tool = tool_type

    if tool_type is STOCK:
      widget_kind = 'stock'
    elif tool_type is VARIABLE:
      widget_kind = 'variable'
    elif tool_type is NONE:
      widget_kind = 'none'
    else:
      raise ValueError, 'unknown tool type.'

    del self.view.focused_item
    self.view.placement_tool.insert_kind = widget_kind

  def get_active_tool(self):
    return self.active_tool

