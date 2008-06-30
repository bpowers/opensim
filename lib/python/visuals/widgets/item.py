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
                                           (gobject.TYPE_OBJECT,)))


  # space between the bounding box and the text
  padding = 4

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

    self.connect("focus_in_event", self.on_focus_in)
    self.connect("focus_out_event", self.on_focus_out)
    self.connect("key_press_event",  self.on_key_press)
    self.connect("button_press_event", self.on_button_press)
    self.connect("button_release_event", self.on_button_release)
    self.connect("motion_notify_event", self.on_motion_notify)
    self.connect("highlight_in_event", self.on_highlight_in)
    self.connect("highlight_out_event", self.on_highlight_out)


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
    self.get_canvas().request_update()
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


  def on_highlight_in(self, item, target):
    raise NotImplementedError


  def on_highlight_out(self, item, target):
    raise NotImplementedError



gobject.type_register(SimItem)
