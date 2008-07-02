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
from opensim.engine.simulator import Simulator


import logging

class SimGoo(goocanvas.Canvas):
  def __init__(self, **kwargs):
    super(SimGoo, self).__init__(**kwargs)
    self.highlighted = None
    self.engine = Simulator()
  

  def grab_highlight(self, widget):
    print("grabbing")
    
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
      print("awesome! new: '%s'", name)


  def remove_item(self, item):
    print("trying to remove")
    self.sim.display_vars.remove(item)
    item.remove()
    
    


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

    self.connect("focus_in_event", self.on_focus_in)
    self.connect("focus_out_event", self.on_focus_out)

    self.add(self.goocanvas)

    self.goocanvas.show()


  def set_active_tool(self, tool_type):
    self.active_tool = tool_type


  def get_active_tool(self):
    return self.active_tool


  def on_background_button_press (self, item, target, event):

    if event.button is 1:
      if self.active_tool is sim.STOCK:
        root = self.goocanvas.get_root_item()
        new_stock = widgets.StockItem(event.x, event.y, \
                                      parent=root, can_focus=True)
        self.display_vars.append(new_stock)
      elif self.active_tool is sim.VARIABLE:
        root = self.goocanvas.get_root_item()
        new_var = widgets.VariableItem(event.x, event.y, \
                                       parent=root, can_focus=True)
        self.display_vars.append(new_var)
      else:
        self.grab_focus()
        self.goocanvas.drop_highlight()
    return True


  def on_focus_in(self, target, event):
    print("awes, got focus %s", self)

    return False

  def on_focus_out(self, target, event):
    print("aww, left focus %s", self)

    return False


  def open_model(self, file_path):
    pass


  def save_model(self, file_path):
    logging.debug("Setting model file and saving.")
    self.engine.set_model_file(file_path)
    self.engine.save()

    f = open(file_path, 'a')
    try:
      self.save_visual_state(f, partial=True)
    finally:
      f.close()

    logging.debug("Saved model.")


  def save_visual_state(self, f, partial=False):
    logging.debug("Saving visual state.")
 
    f.write('\n<!-- below this is layout information for sketches -->\n')
    f.write('<visuals markup="1.0">\n\n')
    f.write('  <page name="default">\n\n')

    # here we go
    for widget in self.display_vars:
      f.write(widget.xml_representation())

    f.write('\n  </page>\n\n')
    f.write('</visuals>\n')

    if partial is True:
      f.write("</opensim>\n")


gobject.type_register(Canvas)
