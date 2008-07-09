#===--- independent.py - OpenSim abstract widget --------------------------===#
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
# This file contains the an abstract widget which contains most of
# the functionality of the stock and variable items
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


class IndependentItem(SimItem):
  def __init__(self, **kwargs):
    super(IndependentItem, self).__init__(**kwargs)


  def name(self):
    return self._display_name.string


  def center(self):
    return (int(self.x + self.width/2), int(self.y + self.height/2))


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
    return False


  def on_focus_in(self, item, target, event):
    return False


  def on_focus_out(self, item, target, event):
    return False


  def on_highlight_in(self, item, target):
    self.text_color = [1, .6, .2]
    self.force_redraw()
    logging.debug("got highlight '%s'" % self.name())
    return False


  def on_highlight_out(self, item, target):
    logging.debug("lost highlight '%s'" % self.name())
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



gobject.type_register(IndependentItem)

