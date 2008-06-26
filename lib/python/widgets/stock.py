import gobject
import gtk
import goocanvas
import math

import logging

from text import TextInfo
from item import SimItem


class StockItem(SimItem):

  def __init__(self, x, y, width=120, height=80, line_width=3.5, **kwargs):
    print("crap")
    super(StockItem, self).__init__(**kwargs)
    print("egadz!")
    self.x = int(x - width/2)
    self.y = int(y - height/2)
    self.width = width
    self.height = height
    self.__needs_resize_calc = True
    self.dragging = False
    self.text_color = [0, 0, 0]

    self.line_width = line_width

    print("hj")
    self._stocks_name = TextInfo("(enter name)", \
                                  dpi=self.get_canvas().dpi, \
                                  placeholder_text=True)

    print("man")
    self.get_canvas().grab_focus(self)
    print("well...")


  def ensure_size(self, cr):
    if self.__needs_resize_calc:
      self._stocks_name.update_extents(cr)

      old_center_x = self.x + self.width/2.0
      old_center_y = self.y + self.height/2.0
      self.width = max(self.width, \
                       self._stocks_name.width + 2*self.padding)
      self.height = max(self.height, \
                        self._stocks_name.height + 2*self.padding)
      self.x = old_center_x - self.width/2.0
      self.y = old_center_y - self.height/2.0
      self.__needs_resize_calc = False


  def do_update(self, entire_tree, cr):
    self.ensure_size(cr)

  def do_paint(self, cr, bounds, scale):

    self.ensure_size(cr)
    cr.rectangle(self.x, self.y, self.width, self.height)
    cr.set_source_rgb (1, 1, 1)
    cr.fill_preserve()
    cr.set_line_width(self.line_width)
    cr.set_source_rgb(self.text_color[0], \
                      self.text_color[1], \
                      self.text_color[2])
    cr.stroke()

    # translate so that our coordinate system is in the widget
    cr.translate(self.x, self.y)
    
    cr.move_to(self.padding, self.height/2.0 + self._stocks_name.height/2.0)
    cr.select_font_face(self._stocks_name.font_face)
    cr.set_font_size(self._stocks_name.font_size)
    cr.show_text(self._stocks_name.string)


  def on_key_press(self, item, target, event):
    key_name = gtk.gdk.keyval_name(event.keyval)

    if key_name in self.enter_key:
      print("enter key!")
    elif key_name in self.delete_key:
      self._stocks_name.backspace()
      self.__needs_resize_calc = True
      self.force_redraw()
    elif key_name in self.escape_key:
      print("escape key!")
    else:
      # add key to name buffer
      self._stocks_name.add(event.string)
      self.__needs_resize_calc = True
      self.force_redraw()

    # return true to stop propogation
    return True


  def on_button_press(self, item, target, event):
    self.get_canvas().grab_focus(item)
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
    print("awes, got focus %s", item)

    self.text_color = [1, .6, .2]
    self.force_redraw()

    return False

  def on_focus_out(self, item, target, event):
    print("aww, left focus %s", item)

    self.text_color = [0, 0, 0]
    self.force_redraw()

    return False

gobject.type_register(StockItem)
