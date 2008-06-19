from sugar.activity import activity


import pygtk
pygtk.require("2.0")


import gtk, gtk.glade, cairo, pango, goocanvas, math
from gettext import gettext as _

import opensim as sim


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
