#===--- rate.py - OpenSim Rate widget -----------------===#
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
# This file contains the implementation of the rate widget
#
#===-----------------------------------------------------------------------===#

import gobject
import gtk
import goocanvas
import math
import cairo

import logging

from text import TextInfo
from item import SimItem
from stock import StockItem
from cloud import CloudItem


class FlowItem(SimItem):

  def __init__(self, flow_from, name=None, 
               dragging=True, focus=True, line_width=3.5, **kwargs):
    super(FlowItem, self).__init__(**kwargs)

    start_coord = flow_from.center()  
    self.x1 = start_coord[0]
    self.y1 = start_coord[1]

    self.x2 = start_coord[0]
    self.y2 = start_coord[1]

    self.__needs_resize_calc = True
    self.dragging = dragging
    self.active_color = [0, 0, 0]

    # keep track of where we're coming from, even if its a cloud.
    self.flow_from = flow_from
    self.flow_to = None

    self._new = True

    self.line_width = line_width

    if name is not None:
      self._display_name = TextInfo(name, 
                                    placeholder_text=False)
    else:
      self._display_name = TextInfo("(new flow)", 
                                    placeholder_text=True)

    if focus:
      self.get_canvas().grab_focus(self)
      self.get_canvas().grab_highlight(self)


  def remove(self):
    # get rid of clouds
    if self.flow_from and type(self.flow_from) is CloudItem:
        self.get_canvas().remove_item(self.flow_from)
    if self.flow_to and type(self.flow_to) is CloudItem:
        self.get_canvas().remove_item(self.flow_to)
    super(FlowItem, self).remove()


  def do_simple_create_path(self, cr):
    self.ensure_size(cr)

    # define the bounding path here.
    cr.rectangle(min(self.x1, self.x2) - self.line_width, 
                 min(self.y1, self.y2) - self.line_width, 
                 max(self.x1, self.x2) + self.line_width, 
                 max(self.y1, self.y2) + self.line_width)


  def ensure_size(self, cr):
    if self.__needs_resize_calc:
      self.bounds_x1 = min(self.x1, self.x2) - self.line_width 
      self.bounds_y1 = min(self.y1, self.y2) - self.line_width
      self.bounds_x2 = max(self.x1, self.x2) + self.line_width
      self.bounds_y2 = max(self.y1, self.y2) + self.line_width
      #self._display_name.update_extents(cr)
      self.__needs_resize_calc = False
      self.force_redraw()


  def do_simple_paint(self, cr, bounds):

    cr.save()
    self.ensure_size(cr)
    cr.move_to(self.x1, self.y1)
    cr.line_to(self.x2, self.y2)
    cr.set_line_width(self.line_width)
    cr.set_source_rgb(self.active_color[0], \
                      self.active_color[1], \
                      self.active_color[2])     
    cr.set_line_cap  (cairo.LINE_CAP_ROUND)
    cr.stroke()

    # print flow name
    #cr.move_to(self.padding, self.height/2.0 + self._display_name.height/2.0)
    #cr.select_font_face(self._display_name.font_face)
    #cr.set_font_size(self._display_name.font_size)
    #cr.show_text(self._display_name.string)
    #cr.restore()


  def set_flow_to(self, flow_to):
    self.flow_to = flow_to
    self.x2, self.y2 = self.flow_to.center()

  def xml_representation(self):
    xml_string = '\
    <flow>\n\
      <name>%s</name>\n\
      <x1>%d</x1>\n\
      <y1>%d</y1>\n\
      <x2>%d</x2>\n\
      <y2>%d</y2>\n\
    </flow>\n' % (self._display_name.string, self.x1, self.y1, 
                   self.x2, self.y2)

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


  def on_button_press(self, item, target, event):
    canvas = self.get_canvas()

    if canvas.override:
      # if we're in the process of drawing a line, just 
      # propogate the signal.
      return False

    canvas.grab_focus(item)
    canvas.grab_highlight(self)
    if event.button == 1:
      pass
    elif event.button == 3:
      # right-click, handle later
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
    self.force_redraw()

    if self._new:
      if self._display_name.placeholder:
        self.get_canvas().remove_item(self)
        return

    self.get_canvas().update_name(self._display_name.string, 
                                  self, new=self._new)

    self._new = False

    return False


gobject.type_register(FlowItem)
