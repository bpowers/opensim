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

import math
import gobject, gtk
import cairo, pango
import gaphas
import gaphas.tool as tool
import constants as sim
import widgets
import libxml2
from opensim import engine
from opensim.engine import Simulator
import tools


import logging

class SimView(gaphas.GtkView):
  def __init__(self, engine, **kwargs):
    super(SimView, self).__init__(**kwargs)

    self.canvas = gaphas.Canvas()

    self.tool = tool.ToolChain().             \
                append(tool.HoverTool()).     \
                append(tool.RubberbandTool())

    self.engine = engine

    display = gtk.gdk.display_get_default()
    screen = display.get_default_screen()
    self.dpi = screen.get_resolution()

    self.set_flags(gtk.CAN_FOCUS)
    self.set_flags(gtk.CAN_DEFAULT)
    self.set_size_request(1440, 900)


  def new_variable(self, var_name):
    return self.engine.new_var(var_name, '')



class Canvas(gtk.ScrolledWindow):

  active_tool = sim.UNDEFINED

  def __init__(self):
    super(Canvas, self).__init__()

    self.engine = Simulator()

    self.display_vars = []

    # allow us to add all our layout information to the save file
    self.engine.connect("saving", self.save_visual_state)

    self.view = SimView(self.engine)
    self.add_with_viewport(self.view)
    self.view.show()


  def set_active_tool(self, tool_type):
    self.active_tool = tool_type


  def get_active_tool(self):
    return self.active_tool


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
    self.goocanvas.engine = Simulator()
    self.goocanvas.engine.load(file_path)
    self.engine = self.goocanvas.engine
    # allow us to add all our layout information to the 
    # save file, need to 're'-connect this since its a new engine
    self.engine.connect("saving", self.save_visual_state)

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
          new_var.var = self.engine.get_var(var_name)
          self.display_vars.append(new_var)
        if name == "stock":
          new_var = widgets.StockItem(x, y, width, height, var_name,
                                      focus=False, parent=goo_root, 
                                      can_focus=True)
          new_var.var = self.engine.get_var(var_name)
          self.display_vars.append(new_var)

      if name == "flow" or name == "link":
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

        new_var = None
        if name == "flow":
          # FIXME: we assume correct input.  handle errors!
          new_var = widgets.FlowItem(None, var_name, (x1, y1), (x2, y2),
                                     focus=False, parent=goo_root,
                                     dragging=False, can_focus=True)
          new_var.var = self.engine.get_var(var_name)
        else:
          new_var = widgets.LinkItem(None, (x1, y1), (x2, y2),
                                     focus=False, parent=goo_root,
                                     dragging=False, can_focus=True)
        # add info to post in order to finish hooking up rate.

        new_var.lower(None)
        self.display_vars.append(new_var)
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
    logging.debug("Canvas: Setting model file and saving.")
    self.engine.props.file_name = file_path
    self.engine.save()

    logging.debug("Canvas: Saved model.")


  def save_visual_state(self, widget, f):
    logging.debug("Canvas: Saving visual state.")
 
    f.write('\n<!-- below this is layout information for sketches -->\n')
    f.write('<visuals markup="1.0">\n\n')
    f.write('  <page name="default">\n\n')

    # here we go
    for widget in self.display_vars:
      f.write(widget.xml_representation())

    f.write('\n  </page>\n\n')
    f.write('</visuals>\n\n')



gobject.type_register(Canvas)
