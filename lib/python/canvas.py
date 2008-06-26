import pygtk
pygtk.require("2.0")

import gobject, gtk, cairo, pango, goocanvas, math
import constants as sim
import widgets



class SimGoo(goocanvas.Canvas):
  def __init__(self, **kwargs):
    super(SimGoo, self).__init__(**kwargs)
    self.highlighted = None
  

  def grab_highlight(self, widget):
    print("grabbing")
    
    if self.highlighted is widget:
      return
    elif self.highlighted is not None:
      self.highlighted.emit("highlight_out_event", self)
      self.highlighted = None
    self.highlighted = widget
    widget.emit("highlight_in_event", self)


  def drop_highlight(self):
    if self.highlighted is not None:
      self.highlighted.emit("highlight_out_event", self)
      self.highlighted = None



class Canvas (gtk.ScrolledWindow):

  active_tool = sim.UNDEFINED

  def __init__(self):
    super(Canvas, self).__init__()
    self.last_state = 0
    self.dragging = False
    self.drag_x = 0
    self.drag_y = 0

    self.goocanvas = SimGoo()

    display = gtk.gdk.display_get_default()
    screen = display.get_default_screen()
    self.goocanvas.dpi = screen.get_resolution()

    # white background
    color_white = gtk.gdk.Color(65535, 65535, 65535, 0)
    self.goocanvas.modify_base(gtk.STATE_NORMAL, color_white)

    self.goocanvas.set_flags(gtk.CAN_FOCUS)
    self.goocanvas.set_flags(gtk.CAN_DEFAULT)
    self.goocanvas.set_size_request(1440, 900)
    self.goocanvas.set_bounds(0, 0, 1440, 900)

    self.goocanvas.automatic_bounds = True

    root = self.goocanvas.get_root_item()
    root.connect("button_press_event", self.on_background_button_press)

    self.connect("focus_in_event", self.on_focus_in)
    self.connect("focus_out_event", self.on_focus_out)

    self.add(self.goocanvas)

    self.goocanvas.show()


  def set_active_tool(self, tool_type):
    self.active_tool = tool_type


  def get_active_tool(self):
    return self.active_tool


  def on_background_button_press (self, item, target, event):

    if event.button is 1 and self.active_tool is sim.STOCK:
      root = self.goocanvas.get_root_item()
      new_stock = widgets.StockItem(event.x, event.y, \
                                    parent=root, can_focus=True)
    else:
      self.grab_focus()
      self.goocanvas.drop_highlight()
    return True


  def on_focus_in(self, target, event):
    print("awes, got focus %s", self)

    return False

  def on_focus_out(self, target, event):
    print("aww, left focus %s", self)

    return False



gobject.type_register(Canvas)
