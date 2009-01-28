#===--- common.py - common functions passes might need -------------------===#
#
# Copyright 2009 Bobby Powers
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
# This file contains functions, like frange and lookup, that passes
# might need
#
#===-----------------------------------------------------------------------===#


def frange(lim_start, lim_end, increment = 1.):
  '''
  Range function that allows floating point range increments.

  Standard python range function doesn't allow floating point
  increments in ranges.
  '''
  lim_start = float(lim_start)
  count = int(math.ceil(lim_end - lim_start)/increment + 1)
  return (lim_start + n*increment for n in range(count))


# simple lookup table implementation
def lookup(table, index):
  '''
  Simple lookup table implementation.

  Table takes the format of a list of 2-tuples.
  '''

  if len(table) is 0: return 0

  # if the request is outside the min or max, then we return
  # the nearest element of the array
  if   index < table[0][0]:  return table[0][1]
  elif index > table[-1][0]: return table[-1][1]

  for i in range(0, len(table)):
    x, y = table[i]

    if index == x: return y
    if index < x:
      # slope = deltaY/deltaX
      slope = (y - table[i-1][1])/(x - table[i-1][0])
      return (index-table[i-1][0])*slope + table[i-1][1]

