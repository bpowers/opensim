#===--- __init__.py - io module functions and initialization -------------===#
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
# This file contains some initialization needed for the Python modules
#
#===-----------------------------------------------------------------------===#

import logging, os, sys, re
#import type_osm
#print(dir(type_osm))
log = logging.getLogger('opensim.io')

__handler_regex = re.compile('^(type_(\w+))\.py$')
__handlers = {}

# import all tests from python files named test_*.py
for f in os.listdir(__path__[0]):
  match = __handler_regex.search(f)
  if match:
    ext = match.group(2)
    handler = match.group(1)
    mod = __import__('opensim.engine.io.' + handler, fromlist=[''])
    __handlers[ext] = mod


def get_handler(model_path):
  '''
  Determines the type of save-file we're dealing and returns a handler for it.

  It will search through all python files in this directory with names
  matching 'type_*.py', and ask each one if it can handle the file.  If the
  module can, we return a reference to the module, which must provide the
  interface for OpenSim IO handlers.
  '''
  # fast path - check the file extension against the known handlers
  if len(model_path) > 4:
    ext = model_path[-3]
    if ext in __handlers.keys() and __handlers[ext].can_handle(model_path):
      return __handlers[ext]

  # if that doesn't work, loop through all the known handlers to see
  # if anyone knows what to do with this file
  for k,handler in __handlers.iteritems():
    if handler.can_handle(model_path):
      return handler

  return None

