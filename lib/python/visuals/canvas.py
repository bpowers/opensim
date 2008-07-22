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
import tools


import logging

class SimGoo(goocanvas.Canvas):
  def __init__(self, line_control=tools.LineControl(), **kwargs):
    super(SimGoo, self).__init__(**kwargs)
    self.highlighted = None
    self.highlight_cb = None
    self.engine = Simulator()

    # used to denote when we're overriding mouseclicks on canvas items.
    # mostly for when we're drawing lines and rates
    self.override = False

    # if we've got a line we're making, we want to attach the
    # motion callback to it.  When we're done moving the line, 
    # detach the callback.  keep track of the callback id here.
    self.line = line_control
  

  def grab_highlight(self, widget):
    if self.highlighted is widget:
      return
    elif self.highlighted is not None:
      self.highlighted.emit("highlight_out_event", self)
    self.highlighted = widget
    self.highlight_cb = widget.connect("highlight_out_event", self.highlight_out)
    widget.emit("highlight_in_event", self)
    self.grab_focus(self.highlighted)
    #logging.debug("done grab_highlight")


  def drop_highlight(self):
    if self.highlighted is not None:
      self.highlighted.emit("highlight_out_event", self)
      self.highlighted = None


  def highlight_out(self, item, target):
    #logging.debug("SimGoo: highlight_out")
    if item is not self.highlighted:
      logging.error("receiving highlight events, but not from right object.")
      return False

    self.highlighted.disconnect(self.highlight_cb)
    self.highlight_cb = None
    self.highlighted = None
    self.sim.grab_focus()
    return False


  def update_name(self, old_name, item, new=False):
    if old_name == item.name():
      return

    if new or old_name == "":
      logging.debug("SimGooCanvas: awesome! new: '%s'" % item.name())
      self.engine.new_variable(item.name())
    else:
      logging.debug("SimGooCanvas: renaming '%s' to '%s'" % 
                    (old_name, item.name()))
      self.engine.rename_variable(old_name, item.name())


  def remove_item(self, item):
    logging.debug("SimGooCanvas: removing '%s'." % item.name())
    self.sim.display_vars.remove(item)
    item.remove()
    self.highlighted = None

    # give the LineControl instance the variable we're removing.
    # if we're removing the line currently being made, then 
    # reset LineControl
    self.line.cleanup(item)


  def show_editor(self, item):
    logging.debug("showing equation editor for: %s" % item.name())
    editor = tools.EquationEditor()
    result = editor.run()
    editor.hide()

    if result == gtk.RESPONSE_OK:
      logging.debug("okay, got an equation:")
      logging.debug("\t'%s'" % editor.equation.get_text())
    else:
      logging.debug('oh well, canceled editor or something.')



