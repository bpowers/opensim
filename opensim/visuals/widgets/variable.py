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
import cairo, pango

import logging

from opensim.visuals.tools import edit_equation
from text import TextInfo
from item import SimItem

class VariableItem(SimItem):

  def __init__(self, x, y, width=200, height=80, name=None,
               focus=True, line_width=3.5, **kwargs):
    super(VariableItem, self).__init__(**kwargs)
    self.x = int(x - width/2)
    self.y = int(y - height/2)
    self.width = width
    self.height = height
    self.__needs_resize_calc = True
    self.dragging = False
    self.active_color = [0, 0, 0]
    self.__old_name = ""
    self.named = True

    self._new = True
    # this will be the variable created in the simulator
    self.var = None

    self.line_width = line_width

    text_width = self.width - self.padding*2 - self.icon_size

    if name is not None:
      self._display_name = TextInfo(name, wrap_width=text_width, 
                                    align=pango.ALIGN_LEFT, 
                                    placeholder_text=False)
    else:
      self._display_name = TextInfo("(enter name)", wrap_width=text_width, 
                                    align=pango.ALIGN_LEFT, 
                                    placeholder_text=True)

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


  def center(self):
    return (int(self.x + self.width/2), int(self.y + self.height/2))


  def abs_center(self):
    center = self.center()
    transform = self.get_transform()
    x0, y0 = 0, 0
    if transform is not None:
      xx, yx, xy, yy, x0, y0 = self.get_transform()
    return (x0 + self.x + self.icon_size/2.0, y0 + center[1])


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


  def xml_representation(self):
    # get the center of the widget, so that we get the correct 
    # behavior when it loads.  also, add the cairo transformation
    # matrix offset.
    x_center = self.bounds_x1 + self.width/2
    y_center = self.bounds_y1 + self.height/2

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
    canvas = self.get_canvas()

    if canvas.override:
      # if we're in the process of drawing a line, just 
      # propogate the signal.  first fix the coordinates
      canvas = self.get_canvas()
      event.x, event.y = canvas.convert_from_item_space(self, 
                                                        event.x, event.y)
      return False

    canvas.grab_focus(item)
    logging.debug("**before grab")
    canvas.grab_highlight(self)
    logging.debug("**after grab")

    if event.button is 1:
      self.drag_x = event.x
      self.drag_y = event.y

      fleur = gtk.gdk.Cursor(gtk.gdk.FLEUR)
      canvas = item.get_canvas()
      canvas.pointer_grab(item,
                          gtk.gdk.POINTER_MOTION_MASK 
                           | gtk.gdk.BUTTON_RELEASE_MASK,
                          fleur, event.time)
      self.dragging = True
    elif event.button is 3:
      edit_equation(self.var, self.get_influences())
      canvas.drop_highlight()
    else:
      print "unsupported button: %d" % event.button

    return True


  def on_button_release(self, item, target, event):
    if event.button is 1:
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
    canvas = self.get_canvas()
    event.x, event.y = canvas.convert_from_item_space(self, event.x, event.y)
    return False


  def on_focus_in(self, item, target, event):
    return False


  def on_focus_out(self, item, target, event):
    return False


  def on_highlight_in(self, item, target):
    self.active_color = [1, .6, .2]
    self.force_redraw()

    logging.debug("h_in : '%s'" % self.name())

    self.__old_name = self.name()

    return False



gobject.type_register(VariableItem)

