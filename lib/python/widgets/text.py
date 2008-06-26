

class TextInfo():

  __base_dpi = 96.0

  def __init__(self, string, dpi=96, font_face='sans-serif', \
               font_size=14, placeholder_text=False, cr=None):
    self.string = string
    self.font_face = font_face
    self._dpi = dpi
    self.scale = self._dpi/self.__base_dpi
    self.font_size = font_size * self.scale
    self.font_size_unscaled = font_size
    self.placeholder = placeholder_text


  def update_extents(self, cr):
    cr.push_group()
    cr.select_font_face(self.font_face)
    cr.set_font_size(self.font_size)
    (x, y, width, height, dx, dy) = cr.text_extents(self.string)
    (ascent, descent, height, x_adv, y_adv) = cr.font_extents()
    cr.pop_group()

    self.x_off = x
    self.y_off = y
    # dx seems to leave the proper amount of padding at the end of the line?
    self.width = dx
    self.height = ascent


  def add(self, string):
    if self.placeholder:
      self.string = ''
      self.placeholder = False
    self.string = self.string + string

  def backspace(self):
    if len(self.string) > 0:
      self.string = self.string[0:-1]


  def new_width(self, requested_width):
    pass

