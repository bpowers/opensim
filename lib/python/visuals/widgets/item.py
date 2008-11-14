#===--- item.py - OpenSim Python model initialization ---------------------===#
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
# This file contains the item other canvas items are based off of
#
#===-----------------------------------------------------------------------===#

import gobject
import gtk
import goocanvas
import math

import logging

from text import TextInfo


class SimItem(goocanvas.ItemSimple, goocanvas.Item):

  __gsignals__ = dict(highlight_in_event=(gobject.SIGNAL_RUN_FIRST,
                                          gobject.TYPE_NONE,
                                          (gobject.TYPE_OBJECT,)),
                      highlight_out_event=(gobject.SIGNAL_RUN_FIRST,
                                           gobject.TYPE_NONE,
                                           (gobject.TYPE_OBJECT,)),
                      item_moved_event=(gobject.SIGNAL_RUN_FIRST,
                                        gobject.TYPE_NONE,
                                        (gobject.TYPE_OBJECT,)))


  # space between the bounding box and the text
  padding = 4

  icon_size = 55

  left_key = ['Left', 'KP_Left']
  right_key = ['Right', 'KP_Right']
  escape_key = ['Escape']
  enter_key = ['Return', 'Control_R']
  delete_key = ['BackSpace']


  def __init__(self, x=10, y=10, width=120, height=80, **kwargs):
    super(SimItem, self).__init__(**kwargs)
    self.x = int(x - width/2)
    self.y = int(y - height/2)
    self.width = width
    self.height = height

    self.influences = []
    self.influenced = []
    
    self.named = False

    self.connect("focus_in_event", self.on_focus_in)
    self.connect("focus_out_event", self.on_focus_out)
    self.connect("key_press_event",  self.on_key_press)
    self.connect("button_press_event", self.on_button_press)
    self.connect("button_release_event", self.on_button_release)
    self.connect("motion_notify_event", self.on_motion_notify)
    self.connect("highlight_in_event", self.on_highlight_in)
    self.connect("highlight_out_event", self.on_highlight_out)


  def get_influences(self):
    '''
    Returns a list of the variables that influence this one. 

    This list of influences is the _visual_ ones, the variables
    that have flows or links ending in the current variable,
    not necessarily the ones referenced in the equation.
    '''

    var_influences = []
    for line in self.influences:
      var_influences.append(line.flow_from)
    return var_influences


  def add_influence(self, line):
    self.influences.append(line)


  def remove_influence(self, line):
    self.influences.remove(line)


  def get_influenced(self):
    '''
    Returns a list of the variables this one influences. 

    This list of influenced variables are the _visual_ ones, 
    the variables that have flows or links starting in the 
    current variable, not necessarily the ones referenced in 
    the equation.
    '''

    var_influenced = []
    for line in self.influenced:
      var_influenced.append(line.flow_to)
    return var_influenced


  def add_influenced(self, line):
    self.influenced.append(line)


  def remove_influenced(self, line):
    self.inflienced.remove(line)


  def do_simple_create_path(self, cr):
    cr.rectangle(self.x, self.y, self.width, self.height)


  def do_simple_paint(self, cr, bounds):
    raise NotImplementedError


  def remove(self):
    super(SimItem, self).remove()


  def do_simple_is_item_at(self, x, y, cr, is_pointer_event):
    if ((x < self.x) or (x > self.x + self.width)) or \
       ((y < self.y) or (y > self.y + self.height)):
      return False
    else:    
      return True


  def force_redraw(self):
    # tell the canvas to redraw the area we're in
    self.get_canvas().request_update()
    self.get_canvas().request_redraw(self.get_bounds())


  def name(self):
    raise NotImplementedError


  def xml_representation(self):
    raise NotImplementedError


  def on_property_change(self, widget, event):
    raise NotImplementedError
  

  def on_key_press(self, item, target, event):
    raise NotImplementedError


  def on_button_press(self, item, target, event):
    raise NotImplementedError


  def on_button_release(self, item, target, event):
    raise NotImplementedError


  def on_motion_notify (self, item, target, event):
    raise NotImplementedError


  def on_focus_in(self, item, target, event):
    raise NotImplementedError


  def on_focus_out(self, item, target, event):
    raise NotImplementedError


  def on_highlight_in(self, item, target):
    raise NotImplementedError


  def on_highlight_out(self, item, target):
    self.active_color = [0, 0, 0]

    if self._new:
      if self._display_name.placeholder:
        self.get_canvas().remove_item(self)
        return

    if self.named is True:
      if self.var is None: 
        self.var = self.get_canvas().new_variable(self.name())
      else:
        self.var.props.name = self.name()

    self._new = False
    self.force_redraw()

    return False



gobject.type_register(SimItem)

