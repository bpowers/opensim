#===--- __init__.py - passes module functions and initialization ---------===#
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
log = logging.getLogger('opensim.passes')

__handler_regex = re.compile('^(output_(\w+))\.py$')
__output_passes = {}

# import all tests from python files named test_*.py
for f in os.listdir(__path__[0]):
  match = __handler_regex.search(f)
  if match:
    ext = match.group(2)
    handler = match.group(1)
    try:
      mod = __import__('opensim.engine.passes.' + handler, fromlist=[''])
      __output_passes[ext] = mod
    except ImportError:
      pass


def get_output_pass(kind):
  '''
  Determines the type of save-file we're dealing and returns a handler for it.

  It will search through all python files in this directory with names
  matching 'type_*.py', and ask each one if it can handle the file.  If the
  module can, we return a reference to the module, which must provide the
  interface for OpenSim IO handlers.
  '''
  if kind in __output_passes.keys():
    mod = __output_passes[kind]
    return getattr(mod, mod.CLASS_NAME)()

  return None

