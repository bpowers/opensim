#===--- variable.py - OpenSim variable widget -----------------------------===#
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
# This file contains the implementation of the variable widget, used to
# represent auxilliary variables on the canvas.
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

LINE_HEIGHT = 2
ICON_SIZE = 40

class VariableItem(Element):

  __gtype_name__ = 'VariableItem'

  def __init__(self, name, x, y, width, height, obj_id, var=None):
    super(StockItem, self).__init__()

    # this will be the variable created in the simulator
    self.var = var

    self.obj_id = obj_id
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
    center_x, center_y = self.abs_center()
    
    line_angle = math.atan2((end_point[1] - center_y), 
                            (end_point[0] - center_x))
    if line_angle < 0: line_angle = 2*math.pi + line_angle
    
    
    radius = self.icon_size/2
 
    center_x = center_x + radius * math.cos(line_angle)
    center_y = center_y + radius * math.sin(line_angle)
    
    return (center_x, center_y)


  def ensure_size(self, cr):
    if self.__needs_resize_calc:
      self._display_name.update_extents(cr)

      old_center_x = self.x + self.width/2.0
      old_center_y = self.y + self.height/2.0
      self.height = max(self.icon_size, self._display_name.height) \
                    + 2*self.padding
      self.x = old_center_x - self.width/2.0
      self.y = old_center_y - self.height/2.0

      self.bounds_x1 = self.x - self.line_width/2.0 
      self.bounds_y1 = self.y - self.line_width/2.0
      self.bounds_x2 = self.x + self.width + self.line_width \
                       + 2*self.padding
      self.bounds_y2 = self.y + self.height + self.line_width \
                       + 2*self.padding

      self.__needs_resize_calc = False
      self.force_redraw()


  def pre_update(self, context):
    cr = context.cairo
    self._display_name.width = self.width - self.padding*2
    self._display_name.update_extents(cr)

    self.height = max(self.height, self._display_name.height + 2*self.padding)
    self.width = max(self.width, self._display_name.text_width + 2*self.padding)


  def do_simple_paint(self, cr, bounds):

    cr.save()

    # keep track of the transformation matrix, so we can save 
    # the right coordinates
    matrix = cr.get_matrix()

    self.ensure_size(cr)
    
    self.type_icon(cr)

    center = self.center()
    cr.translate(int(center[0] + self.icon_size/2 + self.padding), center[1])

    # white background for text
    cr.rectangle(-self._display_name.width/2.0, 
                 -self._display_name.height/2.0,
                 self._display_name.text_width, 
                 self._display_name.height)
    cr.set_source_rgba(1, 1, 1, .8)
    cr.fill()

    cr.set_source_rgb(self.active_color[0], \
                      self.active_color[1], \
                      self.active_color[2]) 

    self._display_name.show_text(cr)

    cr.restore()


  def type_icon(self, cr):
    cr.move_to(self.x + self.icon_size, self.y + .5*self.height)
    cr.arc(self.x + .5*self.icon_size, self.y + .5*self.height, 
           self.icon_size/2, 0, 2*math.pi)
    cr.close_path()
    cr.set_source_rgb (1, 1, 1)
    cr.fill_preserve()
    cr.set_line_width(self.line_width)
    cr.set_source_rgb(self.active_color[0], \
                      self.active_color[1], \
                      self.active_color[2])
    cr.stroke()

  def name(self):
    return self._display_name.string

