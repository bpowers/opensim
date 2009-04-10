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
import math
import cairo

from gaphas.item import Element
from gaphas.connector import Handle
from gaphas.geometry import Rectangle

import logging

from opensim.visuals.tools import edit_equation
from text import TextInfo


class StockItem(Element):

  def __init__(self, name=None,
               line_width=2.0):
    super(StockItem, self).__init__()

    self._new = True
    # this will be the variable created in the simulator
    self.var = None

    self.active_color = [0, 0, 0]
    self.line_width = line_width
    self.__old_name = ''
    self.named = True

    self.width = 150
    self.height = 75

    # keep track of inflows and outflows, for use in engine
    self.inflows = []
    self.outflows = []

    self.padding = 5
    text_width = self.width - self.padding*2

    if name is not None:
      self._display_name = TextInfo(name, wrap_width=text_width, 
                                    placeholder_text=False)
    else:
      self._display_name = TextInfo('(enter name)', wrap_width=text_width, 
                                    placeholder_text=True)

    self.__needs_resize_calc = True


  def set_position(self, x, y):
    if (x, y) != self.get_position():
      self.matrix = (1.0, 0.0, 0.0, 1, x, y)

  def get_position(self):
    return self.matrix[4], self.matrix[5]

  position = property(get_position, set_position)


  def center(self):
    return (int(self.width/2), int(self.height/2))


  def abs_center(self):
    center = self.center()
    transform = self.get_transform()
    x0, y0 = 0, 0
    if transform is not None:
      xx, yx, xy, yy, x0, y0 = transform
    return (x0 + center[0], y0 + center[1])


  def edge_point(self, end_point):
    center_x, center_y = self.abs_center()
    
    line_angle = math.atan2((end_point[1] - center_y), 
                            (end_point[0] - center_x))
    if line_angle < 0: line_angle = 2*math.pi + line_angle
    
    # should always be between 0 and .5*pi
    ref_angle = math.atan2(float(self.height),float(self.width))
    
    width = self.width/2
    height = self.height/2
    
    if line_angle < ref_angle or line_angle > 2*math.pi - ref_angle:
      center_x = center_x + width
      center_y = center_y + width * math.tan(line_angle)
    elif line_angle > math.pi - ref_angle and line_angle < math.pi + ref_angle:
      center_x = center_x - width
      center_y = center_y - width * math.tan(line_angle)
    
    if line_angle >= ref_angle and line_angle <= math.pi - ref_angle:
      center_x = center_x - height * math.tan(line_angle - math.pi/2)
      center_y = center_y + height
    elif line_angle >= math.pi + ref_angle and \
         line_angle <= 2*math.pi - ref_angle:
      center_x = center_x + height * math.tan(line_angle - math.pi/2)
      center_y = center_y - height
    
    #logging.debug("line: %5.1f, ref %5.1f" % (math.degrees(line_angle), 
    #                                          math.degrees(ref_angle)))
    
    return (center_x, center_y)


  '''
  def pre_update(self, context):
    cr = context.cairo
    if self.__needs_resize_calc:
      self._display_name.update_extents(cr)

      old_center_x = self.x + self.width/2.0
      old_center_y = self.y + self.height/2.0
      self.height = max(self.height, \
                        self._display_name.height + 2*self.padding)
      self.x = old_center_x - self.width/2.0
      self.y = old_center_y - self.height/2.0
      
      self.bounds_x1 = self.x - self.line_width/2.0 
      self.bounds_y1 = self.y - self.line_width/2.0
      self.bounds_x2 = self.x + self.width + self.line_width/2.0 
      self.bounds_y2 = self.y + self.height + self.line_width/2.0

      self.__needs_resize_calc = False
  '''

  def draw(self, context):
    cr = context.cairo
    cr.save()

    cr.rectangle(0, 0, self.width, self.height)
    cr.set_source_rgb (1, 1, 1)
    cr.fill_preserve()
    cr.set_line_width(self.line_width)
    cr.set_source_rgb(self.active_color[0], \
                      self.active_color[1], \
                      self.active_color[2])
    cr.stroke()
    
    center = self.center()
    cr.translate(center[0], center[1])
    self._display_name.show_text(cr)

    cr.restore()


  def xml_representation(self):
    # get the center of the widget, so that we get the correct 
    # behavior when it loads.  also, add the cairo transformation
    # matrix offset.
    x_center = self.bounds_x1 + self.width/2.0
    y_center = self.bounds_y1 + self.height/2.0

    xml_string = '\
    <stock>\n\
      <name>%s</name>\n\
      <x>%d</x>\n\
      <y>%d</y>\n\
      <width>%f</width>\n\
      <height>%f</height>\n\
    </stock>\n' % (self._display_name.string, x_center, y_center, 
                   self.width, self.height)

    return xml_string


  def name(self):
    return self._display_name.string


  def on_key_press(self, item, target, event):
    key_name = gtk.gdk.keyval_name(event.keyval)

    if key_name in self.enter_key:
      self.emit("highlight_out_event", self)
    elif key_name in self.delete_key:
      self._display_name.backspace()
    elif key_name in self.escape_key:
      print("escape key!")
    else:
      # add key to name buffer
      self._display_name.add(event.string)

    self.__needs_resize_calc = True
    self.force_redraw()

    # return true to stop propogation
    return True


  def get_rectangle(self):
    x, y = self.get_position()
    return Rectangle(x, y, self.width, self.height)



