#===--- item.py - OpenSim Python model initialization ---------------------===#
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
# This file contains the item other canvas items are based off of
#
#===-----------------------------------------------------------------------===#

import gobject
import gtk
import math

from gaphas.item import Item, Element
from gaphas.state import observed, reversible_property

import logging


class SimItem(Item):

  def __init__(self):
    super(SimItem, self).__init__()


class SimElement(SimItem, Element):

  def __init__(self):
    Element.__init__(self)
    SimItem.__init__(self)

