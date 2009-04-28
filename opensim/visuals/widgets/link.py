#===--- link.py - OpenSim Link widget -------------------------------------===#
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
# This file contains the implementation of the link widget
#
#===-----------------------------------------------------------------------===#

import gobject
import gtk
import goocanvas
import math
from math import pi
import cairo

import logging

from text import TextInfo
from item import SimItem
from stock import StockItem
from cloud import CloudItem


class LinkItem(SimItem):

  __g_type_name__ = 'LinkItem'

  def __init__(self, flow_from=None, start=None, end=None, 
               dragging=True, focus=True, line_width=3, **kwargs):
    super(LinkItem, self).__init__(**kwargs)

    self.__needs_resize_calc = True
    self.dragging = dragging
    self.active_color = [0, 0, 0]
    self.arrow_size = 20

    if flow_from:
      start_coord = flow_from.abs_center()  
      self.x1 = start_coord[0]
      self.y1 = start_coord[1]

      self.x2 = start_coord[0]
      self.y2 = start_coord[1]

      # keep track of where we're coming from, even if its a cloud.
      self.flow_from = flow_from

      #now make sure we update our endpoints when the targets move
      self.__start_cb = self.flow_from.connect("item_moved_event", 
                                               self.update_point)
    else:
      if not start or not end:
        logging.error("flow_from and start or end undefined!")
        return
      
      self.x1, self.y1 = start
      self.x2, self.y2 = end
      self.flow_from = None

    self.flow_to = None
    self._new = True
    self.line_width = line_width

    if focus:
      self.get_canvas().grab_focus(self)
      self.get_canvas().grab_highlight(self)


  def remove(self):
    # get rid of clouds
    if self.flow_from:
      self.flow_from.disconnect(self.__start_cb)

    if self.flow_to:
      self.flow_to.disconnect(self.__end_cb)
    
    super(LinkItem, self).remove()


  def center(self):
    return (int(self.x1 + (self.x2 - self.x1)/2), 
            int(self.y1 + (self.y2 - self.y1)/2))


  def abs_center(self):
    return (self.bounds_x1 + (self.bounds_x2 - self.bounds_x1),
            self.bounds_y1 + (self.bounds_y2 - self.bounds_y1))


  def do_simple_create_path(self, cr):
    self.ensure_size(cr)

    # define the bounding path here.
    cr.rectangle(self.bounds_x1, self.bounds_y1,
                 self.bounds_x2, self.bounds_y2)


  def ensure_size(self, cr):
    if self.__needs_resize_calc:
      self.bounds_x1 = float(min(self.x1, self.x2) - self.arrow_size)
      self.bounds_y1 = float(min(self.y1, self.y2) - self.arrow_size)
      self.bounds_x2 = float(max(self.x1, self.x2) + self.arrow_size)
      self.bounds_y2 = float(max(self.y1, self.y2) + self.arrow_size)

      #self._display_name.update_extents(cr)
      self.__needs_resize_calc = False
      self.force_redraw()


  def do_simple_paint(self, cr, bounds):

    cr.save()
    self.ensure_size(cr)
    cr.move_to(self.x1, self.y1)
    
    angle = math.atan2(self.y2-self.y1, self.x2-self.x1)
    end_x = self.x2 - math.cos(angle) * self.arrow_size
    end_y = self.y2 - math.sin(angle) * self.arrow_size
    
    cr.line_to(end_x, end_y)
    cr.set_line_width(self.line_width)
    cr.set_source_rgb(self.active_color[0], \
                      self.active_color[1], \
                      self.active_color[2])
    cr.stroke()

    # draw arrow
    cr.move_to(self.x2, self.y2)
    # 10/pi is 20 degrees to either side of the line
    cr.line_to(self.x2 - math.cos(angle+20.0/180*pi) * self.arrow_size,
               self.y2 - math.sin(angle+20.0/180*pi) * self.arrow_size)
    cr.curve_to(end_x, end_y, end_x, end_y,
                self.x2 - math.cos(angle-20.0/180*pi) * self.arrow_size,
                self.y2 - math.sin(angle-20.0/180*pi) * self.arrow_size)
    cr.close_path()
    cr.set_line_width(self.line_width/1.5)
    cr.set_line_join(cairo.LINE_JOIN_ROUND)
    cr.stroke_preserve()
    cr.fill()


    
    
    cr.restore()


  def set_flow_from(self, flow_from):

    if self.flow_from:
      self.flow_from.remove_influence(self)
      self.flow_from.disconnect(self.__start_cb)

    self.flow_from = flow_from
    self.flow_from.add_influence(self)
    self.x1, self.y1 = self.flow_from.abs_center()
    self._new = False

    #now make sure we update our endpoints when the targets move
    self.__start_cb = self.flow_from.connect("item_moved_event", 
                                             self.update_point)

    self.__needs_resize_calc = True
    self.force_redraw()
    

  def set_flow_to(self, flow_to):

    if self.flow_to:
      self.flow_to.remove_influenced(self)
      self.flow_to.disconnect(self.__end_cb)

    self.flow_to = flow_to
    self.flow_to.add_influenced(self)
    self.x2, self.y2 = self.flow_to.edge_point((self.x1, self.y2))
    self._new = False

    #now make sure we update our endpoints when the targets move
    self.__end_cb = self.flow_to.connect("item_moved_event", 
                                         self.update_point)

    self.dragging = False
    self.__needs_resize_calc = True
    self.force_redraw()


  def update_point(self, item, target):
    if item is self.flow_from:
      self.x1, self.y1 = self.flow_from.abs_center()
    
    self.x2, self.y2 = self.flow_to.edge_point((self.x1, self.y1))

    self.__needs_resize_calc = True
    self.force_redraw()


  def xml_representation(self):
    xml_string = '\
    <link>\n\
      <x1>%d</x1>\n\
      <y1>%d</y1>\n\
      <x2>%d</x2>\n\
      <y2>%d</y2>\n\
      <start>%s</start>\n\
      <end>%s</end>\n\
    </link>\n' % (self.x1, self.y1, 
                  self.x2, self.y2, 
                  self.flow_from.name(), self.flow_to.name())

    return xml_string


  def name(self):
    return "link"


  def do_simple_is_item_at(self, x, y, cr, is_pointer_event):
    self.ensure_size(cr)

    if ((x < self.bounds_x1) or (x > self.bounds_x2)) or \
       ((y < self.bounds_y1) or (y > self.bounds_y2)):
      return False
    else:    
      return True


  def on_key_press(self, item, target, event):
    return True


  def on_button_press(self, item, target, event):
    canvas = self.get_canvas()

    if canvas.override:
      # if we're in the process of drawing a line, just 
      # propogate the signal.
      return False

    canvas.grab_focus(item)
    canvas.grab_highlight(self)

    if event.button == 1:
      # this is where we deal with bending the line
      pass
    elif event.button == 3:
      pass
    else:
      print "unsupported button: %d" % event.button
    return True


  def on_button_release(self, item, target, event):
      pass


  def on_motion_notify (self, item, target, event):
    if self.dragging is True:
      self.x2 = event.x
      self.y2 = event.y
      self.__needs_resize_calc = True
      self.force_redraw()
      return True
    return False


  def on_focus_in(self, item, target, event):
    return False


  def on_focus_out(self, item, target, event):
    return False


  def on_highlight_in(self, item, target):
    self.active_color = [1, .6, .2]
    self.force_redraw()

    return False


  def on_highlight_out(self, item, target):
    self.active_color = [0, 0, 0]

    if self._new:
      self.get_canvas().remove_item(self)
      return False

    self.force_redraw()

    return False

