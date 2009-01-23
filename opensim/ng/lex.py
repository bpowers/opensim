#===--- lex.py - Token manipulation functions ----------------------------===#
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
# This file contains data structures and functions for scanning equations
# and creating tokens from the input
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

_reserved = {IDEN_INTEGRAL: INTEGRAL,
             }

OPERATORS = '+-*/^,()[]'


class Token:
  '''
  Represents a distinct token in an equation.
  '''

  def __init__(self, start, length, kind, iden=None, val=None):

    # taking a page from clang, lets keep info on where are tok is
    # so that we can provide meaningful diagnostics later.
    self.start = start
    self.length = length

    self.kind = kind
    self.iden = iden
    self.val = val

    self.error = None



class Scanner:
  '''
  Produces tokens from an input equation
  '''
  
  def __init__(self, var=None, eqn=None):
    '''
    Initialize a scanner for a given variable
    '''
    if var:
      self.__var = var
      self.__eqn = var.props.equation
    else:
      self.__eqn = eqn
    # a little shorter than calling len each time we need it
    self.__len = len(self.__eqn)
    self.__pos = 0


  def get_tok(self):
    '''
    Gets the next token from the scanner, or None if at end of equation
    '''
    eqn = self.__eqn
    while self.__pos < self.__len:

      # skip whitespace
      if eqn[self.__pos].isspace():
        self.__pos += 1
        continue

      # check for identifiers
      if eqn[self.__pos].isalpha():
        start = self.__pos

        while self.__pos < self.__len:
          if eqn[self.__pos].isalpha() or eqn[self.__pos].isdigit() or \
             eqn[self.__pos] == '_':
            self.__pos += 1
          else:
            break

        iden = eqn[start:self.__pos]

        tok = Token(start, self.__pos-start, IDENTIFIER, iden)
        # if this identifier is a language construct we
        # update the token type.
        self.__promote_identifier(tok)

        return tok

      # next check for numbers
      elif eqn[self.__pos].isdigit() or eqn[self.__pos] == '.':

        # keep track of how many decimals we have, SHOULD only be 1
        num_decimals = 0

        start = self.__pos
        while self.__pos < self.__len:
          if  eqn[self.__pos].isdigit() or eqn[self.__pos] == '.':
            if eqn[self.__pos] == '.':
              num_decimals += 1
            self.__pos += 1
          else:
            break

        num = eqn[start:self.__pos]
        tok = Token(start, self.__pos-start, NUMBER, num)

        if num_decimals > 1:
          tok.error = 'more than one decimal in number'

        if not tok.error:
          tok.val = float(num)

        return tok

      else:
        if OPERATORS.find(eqn[self.__pos]) is -1:
          raise ValueError, '\'%s\' (%d) is not a valid operator' % \
                            (eqn[self.__pos], self.__pos)
        tok = Token(self.__pos, 1, OPERATOR, eqn[self.__pos])
        self.__pos += 1
        return tok


  def __promote_identifier(self, token):
    '''
    Promote reserved identifiers to their individual token type.

    Some identifiers (like if, integ, else, etc.) represent language
    constructs, so we promote them from identifiers to their respective
    token types.
    '''
    if _reserved.has_key(token.iden):
      token.kind = _reserved[token.iden]



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
  
  scanner = Scanner(var, eqn)
  tok = scanner.get_tok()
  while tok:
    toks.append(tok)
    tok = scanner.get_tok()

  return toks


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
  Removed reserved identifiers from a list of identifiers.  Initially
  (if not only) for getting a list of influences from variables.
  '''

  copy = list(identifiers)

  for iden in identifiers:
    if iden in BUILTIN_FUNCS:
      copy.remove(iden)

  return copy

