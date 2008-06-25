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
  exit_key = ['Escape']
  enter_key = ['Return', 'Control_R']
  delete_key = ['BackSpace']


  def __init__(self, x=10, y=10, width=120, height=80, **kwargs):
    super(SimItem, self).__init__(**kwargs)
    self.x = int(x - width/2)
    self.y = int(y - height/2)
    self.width = width
    self.height = height

    self.connect("focus_in_event", self.on_focus_in)
    self.connect("focus_out_event", self.on_focus_out)
    self.connect("key_press_event",  self.on_key_press)
    self.connect("button_press_event", self.on_button_press)
    self.connect("button_release_event", self.on_button_release)
    self.connect("motion_notify_event", self.on_motion_notify)


  def do_simple_create_path(self, cr):
    cr.rectangle(self.x, self.y, self.width, self.height)


  def do_simple_paint(self, cr, bounds):
    pass


  def do_simple_is_item_at(self, x, y, cr, is_pointer_event):
    if ((x < self.x) or (x > self.x + self.width)) or \
       ((y < self.y) or (y > self.y + self.height)):
      return False
    else:    
      return True


  def force_redraw(self):
    # tell the canvas to redraw the area we're in
    self.get_canvas().request_redraw(self.get_bounds())
  

  def on_key_press(self, item, target, event):
    pass


  def on_button_press(self, item, target, event):
    pass


  def on_button_release(self, item, target, event):
    pass


  def on_motion_notify (self, item, target, event):
    pass


  def on_focus_in(self, item, target, event):
    pass


  def on_focus_out(self, item, target, event):
    pass



gobject.type_register(SimItem)
