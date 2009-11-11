#!/usr/bin/env python
#===--- speed_test.py - test the speed of interpretation ------------------===#
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
# This program runs the non-libtool version of the command-line driver
# 10 times and averages the time it takes to run
#
#===-----------------------------------------------------------------------===#

import os

cmd = 'opensim-d ../examples/models/rabbit.osm -p'

def main ():
  total_time = 0

  for i in range (0,10):
    time_string = os.popen (cmd).readlines ()[0]
    time = int (time_string)
    total_time += time

  # divide by 1e6 to get seconds from milliseconds  
  print "%f" % (total_time/10.0/1e6)


if __name__ == "__main__":
  main ()
