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
import cairo
import pango
import math

from gaphas.item import Item
from gaphas.connector import Handle, LinePort
from gaphas.geometry import Rectangle
from gaphas.state import observed, reversible_property

import logging

from opensim.visuals.tools import edit_equation
from text import TextInfo

LINE_WIDTH = 2
ICON_SIZE = 55
PADDING = 5

class VariableItem(Item):

  __gtype_name__ = 'VariableItem'

  def __init__(self, name, x, y, width, height, var=None):
    '''
    Constructor for our VariableItem.

    We have to do a lot more here than for our StockItem because I
    decided to get fancy with custom handles.  It was good learning
    experience with graphas.
    '''
    super(VariableItem, self).__init__()

    # the 0th handle is always at the item's origin (0,0)
    h_0 = Handle(movable=False)
    h_l = Handle(movable=False)
    h_r = Handle()
    # we don't want to be able to see or interact with either of these,
    # they're basically for positioning the h_r
    h_0.visible = False
    h_l.visible = False

    self._handles = (h_0, h_l, h_r)
    self.handle = h_r
    self._ports = (LinePort(h_0.pos, h_l.pos),
                   LinePort(h_l.pos, h_r.pos))

    # setup constraints
    self.constraint(above=(h_0.pos, h_l.pos), delta=ICON_SIZE/2)
    self.constraint(above=(h_l.pos, h_0.pos), delta=-ICON_SIZE/2)
    self.constraint(horizontal=(h_l.pos, h_r.pos))
    # create minimal width constraint
    self._c_min_w = self.constraint(left_of=(h_l.pos, h_r.pos), delta=ICON_SIZE)

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

    text_width = self.width - PADDING - ICON_SIZE

    self._display_name = TextInfo(name, wrap_width=text_width,
                                    align=pango.ALIGN_LEFT)

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

    radius = ICON_SIZE/2
    center_x = center_x + radius * math.cos(line_angle)
    center_y = center_y + radius * math.sin(line_angle)

    return (center_x, center_y)


  def pre_update(self, context):
    cr = context.cairo
    self._display_name.width = self.width - PADDING*2 - ICON_SIZE
    self._display_name.update_extents(cr)
    new_width = self._display_name.text_width + PADDING + ICON_SIZE

    self.height = max(self.height, ICON_SIZE)
    self.width = max(self.width, new_width)


  def draw(self, context):
    cr = context.cairo

    self.type_icon(context)

    center = self.center()
    cr.translate(ICON_SIZE + PADDING, self.height/2)
    self._display_name.show_text(cr)


  def type_icon(self, context):
    cr = context.cairo

    cr.save()
    # draw our circle of diameter ICON_SIZE
    cr.translate(.5*ICON_SIZE, .5*self.height)
    cr.scale(.5*ICON_SIZE, .5*ICON_SIZE)
    cr.move_to(1.0, 0.0)
    cr.arc(0.0, 0.0, 1.0, 0.0, 2.0 * math.pi)
    cr.restore()

    # give some visual clues as to what our state is
    if context.focused:
      cr.set_source_rgba(.8,.8,1, .8)
    elif context.hovered:
      cr.set_source_rgba(.9,.9,1, .8)
    else:
      cr.set_source_rgba(1,1,1, .8)
    cr.close_path()
    cr.fill_preserve()
    cr.set_line_width(self.line_width)
    cr.set_source_rgb(self.active_color[0], \
                      self.active_color[1], \
                      self.active_color[2])
    cr.stroke()


  def name(self):
    return self._display_name.string

  @observed
  def _set_min_width(self, min_width):
    """
    Set minimal width.
    """
    if min_width < 0:
      raise ValueError, 'Minimal width cannot be less than 0'

    self._c_min_w.delta = min_width
    if self.canvas:
      self.canvas.solver.request_resolve_constraint(self._c_min_w)

  min_width = reversible_property(lambda s: s._c_min_w.delta, _set_min_width)

  def _set_width(self, width):
    """
    Set our width.
    """
    if width < self.min_width:
      width = self.min_width
    self.handle.x = width

  def _get_width(self):
    """
    Width of the box, calculated as the distance from the left and
    right handle.
    """
    return float(self.handle.x)

  width = property(_get_width, _set_width)

  def _set_height(self, height):
    """
    Set the height.
    """
    self.__height = height

  def _get_height(self):
    """
    Variable's height.  Min ICON_SIZE, max depending on text height.
    """
    return self.__height

  height = property(_get_height, _set_height)

