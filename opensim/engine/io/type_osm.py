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
import libxml2

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
  # cheat and only look at the extension for now
  if len(model_path) > 4 and model_path[-3:] == 'osm':
    return True

  return False


def read_model(sim, model_path):
  '''
  Reads a model from a file and loads it into the simulator.
  '''
  doc = libxml2.parseFile(model_path)

  root = doc.children
  if root.name != "opensim":
    log.error("not an opensim XML file")
    return

  model_root = root.children
  # skip through model and text children (XML treats 
  # whitespace as text elements)
  while model_root is not None and model_root.name != "model":
    model_root = model_root.next

  if model_root is None:
    log.error("no node named 'model'")
    return

  var = model_root.children
  # now for the meat and potatoes.
  while var is not None:

    if var.name == 'var':
      var_item = var.children
      var_name = "undefined"
      eqn = None
      while var_item is not None:
        if var_item.name == 'name':
          var_name = var_item.content.strip()
        elif var_item.name == "equation":
          eqn = var_item.content
        var_item = var_item.next

      sim.new_var(var_name, eqn)

    var = var.next

  doc.freeDoc()


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

