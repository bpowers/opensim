#===--- type_osm.py - OpenSim XML file io --------------------------------===#
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
# This file contains functions to deal with file i/o on OpenSim XML files.
#
#===-----------------------------------------------------------------------===#

import logging
import errors

log = logging.getLogger('opensim.io')


# make available a type string to identify the kind of file
# we're working with, and a tuple of the versions of this
# file type that we can interact with.  this is important
# for the visual side of things to know what it needs to
# write out.
TYPE = 'OpenSim XML'
VERSIONS = ('1.0',)


def can_handle(model_path):
  '''
  Indicates whether this module can understand a given save-file.

  Returns a boolean to indicate if we can work with this model or not.
  '''
  return False


def read_model(sim, model_path):
  '''
  Reads a model from a file and loads it into the simulator.
  '''
  pass


def write_file(sim, model_path):
  '''
  Write a model to a specified file.

  Helper function which wraps _write_header, _write_model, and
  _write_footer.
  '''
  pass


def _write_header(sim, open_file):
  '''
  Writes the model header to an open file object.

  Important for things like XML doctype, opening tags, etc.
  '''
  pass


def _write_model(sim, open_file):
  '''
  Writes the meat and potatoes of the model to an open file object.
  '''
  pass


def _write_footer(sim, open_file):
  '''
  Writes the footer to an open file object.

  Important for XML, etc.
  '''
  pass

