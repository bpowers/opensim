from gettext import gettext as _

import pygtk
pygtk.require("2.0")

import gtk
from sugar.graphics.toggletoolbutton import ToggleToolButton

class ModelToolbar(gtk.Toolbar):
  """Provides the toolbar containing the basic modeling functions"""

  def __init__(self):
    gtk.Toolbar.__init__(self)

    #Get our 4 buttons & add type attributes to them
    self.stock = ToggleToolButton("icon-stock")
    self.stock.set_tooltip(_('Create Stocks'))
    self.insert(self.stock, -1)
    self.stock.show()
    self.stock.type = STOCK

    self.flow = ToggleToolButton("icon-flow")
    self.flow.set_tooltip(_('Create Flows'))
    self.insert(self.flow, -1)
    self.flow.show()
    self.flow.type = FLOW    

    self.variable = ToggleToolButton("icon-var")
    self.variable.set_tooltip(_('Create Variables'))
    self.insert(self.variable, -1)
    self.variable.show()
    self.variable.type = VARIABLE

    self.influence = ToggleToolButton("icon-infl")
    self.influence.set_tooltip(_('Create Influence Arrows'))
    self.insert(self.influence, -1)
    self.influence.show()
    self.influence.type = INFLUENCE


