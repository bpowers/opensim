#===--- __init__.py - OpenSim Python model initialization -----------------===#
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
# This file contains some initialization needed for the Python modules
#
#===-----------------------------------------------------------------------===#

import engine.errors as errors
from engine.errors import config_logging

# initialize opensim's logging of errors
config_logging(ostream=None, handler=errors.NullHandler())

def enable_logging():
  errors.config_logging()

VERSION = '0.5.1'
AUTHORS = ('Bobby Powers <bobbypowers@gmail.com>',)
COPYRIGHT = 'Copyright 2008-2009 Bobby Powers'
LICENSE = '''
OpenSim is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenSim is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OpenSim.  If not, see <http://www.gnu.org/licenses/>.
'''

