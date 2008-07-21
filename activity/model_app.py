#===--- model-app.py - Sugar OpenSim interface ----------------------------===#
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
# This file contains the Sugar interface to OpenSim.
#
#===-----------------------------------------------------------------------===#

import pygtk
pygtk.require("2.0")

from sugar.activity import activity

import math, logging
from gettext import gettext as _

import opensim.visuals as sim


class ModelActivity(activity.Activity):
  '''
  The base class for the Model activity.
  '''

  def drawing_tool_toggled(self, widget):
    # if a widget is being un-toggled, then we don't have
    # to worry about it (otherwise breaks toggle effect)
    if widget.get_active() == False:
      # make sure we catch the case where no tool is active.
      if widget.type == self.canvas.get_active_tool():
        self.canvas.set_active_tool(sim.NONE)
      return

    # switch between possible widgets.
    if widget.type != sim.VARIABLE:
      self.model_toolbar.variable.set_active(False)    
    if widget.type != sim.INFLUENCE:
      self.model_toolbar.influence.set_active(False)
    if widget.type != sim.STOCK:
      self.model_toolbar.stock.set_active(False)
    if widget.type != sim.FLOW:
      self.model_toolbar.flow.set_active(False)

    # let the canvas know what tool is active
    self.canvas.set_active_tool(widget.type)

    
  def __init__(self, handle):
    activity.Activity.__init__(self, handle)

    #setup activity sharing here!!!

    self.canvas = sim.Canvas()
    self.canvas.show()

    # Creates the Toolbox. It contains the Activity Toolbar, which is the
    # bar that appears on every Sugar window and contains essential
    # functionalities, such as the 'Collaborate' and 'Close' buttons.
    toolbox = activity.ActivityToolbox(self)

    self.edit_toolbar = activity.EditToolbar()
    toolbox.add_toolbar(_('Edit'), self.edit_toolbar)
    self.edit_toolbar.show()

    self.model_toolbar = sim.ModelToolbar()
    toolbox.add_toolbar(_('Model'), self.model_toolbar)
    self.model_toolbar.show()

    self.model_toolbar.stock.connect('toggled', self.drawing_tool_toggled)
    self.model_toolbar.flow.connect('toggled', self.drawing_tool_toggled)
    self.model_toolbar.influence.connect('toggled', self.drawing_tool_toggled)
    self.model_toolbar.variable.connect('toggled', self.drawing_tool_toggled)

    self.simulate_toolbar = sim.SimulateToolbar()
    toolbox.add_toolbar(_('Simulate'), self.simulate_toolbar)
    self.simulate_toolbar.show()

    self.view_toolbar = sim.ViewToolbar()
    toolbox.add_toolbar(_('View'), self.view_toolbar)
    self.view_toolbar.show()

    self.set_toolbox(toolbox)
    toolbox.show()
 
    self.set_canvas(self.canvas)


  def write_file(self, file_path):
    '''
    Implement this method to save your activity's state.
    '''
    logging.debug('ModelActivity: writing file')
    self.canvas.save_model(file_path)
    logging.debug('ModelActivity: done writing file')

    
  def read_file(self, file_path):
    '''
    Implement this method to resume state saved in write_file().
    '''
    logging.debug('ModelActivity: reading file')
    self.canvas.open_model(file_path)
    logging.debug('ModelActivity: done reading file')

