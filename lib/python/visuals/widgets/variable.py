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
import goocanvas
import math

import logging

from text import TextInfo
from item import SimItem

class VariableItem(SimItem):

  icon_size = 55

  ## Note to read or modify the bounding box of ItemSimple use
  ## self.bounds_x1,x2,y1,y2

  def __init__(self, x, y, width=120, height=80, name=None,
               focus=True, line_width=3.5, **kwargs):
    super(VariableItem, self).__init__(**kwargs)
    self.x = int(x - width/2)
    self.y = int(y - height/2)
    self.width = width
    self.height = height
    self.__needs_resize_calc = True
    self.dragging = False
    self.text_color = [0, 0, 0]

    self._new = True

    self.line_width = line_width

    if name is not None:
      self._display_name = TextInfo(name, \
                                    dpi=self.get_canvas().dpi, \
                                    placeholder_text=False)
    else:
      self._display_name = TextInfo("(enter name)", \
                                    dpi=self.get_canvas().dpi, \
                                    placeholder_text=True)

    if focus:
      self.get_canvas().grab_focus(self)
      self.get_canvas().grab_highlight(self)


  def do_simple_create_path(self, cr):
    self.ensure_size(cr)
    ## define the bounding path hertocke.
    cr.rectangle(self.x, self.y, self.width, self.height)


  def ensure_size(self, cr):
    if self.__needs_resize_calc:
      self._display_name.update_extents(cr)

      old_center_x = self.x + self.width/2.0
      old_center_y = self.y + self.height/2.0
      self.width = max(self.width, self._display_name.width + \
                                   2*self.padding + self.icon_size)
      self.height = max(self.height, self._display_name.height + \
                                     2*self.padding + self.icon_size)
      self.x = old_center_x - self.width/2.0
      self.y = old_center_y - self.height/2.0
      self.__needs_resize_calc = False




  def do_simple_paint(self, cr, bounds):

    cr.save()

    # keep track of the transformation matrix, so we can save 
    # the right coordinates
    matrix = cr.get_matrix()

    self.ensure_size(cr)
    
    self.type_icon(cr)

    cr.translate(self.x + self.icon_size + self.padding, self.y + self.height/2.0 + \
                                      self._display_name.height/2.0)
    cr.select_font_face(self._display_name.font_face)
    cr.set_font_size(self._display_name.font_size)
    cr.show_text(self._display_name.string)
    cr.restore()


  def type_icon(self, cr):
    cr.move_to(self.x + self.icon_size, self.y + .5*self.height)
    cr.arc(self.x + .5*self.icon_size, self.y + .5*self.height, 
           self.icon_size/2, 0, 2*math.pi)
    cr.close_path()
    cr.set_source_rgb (1, 1, 1)
    cr.fill_preserve()
    cr.set_line_width(self.line_width)
    cr.set_source_rgb(self.text_color[0], \
                      self.text_color[1], \
                      self.text_color[2])
    cr.stroke()


  def xml_representation(self):
    # get the center of the widget, so that we get the correct 
    # behavior when it loads.  also, add the cairo transformation
    # matrix offset.
    x_center = self.bounds_x1 + self.width/2.0
    y_center = self.bounds_y1 + self.height/2.0

    xml_string = '\
    <var>\n\
      <name>%s</name>\n\
      <x>%d</x>\n\
      <y>%d</y>\n\
      <width>%f</width>\n\
      <height>%f</height>\n\
    </var>\n' % (self._display_name.string, x_center, y_center, 
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


  def on_button_press(self, item, target, event):
    self.get_canvas().grab_focus(item)
    self.get_canvas().grab_highlight(self)
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
    return True


  def on_focus_in(self, item, target, event):
    return False


  def on_focus_out(self, item, target, event):
    return False


  def on_highlight_in(self, item, target):
    print("HIGH IN")
    self.text_color = [1, .6, .2]
    self.force_redraw()

    return False


  def on_highlight_out(self, item, target):
    print("HIGH OUT")
    self.text_color = [0, 0, 0]
    self.force_redraw()

    if self._new:
      if self._display_name.placeholder:
        self.get_canvas().remove_item(self)
        return

    self.get_canvas().update_name(self._display_name.string, 
                                  self, new=self._new)

    self._new = False

    return False

gobject.type_register(VariableItem)

