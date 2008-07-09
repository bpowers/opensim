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
import cairo

import logging

from text import TextInfo
from independent import IndependentItem


class StockItem(IndependentItem):

  def __init__(self, x, y, width=140, height=80, name=None,
               focus=True, line_width=3.5, **kwargs):
    super(StockItem, self).__init__(**kwargs)

    self._new = True

    self.x = int(x - width/2)
    self.y = int(y - height/2)
    self.width = width
    self.height = height
    self.dragging = False

    self.text_color = [0, 0, 0]
    self.line_width = line_width

    # keep track of inflows and outflows, for use in engine
    self.inflows = []
    self.outflows = []

    text_width = self.width - self.padding*2

    if name is not None:
      self._display_name = TextInfo(name, wrap_width=text_width, 
                                    placeholder_text=False)
    else:
      self._display_name = TextInfo("(enter name)", wrap_width=text_width, 
                                    placeholder_text=True)

    self.__needs_resize_calc = True

    if focus:
      self.get_canvas().grab_focus(self)
      self.get_canvas().grab_highlight(self)


  def do_simple_create_path(self, cr):
    self.ensure_size(cr)

    # define the bounding path here.
    cr.rectangle(self.x - self.line_width/2.0, 
                 self.y - self.line_width/2.0,
                 self.width + self.line_width/2.0, 
                 self.height + self.line_width/2.0)


  def ensure_size(self, cr):
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
      self.force_redraw()


  def do_simple_paint(self, cr, bounds):

    cr.save()
    self.ensure_size(cr)
    cr.rectangle(self.x, self.y, self.width, self.height)
    cr.set_source_rgb (1, 1, 1)
    cr.fill_preserve()
    cr.set_line_width(self.line_width)
    cr.set_source_rgb(self.text_color[0], \
                      self.text_color[1], \
                      self.text_color[2])
    cr.stroke()

    # translate so that our coordinate system is in the widget
    
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



gobject.type_register(StockItem)

