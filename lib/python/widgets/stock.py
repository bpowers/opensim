import gobject
import gtk
import goocanvas
import math

import logging

import text


class StockItem(goocanvas.ItemSimple, goocanvas.Item):

  def __init__(self, x, y, width=120, height=80, line_width=3.5, **kwargs):
    super(StockItem, self).__init__(**kwargs)
    self.x = int(x - width/2)
    self.y = int(y - height/2)
    self.width = width
    self.height = height
    self.__needs_resize_calc = True

    self.line_width = line_width

    self._display_name = Text("Rabbit Population Model")


  def do_simple_create_path(self, cr):
    if self.__needs_resize_calc:
      cr.select_font_face('Arial')
      cr.set_font_size(18)
      (x, y, width, height, dx, dy) = \
         cr.text_extents(self._display_name.string)
      #print("x:%f y:%f w:%f h:%f dx:%f dy:%f" % (x, y, width, height, dx, dy))
      ## define the bounding path here.
      old_center_x = self.x + self.width/2
      old_center_y = self.y + self.height/2
      self.width = 2*max(self.width, width)
      self.height = 2*max(self.height, height)
      self.x = old_center_x - self.width/2
      self.y = old_center_y - self.height/2
      self.__needs_resize_calc = False

    cr.rectangle(self.x, self.y, self.width, self.height)


  def do_simple_paint(self, cr, bounds):

    self.do_simple_create_path(cr)
    cr.set_source_rgb (1, 1, 1)
    cr.fill_preserve()
    cr.set_line_width(self.line_width)
    cr.set_source_rgb(0, 0, 0)
    cr.stroke()
    cr.translate(self.x, self.y)
    cr.set_source_rgba(1, 0.2, 0.2, 0.6)
    #cr.move_to(self.width/2,self.height/2)
    cr.arc(self.width/2,self.height/2, 10, 0, 2*math.pi);
    cr.close_path()
    cr.fill()
    cr.select_font_face('sans-serif')
    cr.set_font_size(50)
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
