import gobject
import gtk
import goocanvas
import math

import logging

from text import TextInfo


class SimItem(goocanvas.ItemSimple, goocanvas.Item):

  # space between the bounding box and the text
  padding = 4

  left_key = ['Left', 'KP_Left']
  right_key = ['Right', 'KP_Right']
  escape_key = ['Escape']
  enter_key = ['Return', 'Control_R']
  delete_key = ['BackSpace']


  def __init__(self, x=10, y=10, width=120, 
               height=80, **kwargs):

    ## chain to parent constructor
    super(SimItem, self).__init__(**kwargs)

    self.x = x
    self.y = y
    self.width = width
    self.height = height

    self.connect("focus_in_event", self.on_focus_in)
    self.connect("focus_out_event", self.on_focus_out)
    self.connect("key_press_event",  self.on_key_press)
    self.connect("button_press_event", self.on_button_press)
    self.connect("button_release_event", self.on_button_release)
    self.connect("motion_notify_event", self.on_motion_notify)


  # optional methods

  def do_get_bounds(self):
    return self.bounds


  def do_get_item_at(self, x, y, cr, is_pointer_event, parent_is_visible):
    print("checking stuff...")
    if ((x < self.x) or (x > self.x + self.width)) or \
       ((y < self.y) or (y > self.y + self.height)):
      return False
    else:  
      return True


  # mandatory methods

  def do_update(self, entire_tree, cr):
    raise NotImplementedError


  def do_paint(self, cr, bounds, scale):
    raise NotImplementedError


  # custom methods

  def force_redraw(self):
    # tell the canvas to redraw the area we're in
    self.get_canvas().request_redraw(self.get_bounds())
  

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



gobject.type_register(SimItem)
