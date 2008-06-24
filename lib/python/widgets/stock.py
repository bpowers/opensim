import gobject
import gtk
import goocanvas
import math

import logging

from text import TextInfo


class StockItem(goocanvas.ItemSimple, goocanvas.Item):

  # space between the bounding box and the text
  padding = 4

  def __init__(self, x, y, width=120, height=80, line_width=3.5, **kwargs):
    super(StockItem, self).__init__(**kwargs)
    self.x = int(x - width/2)
    self.y = int(y - height/2)
    self.width = width
    self.height = height
    self.__needs_resize_calc = True

    self.line_width = line_width

    self._display_name = TextInfo("Rabbit Population Model")


  def do_simple_create_path(self, cr):
    if self.__needs_resize_calc:
      self._display_name.update_extents(cr)

      old_center_x = self.x + self.width/2
      old_center_y = self.y + self.height/2
      self.width = max(self.width, \
                       self._display_name.width + 2*self.padding)
      self.height = max(self.height, \
                        self._display_name.height + 2*self.padding)
      self.x = old_center_x - self.width/2
      self.y = old_center_y - self.height/2
      self.__needs_resize_calc = False

    # define the bounding path here.
    cr.rectangle(self.x, self.y, self.width, self.height)


  def do_simple_paint(self, cr, bounds):

    self.do_simple_create_path(cr)
    cr.set_source_rgb (1, 1, 1)
    cr.fill_preserve()
    cr.set_line_width(self.line_width)
    cr.set_source_rgb(0, 0, 0)
    cr.stroke()

    # translate so that our coordinate system is in the widget
    cr.translate(self.x, self.y)
    
    cr.move_to(self.padding, self.height/2 + self._display_name.height/2)
    cr.select_font_face(self._display_name.font_face)
    cr.set_font_size(self._display_name.font_size)
    cr.show_text(self._display_name.string)


  def do_simple_is_item_at(self, x, y, cr, is_pointer_event):
    if ((x < self.x) or (x > self.x + self.width)) or \
       ((y < self.y) or (y > self.y + self.height)):
      return False
    else:    
      return True

  def do_button_press_event(self, target, event):
    print "stock button press!"

gobject.type_register(StockItem)
