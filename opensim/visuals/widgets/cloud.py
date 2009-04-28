#===--- cloud.py - OpenSim Cloud widget -----------------------------------===#
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
# This file contains the implementation of the stock widget
#
#===-----------------------------------------------------------------------===#

import gobject
import gtk
import math
import cairo
import rsvg
import os

from gaphas.item import Item
from gaphas.connector import Handle, LinePort
from gaphas.geometry import Rectangle
from gaphas.state import observed, reversible_property

import logging

from text import TextInfo
from item import SimItem

ICON_SIZE = 55

class CloudItem(SimItem):

  __gtype_name__ = 'CloudItem'

  def __init__(self, x, y):
    super(CloudItem, self).__init__()

    self.width = ICON_SIZE
    self.height = ICON_SIZE

    # the 0th handle is always at the item's origin (0,0)
    h_0 = Handle(movable=False)
    h_l = Handle(movable=False)
    h_r = Handle(movable=False)
    # we don't want to be able to see or interact with either of these,
    # they're basically for positioning the h_r
    h_0.visible = False
    h_l.visible = False
    h_r.visible = False

    self._handles = (h_0, h_l, h_r)
    self._ports = (LinePort(h_0.pos, h_l.pos),
                   LinePort(h_l.pos, h_r.pos))

    # setup constraints
    self.constraint(above=(h_0.pos, h_l.pos), delta=ICON_SIZE)
    self.constraint(left_of=(h_l.pos, h_r.pos), delta=ICON_SIZE)

    # keep track of inflows and outflows, for use in engine
    self.inflows = []
    self.outflows = []

    # find our cloud icon hanging out with the other icons.
    icon_paths = gtk.icon_theme_get_default().get_search_path()
    cloud_path = None

    for path in icon_paths:
      possible_path = os.path.join(path, "opensim-cloud.svg")
      if os.path.isfile(possible_path):
        cloud_path = possible_path
        break

    if cloud_path is None:
      logging.error("could not find cloud svg!")
      raise ValueError, 'missing cloud svg'

    self._cloud = rsvg.Handle(cloud_path)

    self.set_position(x - self.width/2, y - self.height/2)


  def _name(self):
    return 'cloud'

  name = property(_name)


  def _get_new(self):
    return False

  new = property(_get_new)


  def set_position(self, x, y):
    if (x, y) != self.get_position():
      self.matrix = (1.0, 0.0, 0.0, 1, x, y)

  def get_position(self):
    return self.matrix[4], self.matrix[5]

  position = property(get_position, set_position)


  def center(self):
    return (int(self.width/2), int(self.height/2))


  def edge_point(self, end_point):
    center_x, center_y = self.center()
    
    line_angle = math.atan2((end_point[1] - center_y), 
                            (end_point[0] - center_x))
    if line_angle < 0: line_angle = 2*math.pi + line_angle
    
    
    radius = (self.width + self.height)/4
    
    center_x = center_x + radius * math.cos(line_angle)
    center_y = center_y + radius * math.sin(line_angle)
    
    logging.debug("CLOUD: %5.1f (%d, %d)" % (math.degrees(line_angle), 
                                             center_x, center_y))
    
    return (center_x, center_y)


  def draw(self, context):
    '''
    Render our nice cloud on the cairo context.
    '''
    cr = context.cairo
    cr.save()
    try:
      self._cloud.render_cairo(cr)
    except:
      self._cloud.render_cairo(cr._cairo)
    cr.restore()


  def xml_representation(self):
    return ''


  def get_rectangle(self):
    x, y = self.get_position()
    return Rectangle(x, y, self.width, self.height)

