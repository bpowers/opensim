#===--- parser.py - AST building functions -------------------------------===#
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
# This file contains the parser class to build ASTs from a list of
# variables
#
#===----------------------------------------------------------------------===#


def validate_stock(tokens, simulator):
  '''
  Validate the equation for a stock.
  '''

  return True


def validate_lookup(tokens):
  '''
  Validate the equation for a lookup table.
  '''

  return True


def validate_const(tokens):
  '''
  Validate the equation for a constant variable.
  '''

  return True


def validate_aux(tokens):
  '''
  Validate the equation for an auxilliary variable.
  '''

  return True
