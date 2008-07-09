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
from item import SimItem


class CloudItem(SimItem):

  def __init__(self, x, y, **kwargs):
    super(CloudItem, self).__init__(**kwargs)

    logging.debug("oh yea TOTALLY clouding it up")

    self._new = True
    self.dragging = False

    self.width = 55
    self.height = 55

    # keep track of inflows and outflows, for use in engine
    self.inflows = []
    self.outflows = []

    self._icon = gtk.Image()
    self._icon = self._icon.set_from_file('opensim-cloud')

    self.__needs_resize_calc = True
    self.force_redraw()


  def do_simple_create_path(self, cr):
    self.ensure_size(cr)

    # define the bounding path here.
    cr.rectangle(self.x, 
                 self.y,
                 self.x + self.width, 
                 self.y + self.height)


  def center(self):
    return (self.x, self.y)


  def ensure_size(self, cr):
    if self.__needs_resize_calc:
      
      self.bounds_x1 = float(self.x)
      self.bounds_y1 = float(self.y)
      self.bounds_x2 = float(self.x + self.width)
      self.bounds_y2 = float(self.y + self.height)

      self.__needs_resize_calc = False
      self.force_redraw()


  def do_simple_paint(self, cr, bounds):

    logging.debug("DRAWING START")
    cr.save()
    center = self.center()
    cr.translate(center[0]-self.width()/2, center[1]-self.height/2)
    
    pcctx = pangocairo.CairoContext(cr)
    gcr = gtk.gdk.CairoContext(gccxt)

    pixbuf = self._icon.get_pixbuf()
    gcr.set_source_pixbuf(pixbuf, 0, 0)
    gcr.paint()

    cr.restore()
    logging.debug("DRAWING END")


  def xml_representation(self):
    xml_string = '\
    <cloud>\n\
      <x>%d</x>\n\
      <y>%d</y>\n\
    </cloud>\n' % (self.x, self.y)

    return xml_string


  def name(self):
    return self._display_name.string


  def on_key_press(self, item, target, event):
    return False


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

    return False


  def on_highlight_out(self, item, target):
    self.text_color = [0, 0, 0]
    self.force_redraw()

    if self._new:
      if self._display_name.placeholder:
        self.get_canvas().remove_item(self)
        return

    self._new = False

    return False



gobject.type_register(CloudItem)
