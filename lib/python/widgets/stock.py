import gobject
import gtk
import goocanvas



class StockItem(goocanvas.ItemSimple, goocanvas.Item):

  ## Note to read or modify the bounding box of ItemSimple use
  ## self.bounds_x1,x2,y1,y2

  def __init__(self, x, y, width=120, height=80, line_width=2, **kwargs):
    super(StockItem, self).__init__(**kwargs)
    self.x = int(x - width/2)
    self.y = int(y - height/2)
    self.width = width
    self.height = height
    self.line_width = line_width


  def do_simple_create_path(self, cr):
    ## define the bounding path here.
    cr.rectangle(self.x, self.y, self.width, self.height)


  def do_simple_paint(self, cr, bounds):

    self.do_simple_create_path(cr)
    cr.set_source_rgb (1, 1, 1)
    cr.fill_preserve()
    cr.set_line_width(self.line_width)
    cr.set_source_rgb(0, 0, 0)
    cr.stroke()


  def do_simple_is_item_at(self, x, y, cr, is_pointer_event):
    if ((x < self.x) or (x > self.x + self.width)) or \
       ((y < self.y) or (y > self.y + self.height)):
      return False
    else:    
      return True

  def do_button_press_event(self, target, event):
    print "stock button press!"

gobject.type_register(StockItem)
