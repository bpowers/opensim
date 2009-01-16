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

# compile some regular expressions for use tokenizing
__RE_IDEN = re.compile('^[a-z](\w|\s)*')
__RE_NUM = re.compile('^[0-9]*\.?[0-9]*(e?[-+]?[0-9]+)?')


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

  eqn = eqn.strip()

  # we're case insensitive in system dynamics...
  eqn = eqn.lower()

  toks = []

  while eqn is not '':

    if eqn[0].isalpha():
      m = __RE_IDEN.search(eqn)

      tok = (IDENTIFIER, m.group().strip())
      # if this identifier is a language construct we
      # update the token type.
      tok = _promote_identifier(tok)

      toks.append(tok)

      eqn = eqn[m.end():]

    elif eqn[0].isdigit() or eqn[0] is '.':
      m = __RE_NUM.search(eqn)
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

  if __reserved.has_key(iden):
    return (__reserved[iden], iden)

  return token


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

