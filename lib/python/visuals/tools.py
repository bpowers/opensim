#===--- tools.py - Toolbars for Sugar OpenSim -----------------------------===#
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
# This file contains implementations of toolbars for the Model activity.
#
#===-----------------------------------------------------------------------===#

from gettext import gettext as _

import pygtk
pygtk.require("2.0")

import gtk
from sugar.graphics.toggletoolbutton import ToggleToolButton

from constants import *


class ModelToolbar(gtk.Toolbar):
  '''Provides the toolbar containing the basic modeling functions'''

  def __init__(self):
    gtk.Toolbar.__init__(self)

    #Get our 4 buttons & add type attributes to them
    self.stock = ToggleToolButton("opensim-stock")
    self.stock.set_tooltip(_('Create Stocks'))
    self.insert(self.stock, -1)
    self.stock.show()
    self.stock.type = STOCK

    self.flow = ToggleToolButton("opensim-flow")
    self.flow.set_tooltip(_('Create Flows'))
    self.insert(self.flow, -1)
    self.flow.show()
    self.flow.type = FLOW    

    self.variable = ToggleToolButton("opensim-var")
    self.variable.set_tooltip(_('Create Variables'))
    self.insert(self.variable, -1)
    self.variable.show()
    self.variable.type = VARIABLE

    self.influence = ToggleToolButton("opensim-infl")
    self.influence.set_tooltip(_('Create Influence Arrows'))
    self.insert(self.influence, -1)
    self.influence.show()
    self.influence.type = INFLUENCE



class LineControl:
  '''
  This class keeps track of things related to the process of
  adding new lines to the canvas.
  '''
  def __init__(self):
    # if we've got a line we're making, we want to attach the
    # motion callback to it.  When we're done moving the line, 
    # detach the callback.  keep track of the callback id here.
    self.cb_id = None
    self.line = None
    self._canvas = None


  def set_canvas(self, canvas):
    self._canvas = canvas


  def cleanup(self, item):
    if item is self.line:
      if self.cb_id and self._canvas:
        self._canvas.get_root_item().disconnect(self.cb_id)
        self.cb_id = None
      self.line = None
  

