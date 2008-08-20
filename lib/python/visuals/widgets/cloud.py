#===--- stock.py - OpenSim Stock widget -----------------===#
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
# This file contains the implementation of the stock widget
#
#===-----------------------------------------------------------------------===#

import gobject
import gtk
import goocanvas
import math
import cairo, pangocairo
import rsvg
import os

import logging

from text import TextInfo
from item import SimItem


class CloudItem(SimItem):

  def __init__(self, x, y, width=55, height=55, **kwargs):
    super(CloudItem, self).__init__(**kwargs)

    self._new = True

    self.x = x - width/2.0
    self.y = y - height/2.0
    self.width = width
    self.height = height
    self.dragging = False

    # find our cloud icon hanging out with the other icons.
    icon_paths = gtk.icon_theme_get_default().get_search_path()
    cloud_path = None

    for path in icon_paths:
      possible_path = os.path.join(path, "opensim-cloud.svg")
      if os.path.isfile(possible_path):
        cloud_path = possible_path
        break

    if cloud_path is None:
      logging.error("could not find cloud svg!")
      raise Exception

    self._cloud = rsvg.Handle(cloud_path)

    # keep track of inflows and outflows, for use in engine
    self.inflows = []
    self.outflows = []

    self.__needs_resize_calc = True


  def name(self):
    return "cloud"


  def do_simple_create_path(self, cr):
    self.ensure_size(cr)

    # define the bounding path here.
    cr.rectangle(self.x, self.y, self.width, self.height)


  def center(self):
    return (int(self.x + self.width/2), int(self.y))


  def abs_center(self):
    if self.__needs_resize_calc:
      logging.debug("WEIRD DEBUG")
      return self.center()
    return (int(self.bounds_x1 + self.width/2), 
            int(self.bounds_y2 - self.height/2))


  def edge_point(self, end_point):
    center_x, center_y = self.abs_center()
    
    line_angle = math.atan2((end_point[1] - center_y), 
                            (end_point[0] - center_x))
    if line_angle < 0: line_angle = 2*math.pi + line_angle
    
    
    radius = (self.width + self.height)/4
    
    center_x = center_x + radius * math.cos(line_angle)
    center_y = center_y + radius * math.sin(line_angle)
    
    logging.debug("CLOUD: %5.1f (%d, %d)" % (math.degrees(line_angle), 
                                             center_x, center_y))
    
    return (center_x, center_y)


  def ensure_size(self, cr):
    if self.__needs_resize_calc:
      
      self.bounds_x1 = self.x
      self.bounds_y1 = self.y
      self.bounds_x2 = self.x + self.width 
      self.bounds_y2 = self.y + self.height

      self.__needs_resize_calc = False
      self.force_redraw()


  def do_simple_paint(self, cr, bounds):

    cr.save()
    self.ensure_size(cr)
    cr.translate(self.x, self.y)
    self._cloud.render_cairo(cr)

    cr.restore()


  def xml_representation(self):
    return ""


  def on_key_press(self, item, target, event):
    return False


  def on_button_press(self, item, target, event):
    canvas = self.get_canvas()

    if canvas.override:
      # if we're in the process of drawing a line, just 
      # propogate the signal.  first fix the coordinates
      canvas = self.get_canvas()
      event.x, event.y = canvas.convert_from_item_space(self, 
                                                        event.x, event.y)
      return False

    canvas.grab_focus(item)
    canvas.grab_highlight(self)

    if event.button == 1:
      self.drag_x = event.x
      self.drag_y = event.y

      fleur = gtk.gdk.Cursor(gtk.gdk.FLEUR)
      canvas = item.get_canvas()
      canvas.pointer_grab(item,
                          gtk.gdk.POINTER_MOTION_MASK 
                           | gtk.gdk.BUTTON_RELEASE_MASK,
                          fleur, event.time)
      self.dragging = True
    elif event.button == 3:
      # right-click, handle later
      pass
    else:
      print "unsupported button: %d" % event.button
    return True


  def on_button_release(self, item, target, event):
    canvas = item.get_canvas()
    canvas.pointer_ungrab(item, event.time)
    self.dragging = False


  def on_motion_notify (self, item, target, event):
    if (self.dragging == True) and (event.state & gtk.gdk.BUTTON1_MASK):
      new_x = event.x
      new_y = event.y
      item.translate(new_x - self.drag_x, new_y - self.drag_y)
      self.emit("item_moved_event", self)
      return True
    return False


  def on_focus_in(self, item, target, event):
    return False


  def on_focus_out(self, item, target, event):
    return False


  def on_highlight_in(self, item, target):
    self.text_color = [1, .6, .2]
    self.force_redraw()

    return False


  def on_highlight_out(self, item, target):
    self.text_color = [0, 0, 0]
    self.force_redraw()

    self._new = False

    return False



gobject.type_register(CloudItem)

