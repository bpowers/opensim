import gobject
import gtk
import goocanvas
import math

import logging

from text import TextInfo


class SimItem(gobject.GObject, goocanvas.Item):

  __gproperties__ = {
    'title': (str, None, None, '', gobject.PARAM_READWRITE),
    'description': (str, None, None, '', gobject.PARAM_READWRITE),
    'can-focus': (bool, None, None, False, gobject.PARAM_READWRITE),
    'visibility-threshold': (float, None, None, 0, 10e6, 0, gobject.PARAM_READWRITE),
    'visibility': (goocanvas.ItemVisibility, None, None, goocanvas.ITEM_VISIBLE, gobject.PARAM_READWRITE),
    'pointer-events': (goocanvas.PointerEvents, None, None, goocanvas.EVENTS_NONE, gobject.PARAM_READWRITE),
    'transform': (goocanvas.TYPE_CAIRO_MATRIX, None, None, gobject.PARAM_READWRITE),
    'parent': (gobject.GObject, None, None, gobject.PARAM_READWRITE),
    }


  # space between the bounding box and the text
  padding = 4

  left_key = ['Left', 'KP_Left']
  right_key = ['Right', 'KP_Right']
  escape_key = ['Escape']
  enter_key = ['Return', 'Control_R']
  delete_key = ['BackSpace']


  def __init__(self, x=10, y=10, width=120, 
               height=80, parent=None, **kwargs):
    self.bounds = goocanvas.Bounds()
    self.view = None
    self.parent = parent

    ## chain to parent constructor
    gobject.GObject.__init__(self, **kwargs)

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


  def do_set_parent(self, parent):
    assert self.parent is None
    self.parent = parent


  def do_set_property(self, pspec, value):
    if pspec.name == 'title':
      self.title = value
    elif pspec.name == 'description':
      self.description = value
    elif pspec.name == 'can-focus':
      self.can_focus = value
    elif pspec.name == 'visibility':
      self.visibility = value
    elif pspec.name == 'visibility-threshold':
      self.visibility_threshold = value
    elif pspec.name == 'pointer-events':
      self.pointer_events = value
    elif pspec.name == 'transform':
      self.transform = value
    elif pspec.name == 'parent':
      self.parent = value
    else:
      raise AttributeError, 'unknown property %s' % pspec.name
      
  
  def do_get_property(self, pspec):
    if pspec.name == 'title':
      return self.title
    elif pspec.name == 'description':
      return self.description
    elif pspec.name == 'can-focus':
      return self.can_focus
    elif pspec.name == 'visibility':
      return self.visibility
    elif pspec.name == 'visibility-threshold':
      return self.visibility_threshold
    elif pspec.name == 'pointer-events':
      return self.pointer_events
    elif pspec.name == 'transform':
      return self.transform
    elif pspec.name == 'parent':
      return self.parent
    else:
      raise AttributeError, 'unknown property %s' % pspec.name


  # optional methods

  def do_get_bounds(self):
    return self.bounds


  def do_get_item_at(self, x, y, cr, is_pointer_event, parent_is_visible):
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
