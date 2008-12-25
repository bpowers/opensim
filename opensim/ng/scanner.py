#===--- scanner.py - Token manipulatiopn functions -----------------------===#
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
# This file contains data structures and functions for manipulating 
# equation tokens
#
#===----------------------------------------------------------------------===#


import re
from constants import *

# define the tokens we're likely to run into
IDENTIFIER = 1
NUMBER     = 2
OPERATOR   = 3
IF         = 4
INTEGRAL   = 5

TOK_RANGE_MIN = IDENTIFIER
TOK_RANGE_MAX = OPERATOR


IDEN_INTEGRAL = 'integ'

IDEN_RESERVED = [IDEN_INTEGRAL, 'min', 'max']


_token_names  = [(IDENTIFIER, 'identifier'),
                 (NUMBER, 'number'),
                 (OPERATOR, 'operator'),
                 (INTEGRAL, 'integral'),
                 (IF, 'if statement'),
                ]

_reserved     = [(IDEN_INTEGRAL, INTEGRAL),
                 ('if_then_else', IF),
                ]

def tokenize(eqn):
  '''
  Takes a string representing an equation and returns a list of
  tuples, where the first value is the token type, followed by
  the token data
  '''

  if not eqn or type(eqn) is not str:
    return []

  eqn = eqn.strip()
  if eqn == '':
    return []

  # we're case insensitive in system dynamics...
  eqn = eqn.lower()

  toks = []

  # compile some regular expressions
  re_iden = re.compile('^[a-z](\w|\s)*')
  re_num = re.compile('^[0-9]*\.?[0-9]*(e?[-+]?[0-9]+)?')

  while eqn is not '':

    if eqn[0].isalpha():
      m = re_iden.search(eqn)

      tok = (IDENTIFIER, m.group())
      # if this identifier is a language construct we
      # update the token type.
      tok = _promote_identifier(tok)

      toks.append(tok)

      eqn = eqn[m.end():]

    elif eqn[0].isdigit() or eqn[0] is '.':
      m = re_num.search(eqn)
      toks.append((NUMBER, m.group()))

      eqn = eqn[m.end():]

    else:
      toks.append((OPERATOR, eqn[0]))
      eqn = eqn[1:]

    eqn = eqn.lstrip()    

  return toks


def _promote_identifier(token):
  '''
  Promote reserved identifiers to their individual token type.

  Some identifiers (like if, integ, else, etc.) represent language
  constructs, so we promote them from identifiers to their respective
  token types.
  '''

  iden = token[1]

  for ref in _reserved:
    if iden == ref[0]:
      return (ref[1], iden)

  return token


def name_for_token_type(tok_type):
  '''
  A nice helper function to return the string of the
  token type (mostly for debugging purposes I assume)
  '''

  if tok_type < TOK_RANGE_MIN and tok_type > TOK_RANGE_MAX:
    log.error('variable is out of range!')
    return ''

  if tok_type is IDENTIFIER:
    return 'identifier'
  elif tok_type is NUMBER:
    return 'number'
  elif tok_type is OPERATOR:
    return 'operator'
  else:
    return 'undefined'


def strip_reserved(identifiers):
  '''
  Removed reserved identifiers from a lost of identifiers.  Initially
  (if not only) for getting a list of influences from variables.
  '''

  copy = list(identifiers)

  for iden in identifiers:
    if iden in IDEN_RESERVED:
      copy.remove(iden)

  return copy

