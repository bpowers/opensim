#===--- stock.py - OpenSim Stock widget ----------------------------------===#
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

LINE_WIDTH = 2.0
PADDING = 5.0


class StockItem(Element):

  __gtype_name__ = 'StockItem'

  def __init__(self, name, x, y, width, height, var=None):
    super(StockItem, self).__init__()

    # this will be the variable created in the simulator
    self.var = var

    self.active_color = [0, 0, 0]
    self.line_width = LINE_WIDTH
    self.__old_name = ''

    self.width = width
    self.height = height

    # keep track of inflows and outflows, for use in engine
    self.inflows = []
    self.outflows = []

    self.padding = PADDING
    text_width = self.width - self.padding*2

    if name is not None:
      self._display_name = TextInfo(name, wrap_width=text_width, 
                                    placeholder_text=False)
    else:
      self._display_name = TextInfo('(enter name)', wrap_width=text_width,
                                    placeholder_text=True)

    self.set_position(x - width/2, y - height/2)


  def set_position(self, x, y):
    if (x, y) != self.get_position():
      self.matrix = (1.0, 0.0, 0.0, 1, x, y)

  def get_position(self):
    return self.matrix[4], self.matrix[5]

  position = property(get_position, set_position)


  def center(self):
    return (int(self.width/2), int(self.height/2))


  def edge_point(self, end_point):
    center_x, center_y = self.center()
    
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


  def pre_update(self, context):
    cr = context.cairo
    self._display_name.width = self.width - self.padding*2
    self._display_name.update_extents(cr)

    self.height = max(self.height, self._display_name.height + 2*self.padding)
    self.width = max(self.width, self._display_name.text_width + 2*self.padding)


  def draw(self, context):
    cr = context.cairo
    cr.save()

    cr.rectangle(0, 0, self.width, self.height)

    # give some visual clues as to what our state is
    if context.focused:
      cr.set_source_rgba(.8,.8,1, .8)
    elif context.hovered:
      cr.set_source_rgba(.9,.9,1, .8)
    else:
      cr.set_source_rgba(1,1,1, .8)
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


  def name(self):
    return self._display_name.string


  def get_rectangle(self):
    x, y = self.get_position()
    return Rectangle(x, y, self.width, self.height)



