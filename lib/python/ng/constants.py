#===--- constants.py - OpenSim simulation engine -------------------------===#
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
#===----------------------------------------------------------------------===#
#
# This file contains the pure python implementation of a system dynamics 
# simulator.
#
#===----------------------------------------------------------------------===#


# constants for the type of output we want
EMIT_IR               = 1
EMIT_PYTHON           = 2
EMIT_FORTRAN          = 3
EMIT_OUTPUT           = 4
EMIT_AS3              = 5

EMIT_RANGE_MIN        = EMIT_IR
EMIT_RANGE_MAX        = EMIT_AS3


# fundamental variable types
UNDEF                 = 0
STOCK                 = 1
AUX                   = 2
LOOKUP                = 3
CONST                 = 4
FLOW                  = 5

TYPE_MIN              = UNDEF
TYPE_MAX              = FLOW


# inital values for time variables
INITIAL_TIME_START    = 1.0
INITIAL_TIME_END      = 100.0
INITIAL_TIME_STEP     = .125
INITIAL_TIME_SAVESTEP = 1.0