class Canvas (gtk.ScrolledWindow):

  active_tool = sim.UNDEFINED

  def __init__(self):
    super(Canvas, self).__init__()
    self.last_state = 0
    self.dragging = False
    self.drag_x = 0
    self.drag_y = 0

    self.line = tools.LineControl()
    self.line.set_canvas(self)

    self.goocanvas = SimGoo(self.line)
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

      elif self.active_tool is sim.INFLUENCE:
        widget = self.goocanvas.get_item_at(event.x, event.y, False)

        if self.line.cb_id is None:
          if widget is None or (type(widget) is not widgets.StockItem and
                                type(widget) is not widgets.FlowItem and 
                                type(widget) is not widgets.VariableItem):
            logging.debug("Canvas: can't start a link here")
            return True

          logging.debug("sweet new line")
          self.line.new_link(widget)
        else:
          if widget is None or (type(widget) is not widgets.FlowItem and 
                                type(widget) is not widgets.VariableItem):
            logging.debug("Canvas: can't end a link here")
            return True

          self.line.end_flow(widget)

      elif self.active_tool is sim.FLOW:
        widget = self.goocanvas.get_item_at(event.x, event.y, False)


        if self.line.cb_id is None:
          if widget is not None and type(widget) is not widgets.StockItem:
            # if we landed on anything but a stock, break
            return True
          elif not widget:
            # create a cloud if they clicked on the background for a flow 
            logging.debug("creating a cloud")
            new_var = widgets.CloudItem(event.x, event.y, parent=root)
            self.display_vars.append(new_var)
            widget = new_var
        
          logging.debug("starting new flow")
          self.line.new_flow(widget)

        else:

          #okay, we're finishing the line here.
          if widget is not None and type(widget) is not widgets.StockItem:
            # if we landed on anything but a stock, break
            return True
          elif not widget:
            # don't allow flows between 2 clouds
            if type(self.line.line.flow_from) is widgets.CloudItem:
              self.goocanvas.remove_item(self.line.line)
              return True

            # create a cloud if they clicked on the background for a flow 
            logging.debug("creating a cloud")
            new_var = widgets.CloudItem(event.x, event.y, parent=root)
            self.display_vars.append(new_var)
            widget = new_var
          
          logging.debug("ending new flow")
          self.line.end_flow(widget)

      else:
        self.goocanvas.drop_highlight()
        self.grab_focus()

    return True


  def on_motion(self, item, target, event):
    if self.active_tool is sim.FLOW or self.active_tool is sim.VARIABLE:
      #logging.debug("motion notify!")
      pass


  def on_focus_in(self, target, event):
    #logging.debug("Canvas: got focus")

    return False

  def on_focus_out(self, target, event):
    #logging.debug("Canvas: left focus")

    return False


  def open_model(self, file_path):
    logging.debug("path ascii: '%s'" % file_path.encode('ascii'))
    #logging.debug("path utf-8: '%s'" % file_path)
    logging.debug("Dropping highlight to open.")
    self.goocanvas.drop_highlight()

    logging.debug("Loading model part of file.")
    # the engine requires file paths to be in ASCII (OLD SCHOOL REPRESENT)
    self.goocanvas.engine = Simulator(file_path.encode('ascii'))
    self.engine = self.goocanvas.engine
    self.goocanvas.engine.info()

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

    post = []
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

      if name == "flow":
        var_item = var.children
        var_name, start, end = "undefined", "none", "none"
        x1, x2, y1, y2 = 0, 0, 0, 0
        while var_item is not None:
          if var_item.name == "x1":
            x1 = float(var_item.content)
          elif var_item.name == "y1":
            y1 = float(var_item.content)
          elif var_item.name == "x2":
            x2 = float(var_item.content)
          elif var_item.name == "y2":
            y2 = float(var_item.content)
          elif var_item.name == "name":
            var_name = var_item.content
          elif var_item.name == "start":
            start = var_item.content
          elif var_item.name == "end":
            end = var_item.content
          var_item = var_item.next

        # using ints for output so that its more readable.  values
        # are actually floats
        logging.debug("Canvas: Adding flow " + 
                      "'%s' (x1'%d', y1'%d', x2'%d', y2'%d', s'%s' e'%s')" % \
                      (var_name, x1, y1, x2, y2, start, end))

        # FIXME: we assume correct input.  handle errors!
        new_var = widgets.FlowItem(None, var_name, (x1, y1), (x2, y2),
                                   focus=False, parent=goo_root, 
                                   can_focus=True)
        new_var.lower(None)
        self.display_vars.append(new_var)
        
        # add info to post in order to finish hooking up rate.
        post.append((new_var, start, end))

      var = var.next

    doc.freeDoc()
    
    # now do postprocessing to finish hooking up flows and influences
    for var in post:
      logging.debug("hooking up '%s'", var[0].name())
      
      widget = None
      if var[1] == "cloud":
        logging.debug("creating a cloud for the start")
        new_cloud = widgets.CloudItem(var[0].x1, var[0].y1, parent=goo_root)
        self.display_vars.append(new_cloud)
        widget = new_cloud
      else:
        # here we need to get the stock with the name var[1]
        for d_var in self.display_vars:
          if d_var.name() == var[1]:
            widget = d_var
            break
      if widget:
        logging.debug("setting flow-from")
        var[0].set_flow_from(widget)
      
      widget = None
      if var[2] == "cloud":
        logging.debug("creating a cloud for the start")
        new_cloud = widgets.CloudItem(var[0].x2, var[0].y2, parent=goo_root)
        self.display_vars.append(new_cloud)
        widget = new_cloud
      else:
        # here we need to get the stock with the name var[2]
        for d_var in self.display_vars:
          if d_var.name() == var[2]:
            widget = d_var
            break
      if widget:
        logging.debug("setting flow-to")
        var[0].set_flow_to(widget)


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
