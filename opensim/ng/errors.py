#===--- errors.py - Error reporting functions -----------------------------===#
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
# This file contains functions for the reporting of errors
#
#===----------------------------------------------------------------------===#

import sys, logging, logging.handlers
import opensim.ng.scanner

logger = None

def config_logging(level=logging.DEBUG, file=None, stream=sys.stderr):
  '''
  Initialize logging for opensim modules
  '''
  logger = logging.getLogger('opensim')
  logger.setLevel(level)

  # I like this format.
  format = logging.Formatter('%(levelname)s: %(message)s')

  if file:
    file_log = logging.handlers.FileHandler(file)
    file_log.setLevel(logging.DEBUG)
    file_log.setFormatter(format)
    logger.addHandler(file_log)
  if stream:
    stream_log = logging.StreamHandler(stream)
    stream_log.setLevel(logging.DEBUG)
    stream_log.setFormatter(format)
    logger.addHandler(stream_log)


def report_eqn_error(error, var, tok, other_toks=None, log=logger):
  '''
  Report an error that occured when scanning or parsing an equation.

  It kicks butt to know whats wrong when your model doesn't run,
  so we're going to try hard to produce great diagnostics like:

  error: perceived_demand.equation: 'snooth' is not a known function:
  perceived_demand = snooth(demand, 5)
                     ^~~~~~

  color would be cool too eventually for the command line
  '''

  # first line of message
  desc = '%s.equation: %s' % (var.props.name, error)

  # equation, second line
  eqn = '%s = %s' % (var.props.name, var.props.equation)

  uline = ' ' * len(var.props.equation)
  uline = uline[0:tok.start] + '^' + '~'*(tok.length-1) + \
          uline[tok.start+tok.length-1:]
  uline = uline.rjust(len(eqn))

  err = '%s\n%s\n%s' % (desc, eqn, uline)

