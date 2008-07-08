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

import gobject, gtk, cairo, pango, goocanvas, math
import constants as sim
import widgets
import libxml2
from opensim.engine.simulator import Simulator


import logging

class SimGoo(goocanvas.Canvas):
  def __init__(self, **kwargs):
    super(SimGoo, self).__init__(**kwargs)
    self.highlighted = None
    self.engine = Simulator()

    # used to denote when we're overriding mouseclicks on canvas items.
    # mostly for when we're drawing lines and rates
    self.override = False
  

  def grab_highlight(self, widget):
    if self.highlighted is widget:
      return
    elif self.highlighted is not None:
      self.highlighted.emit("highlight_out_event", self)
      self.highlighted = None
    self.highlighted = widget
    widget.emit("highlight_in_event", self)


  def drop_highlight(self):
    if self.highlighted is not None:
      self.highlighted.emit("highlight_out_event", self)
      self.highlighted = None


  def update_name(self, name, item, new=False):
    if new:
      logging.debug("SimGooCanvas: awesome! new: '%s'", name)


  def remove_item(self, item):
    logging.debug("SimGooCanvas: removing '%s'." % item.name())
    self.sim.display_vars.remove(item)
    item.remove()
    self.highlighted = None
    
    


class Canvas (gtk.ScrolledWindow):

  active_tool = sim.UNDEFINED

  def __init__(self):
    super(Canvas, self).__init__()
    self.last_state = 0
    self.dragging = False
    self.drag_x = 0
    self.drag_y = 0

    self.goocanvas = SimGoo()
    self.engine = self.goocanvas.engine 
    self.goocanvas.sim = self
    self.display_vars = []

    display = gtk.gdk.display_get_default()
    screen = display.get_default_screen()
    self.goocanvas.dpi = screen.get_resolution()

    # white background
    color_white = gtk.gdk.Color(65535, 65535, 65535, 0)
    self.goocanvas.modify_base(gtk.STATE_NORMAL, color_white)

    self.goocanvas.set_flags(gtk.CAN_FOCUS)
    self.goocanvas.set_flags(gtk.CAN_DEFAULT)
    self.goocanvas.set_size_request(1440, 900)
    self.goocanvas.set_bounds(0, 0, 1440, 900)

    self.goocanvas.automatic_bounds = True

    root = self.goocanvas.get_root_item()
    root.connect("button_press_event", self.on_background_button_press)
    root.connect("motion_notify_event", self.on_motion)

    self.connect("focus_in_event", self.on_focus_in)
    self.connect("focus_out_event", self.on_focus_out)

    self.add(self.goocanvas)

    self.goocanvas.show()


  def set_active_tool(self, tool_type):
    self.active_tool = tool_type
    if tool_type is sim.FLOW or tool_type is sim.INFLUENCE:
      self.goocanvas.override = True
    else:
      self.goocanvas.override = False
    if self.goocanvas.highlighted:
      self.goocanvas.highlighted.emit("highlight_out_event", self)


  def get_active_tool(self):
    return self.active_tool


  def on_background_button_press (self, item, target, event):
    root = self.goocanvas.get_root_item()
    
    if event.button is 1:
      if self.active_tool is sim.STOCK:
        new_stock = widgets.StockItem(event.x, event.y, \
                                      parent=root, can_focus=True)
        self.display_vars.append(new_stock)

      elif self.active_tool is sim.VARIABLE:
        new_var = widgets.VariableItem(event.x, event.y, \
                                       parent=root, can_focus=True)
        self.display_vars.append(new_var)

      elif self.active_tool is sim.FLOW:
        logging.debug("background click for Flow")
        widget = self.goocanvas.get_item_at(event.x, event.y, False)
        if widget is not None and type(widget) is widgets.StockItem:
          new_var = widgets.RateItem((event.x, event.y), flow_out=widget,
                                      parent=root, can_focus=True)
        else:
          # create a cloud if they clicked on the background for a flow 
          pass
      else:
        self.grab_focus()
        self.goocanvas.drop_highlight()
    return True


  def on_motion(self, item, target, event):
    if self.active_tool is sim.FLOW or self.active_tool is sim.VARIABLE:
      #logging.debug("motion notify!")
      pass


  def on_focus_in(self, target, event):
    logging.debug("Canvas: got focus")

    return False

  def on_focus_out(self, target, event):
    logging.debug("Canvas: left focus")

    return False


  def open_model(self, file_path):
    logging.debug("Dropping highlight to open.")
    self.goocanvas.drop_highlight()
    logging.debug("Opening model '%s'." % file_path)
    doc = libxml2.parseFile(file_path)

    root = doc.children
    if root.name != "opensim":
      logging.error("not an opensim XML file")
      return

    vis_root = root.children
    # skip through model and text children (XML treats 
    # whitespace as text elements)
    while vis_root is not None and vis_root.name != "visuals":
      vis_root = vis_root.next

    if vis_root is None:
      logging.error("no node named 'visuals'")
      return

    # make a copy so we're not editing the list we're iterating through
    vars_copy = list(self.display_vars) 
    for old_widget in vars_copy:
      self.goocanvas.remove_item(old_widget)

    page = vis_root.children
    while page is not None and page.name != "page":
        page = page.next
    
    if page is None:
      logging.error("Canvas: each visual part of a savefile must have " + \
                    "at least one page.")

    goo_root = self.goocanvas.get_root_item()
    var = page.children
    # now for the meat and potatoes.
    while var is not None:
      name = var.name
      if name == "stock" or name == "var":
        var_item = var.children
        var_name = "undefined"
        x, y, width, height = 0, 0, 100, 100
        while var_item is not None:
          if var_item.name == "x":
            x = float(var_item.content)
          elif var_item.name == "y":
            y = float(var_item.content)
          elif var_item.name == "width":
            width = float(var_item.content)
          elif var_item.name == "height":
            height = float(var_item.content)
          elif var_item.name == "name":
            var_name = var_item.content
          var_item = var_item.next

        # using ints for output so that its more readable.  values
        # are actually floats
        logging.debug("Canvas: Adding variable " + 
                      "'%s' (x'%d', y'%d', w'%d', h'%d')" % \
                      (var_name, x, y, width, height))

        # FIXME: we assume correct input.  handle errors!
        if name == "var":
          new_var = widgets.VariableItem(x, y, width, height, var_name,
                                         focus=False, parent=goo_root, 
                                         can_focus=True)
          self.display_vars.append(new_var)
        if name == "stock":
          new_var = widgets.StockItem(x, y, width, height, var_name,
                                      focus=False, parent=goo_root, 
                                      can_focus=True)
          self.display_vars.append(new_var)
      var = var.next

    doc.freeDoc()


  def save_model(self, file_path):
    logging.debug("Canvas: Dropping highlight to save.")
    self.goocanvas.drop_highlight()
    logging.debug("Canvas: Setting model file and saving.")
    self.engine.set_model_file(file_path)
    self.engine.save()

    f = open(file_path, 'a')
    try:
      self.save_visual_state(f)
      f.write("</opensim>\n")
    finally:
      f.close()

    logging.debug("Canvas: Saved model.")


  def save_visual_state(self, f):
    logging.debug("Canvas: Saving visual state.")
 
    f.write('\n<!-- below this is layout information for sketches -->\n')
    f.write('<visuals markup="1.0">\n\n')
    f.write('  <page name="default">\n\n')

    # here we go
    for widget in self.display_vars:
      f.write(widget.xml_representation())

    f.write('\n  </page>\n\n')
    f.write('</visuals>\n')



gobject.type_register(Canvas)
