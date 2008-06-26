import pygtk
pygtk.require("2.0")

from sugar.activity import activity

import math, logging
from gettext import gettext as _

import opensim as sim


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

    self.set_toolbox(toolbox)
    toolbox.show()
 
    self.set_canvas(self.canvas)



  def write_file(self, file_path):
    '''
    Implement this method to save your activity's state.
    '''
    logging.debug('writing file')
    self.canvas.write_model(file_path)
    f = open(file_path, 'w')
    try:
      f.write(self._get_log())
    finally:
      f.close()


    
  def read_file(self, file_path):
    '''
    Implement this method to resume state saved in write_file().
    '''
    raise NotImplementedError
