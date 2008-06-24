class TextInfo():

  def __init__(self, string, font_face='sans-serif', font_size=14, cr=None):
    self.string = string
    self.font_face = font_face
    self.font_size = font_size


  def update_extents(self, cr):
    cr.push_group()
    cr.select_font_face(self.font_face)
    cr.set_font_size(self.font_size)
    (x, y, width, height, dx, dy) = cr.text_extents(self.string)
    cr.pop_group()

    self.x_off = x
    self.y_off = y
    # dx seems to leave the proper amount of padding at the end of the line?
    self.width = dx
    self.height = height