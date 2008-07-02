#===--- text.py - OpenSim text manager ------------------------------------===#
#
# Copyright 2008 Bobby Powers
#
# This file is part of OpenSim.
# 
# OpenSim is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# OpenSim is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with OpenSim.  If not, see <http://www.gnu.org/licenses/>.
#
#===-----------------------------------------------------------------------===#
#
# This file contains the implementation of TextInfo, the class designed
# to store text labels and provide information about the size of them.
#
#===-----------------------------------------------------------------------===#

class TextInfo():

  # this is the DPI on my Hardy system
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
    cr.save()
    cr.select_font_face(self.font_face)
    cr.set_font_size(self.font_size)
    (x, y, width, height, dx, dy) = cr.text_extents(self.string)
    (ascent, descent, height, x_adv, y_adv) = cr.font_extents()
    cr.restore()

    self.x_off = x
    self.y_off = y
    # dx seems to leave the proper amount of padding at the end of the line?
    self.width = dx
    self.height = ascent - descent


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

