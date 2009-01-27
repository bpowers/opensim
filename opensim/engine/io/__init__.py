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

import logging, os, re
import opensim.engine.errors

log = logging.getLogger('opensim.io')

__handler_regex = re.compile('^(type_(\w+))\.py$')
__handlers = []

# import all tests from python files named test_*.py
for file in os.listdir('.'):
  match = __handler_regex.search(file)
  if match:
    test_file = match.group(1)
    mod = __import__(test_file)
    suite = getattr(mod, 'suite')
    __tests.append(suite())

def get_handler(model_path):
  '''
  Determines the type of save-file we're dealing and returns a handler for it.

  It will search through all python files in this directory with names
  matching 'type_*.py', and ask each one if it can handle the file.  If the
  module can, we return a reference to the module, which must provide the
  interface for OpenSim IO handlers.
  '''
  log.error('in get handler')

