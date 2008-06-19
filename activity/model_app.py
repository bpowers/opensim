import pygtk
pygtk.require("2.0")

from sugar.activity import activity
from sugar.graphics.toolbutton import ToolButton

import gtk, math
from gettext import gettext as _

import opensim as sim


class ModelToolbar(gtk.Toolbar):
    """Provides the toolbar containing the basic modeling functions"""

  def __init__(self):
    gtk.Toolbar.__init__(self)

    #Get our 4 buttons & add type attributes to them
    self.variable = ToolButton("icon-var")
    self.variable.set_tooltip(_('Create Variables'))
    self.insert(self.variable, -1)
    self.variable.show()
    self.variable.type = sim.VARIABLE

    self.influence = ToolButton("icon-infl")
    self.influence.set_tooltip(_('Create Influence Arrows'))
    self.insert(self.influence, -1)
    self.influence.show()
    self.influence.type = sim.INFLUENCE

    self.stock = ToolButton("icon-stock")
    self.stock.set_tooltip(_('Create Stocks'))
    self.insert(self.stock, -1)
    self.stock.show()
    self.stock.type = sim.STOCK

    self.flow = ToolButton("icon-flow")
    self.flow.set_tooltip(_('Create Flows'))
    self.insert(self.flow, -1)
    self.flow.show()
    self.flow.type = sim.FLOW    


class ModelActivity(activity.Activity):
  '''
  The base class for the Model activity.
  '''

  def hello(self, widget, data=None):
    logging.info('Hello World')

    
  def __init__(self, handle):
    print "running activity init", handle
    activity.Activity.__init__(self, handle)
    print "activity running"

    #setup activity sharing here!!!

    self.canvas = sim.Canvas()
    self.canvas.show()

    # Creates the Toolbox. It contains the Activity Toolbar, which is the
    # bar that appears on every Sugar window and contains essential
    # functionalities, such as the 'Collaborate' and 'Close' buttons.
    toolbox = activity.ActivityToolbox(self)

    toolbar = activity.EditToolbar()
    toolbox.add_toolbar(_('Edit'), toolbar)
    toolbar.show()

    #self._edit_toolbar = activity.EditToolbar(self.canvas)
    #toolbox.add_toolbar(_('Edit'), self._edit_toolbar)
    self.set_toolbox(toolbox)
    toolbox.show()
    #self._edit_toolbar.show()
 
    
    self.set_canvas(self.canvas)
     
    print "AT END OF THE CLASS"



  def write_file(self, file_path):
    '''
    Implement this method to save your activity's state.
    '''
    raise NotImplementedError

    
  def read_file(self, file_path):
    '''
    Implement this method to resume state saved in write_file().
    '''
    raise NotImplementedError
