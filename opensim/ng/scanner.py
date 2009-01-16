#===--- scanner.py - Token manipulation functions ------------------------===#
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
INTEGRAL   = 4

__token_names  = ['',
                  'identifier',
                  'number',
                  'operator',
                  'integral',
                 ]

TOK_RANGE_MIN = IDENTIFIER
TOK_RANGE_MAX = len(__token_names)

# identifier that represents integrals
IDEN_INTEGRAL = 'integ'

__reserved = {IDEN_INTEGRAL: INTEGRAL,
             }

OPERATORS = '+-*/^,()[]'


class Token:
  '''
  Represents a distinct token in an equation.
  '''

  def __init__(self, position, length, kind, iden=None, val=None):

    # taking a page from clang, lets keep info on where are tok is
    # so that we can provide meaningful diagnostics later.
    self.position = position
    self.length = length

    self.kind = kind
    self.iden = iden
    self.val = val

    self.error = None



def tokenize(eqn, var=None):
  '''
  Return a list of 'Token's for a given equation

  Takes a string representing an equation and returns a list of
  tuples, where the first value is the token type, followed by
  the token data
  '''

  # this is python but we really want to do strict type
  # checking here, as anything other than a real string
  # will cause us to throw an error
  if eqn is None:
    raise TypeError, 'need something, not None, to tokenize'

  if type(eqn) is not str:
    raise TypeError, 'can only tokenize strings, not \'%s\'' % type(eqn)

  if eqn.strip() == '':
    return []

  # we're case insensitive in system dynamics...
  eqn = eqn.lower()

  toks = []
  pos = 0
  length = len(eqn)
  while pos < length:

    # skip whitespace
    if eqn[pos].isspace():
      pos += 1
      continue

    # check for identifiers
    if eqn[pos].isalpha():
      start = pos

      while pos < length:
        if eqn[pos].isalpha() or eqn[pos].isdigit() or eqn[pos] == '_':
          pos += 1
        else:
          break

      iden = eqn[start:pos]

      tok = Token(start, pos-start, IDENTIFIER, iden)
      # if this identifier is a language construct we
      # update the token type.
      __promote_identifier(tok)

      toks.append(tok)

    # next check for numbers
    elif eqn[pos].isdigit() or eqn[pos] == '.':

      # keep track of how many decimals we have, SHOULD only be 1
      num_decimals = 0

      start = pos
      while pos < length:
        if  eqn[pos].isdigit() or eqn[pos] == '.':
          if eqn[pos] == '.':
            num_decimals += 1
          pos += 1
        else:
          break

      num = eqn[start:pos]
      tok = Token(start, pos-start, NUMBER, num)

      if num_decimals > 1:
        tok.error = 'more than one decimal in number'

      if not tok.error:
        tok.val = float(num)

      toks.append(tok)

    else:
      if OPERATORS.find(eqn[pos]) is -1:
        raise ValueError, '\'%s\' (%d) is not a valid operator' % (eqn[pos],
                                                                   pos)
      tok = Token(pos, 1, OPERATOR, eqn[pos])
      toks.append(tok)

      pos += 1

  return toks


def __promote_identifier(token):
  '''
  Promote reserved identifiers to their individual token type.

  Some identifiers (like if, integ, else, etc.) represent language
  constructs, so we promote them from identifiers to their respective
  token types.
  '''

  if __reserved.has_key(token.iden):
    token.kind = __reserved[token.iden]


def name_for_tok_type(tok_type):
  '''
  A nice helper function to return the string of the
  token type (mostly for debugging purposes I assume)
  '''
  if not isinstance(tok_type, int):
    raise TypeError, 'tok_type must be an int, \'%s\' is %s' \
                     % (tok_type, type(tok_type))

  if tok_type < TOK_RANGE_MIN and tok_type > TOK_RANGE_MAX:
    log.error('variable is out of range!')
    return ''

  return __token_names[tok_type]


def strip_reserved(identifiers):
  '''
  Removed reserved identifiers from a lost of identifiers.  Initially
  (if not only) for getting a list of influences from variables.
  '''

  copy = list(identifiers)

  for iden in identifiers:
    if iden in BUILTIN_FUNCS:
      copy.remove(iden)

  return copy

