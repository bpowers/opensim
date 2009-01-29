#===--- parse.py - Parser for SD equations -------------------------------===#
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
# This file contains functions for parsing equations and creating ASTs.
#
#===----------------------------------------------------------------------===#


import logging
import lex
from ast import *
import constants as sim
from errors import report_eqn_error

log = logging.getLogger('opensim.parse')

_precedence = {'=': 2,
               '<': 10,
               '>': 10,
               '+': 20,
               '-': 20,
               '*': 40,
               '/': 40,
               '^': 60,
              }


class TempVar:
  '''
  Temporary variable object for storing e.g. net flows.
  '''
  def __init__(self, name):
    self.name = name



class Parser:
  '''
  Class to parse variable expressions and create well formed ASTs.
  '''

  def __init__(self, var):
    '''
    Initialize a parser for a specific variable and it's equation.
    '''
    self.__var = var
    self.__scanner = lex.Scanner(var)
    self.__cur_tok = None
    self.__ast = None

    self.kind = sim.UNDEF
    self.valid = False
    self.refs = None


  def parse(self):
    '''
    Parse our variables equation, making available pertinent information.

    Information includes:
    * a well-formed AST, if the equation is valid
    * any errors or warnings that were encountered
    * the lookup table, if applicable
    '''
    self._get_next_tok()
    self._parse()


  def _get_next_tok(self):
    self.__cur_tok = self.__scanner.get_tok()


  def _get_tok_precedence(self):

    if self.__cur_tok is None:
      return -1

    if self.__cur_tok.kind is not lex.OPERATOR:
      return -1

    if self.__cur_tok.iden in _precedence:
      return _precedence[self.__cur_tok.iden]
    else:
      return -1


  def _parse(self):
    if self.__cur_tok is None:
      return

    if self.__cur_tok.kind is lex.INTEGRAL:
      self._parse_integ()
    elif self.__cur_tok.kind is lex.OPERATOR and self.__cur_tok.iden == '[':
      self._parse_lookup()
    else:
      self._parse_eqn()


  def _parse_integ(self):
    '''
    Handle integral equations.

    Integrals look like this:

      integ ( expr, expr )
    net flow---^     ^---initial value
    '''
    # eat 'integ'
    self._get_next_tok()

    if not self.__cur_tok or self.__cur_tok.iden != '(':
      err = "expected '(', not '%s' in integral" % self.__cur_tok.iden
      return report_eqn_error(err, self.__var, self.__cur_tok)
    # eat '('
    self._get_next_tok()

    net_flow = self._parse_expr()
    if not net_flow:
      return None

    if not self.__cur_tok or self.__cur_tok.iden != ',':
      err = "expected ',', not '%s' in integral" % self.__cur_tok.iden
      return report_eqn_error(err, self.__var, self.__cur_tok)
    # eat ','
    self._get_next_tok()

    initial_val = self._parse_expr()
    if not initial_val:
      return None

    if not self.__cur_tok or self.__cur_tok.iden != ')':
      err = "expected ')', not '%s' in integral" % self.__cur_tok.iden
      return report_eqn_error(err, self.__var, self.__cur_tok)
    # eat ')'
    self._get_next_tok()

    # TODO: need to sort out assignment and intial
    self.valid = True
    self.kind = sim.STOCK
    self.net_flow = net_flow
    self.initial = ASTAssignExpr(self.__var.props.name, initial_val)



  def _parse_lookup(self):
    '''
    Handle the equation as a lookup table.

    We're going to do this through python's built in 'eval' function,
    since our syntax for declaring lookup tables is equivolent to the
    Python for declaring lists of two-tuples.
    '''
    try:
      self.table = eval(self.__var.props.equation.strip())
      assert isinstance(self.table, list)
    except:
      # TODO: we should be more expressive here
      log.error('lookup table was incorrectly formatted:\n%s' %
                self.__var.props.equation)
    else:
      self.kind = sim.LOOKUP
      self.valid = True


  def _parse_eqn(self):
    '''
    Handle equations for auxiliary variables and flows.

    These will just be exprs, but we need to make sure we get a valid
    AST and set our kind accordingly.
    '''
    ast = self._parse_expr()

    if ast:
      self.ast = ASTAssignExpr(self.__var.props.name, ast)
      self.valid = True
      if isinstance(ast, ASTValue):
        self.kind = sim.CONST
      else:
        self.kind = sim.AUX


  def _parse_expr(self):
    '''
    Handles expressions and returns a corresponding AST.
    '''
    lhs = self._parse_unary()
    if not lhs:
      return None

    return self._parse_binop_rhs(0, lhs)


  def _parse_binop_rhs(self, expr_prec, lhs):
    '''
    Handle recursively binary operators using operator precedence.
    '''
    while True:
      tok_prec = self._get_tok_precedence()
      if tok_prec < expr_prec:
        return lhs

      # remember the current operator and eat it's token
      op = self.__cur_tok.iden
      self._get_next_tok()

      rhs = self._parse_unary()
      if not rhs:
        return None

      next_prec = self._get_tok_precedence()
      if tok_prec < next_prec:
        rhs = self._parse_binop_rhs(tok_prec+1, rhs)
        if not rhs:
          return None

      lhs = ASTBinExpr(op, lhs, rhs)


  def _parse_primary(self):
    '''
    Handle identifiers, function calls and constants
    '''
    if self.__cur_tok.kind is lex.NUMBER:
      return self._parse_number()
    elif self.__cur_tok.kind is lex.IDENTIFIER:
      return self._parse_iden()
    else:
      err = 'expected identifier or number, not \'%s\'' % self.__cur_tok.iden
      return report_eqn_error(err, self.__var, self.__cur_tok)


  def _parse_number(self):
    '''
    Handles numeric constants and returns an AST leaf node.
    '''
    node = ASTValue(self.__cur_tok.val)
    # eat the number token
    self._get_next_tok()
    return node


  def _parse_iden(self):
    '''
    Handle identifier and function calls.
    '''
    iden = self.__cur_tok.iden
    # eat identifier expression
    self._get_next_tok()

    if not self.__cur_tok or (not self.__cur_tok.iden == '(' and \
       not self.__cur_tok.iden == '['):
      if not self.refs:
        self.refs = [iden]
      else:
        self.refs.append(iden)
      return ASTVarRef(iden)

    # eat '(' or '['
    paren_char = self.__cur_tok.iden
    self._get_next_tok()

    # if we have a lookup...
    if paren_char == '[':
      arg = self._parse_expr()
      if not self.__cur_tok.iden == ']':
        err = 'Expected \']\' not \'%s\'' % self.__cur_tok.iden
        return report_eqn_error(err, self.__var, self.__cur_tok)
      self._get_next_tok()
      return ASTLookupRef(iden, arg)

    args = []
    if self.__cur_tok.iden != ')':
      while True:
        arg = self._parse_expr()
        if not arg:
          return None
        args.append(arg)

        if self.__cur_tok.iden == ')':
          break

        if self.__cur_tok.iden != ',':
          err = 'Expected \',\' not \'%s\'' % self.__cur_tok.iden
          return report_eqn_error(err, self.__var, self.__cur_tok)
        self._get_next_tok()

    # eat the ')'
    self._get_next_tok()

    return ASTCallExpr(iden, args)


  def _parse_unary(self):
    '''
    Handle unary operations.
    '''
    if self.__cur_tok.kind is not lex.OPERATOR:
      return self._parse_primary()
    elif self.__cur_tok.iden == '(':
      return self._parse_paren()

    # our only allowed binary operators are + and -
    if not self.__cur_tok.iden == '+' and not self.__cur_tok.iden == '-':
      err = 'unexpected or unknown unary operator: %s' % self.__cur_tok.iden
      return report_eqn_error(err, self.__var, self.__cur_tok)

    # get our operator then eat the token
    op = self.__cur_tok.iden
    self._get_next_tok()
    lval =  self._parse_unary()
    if lval:
      return ASTUnaryExpr(op, lval)


  def _parse_paren(self):
    '''
    Handle parenthesis.
    '''
    # eat '('
    self._get_next_tok(self)
    expr = self._parse_expr()

    if not expr:
      return None

    if self.__cur_tok.iden != ')':
      err = 'expected \')\', not \'%s\'' % self.__cur_tok.iden
      return report_eqn_error(err, self.__var, self.__cur_tok)

    # eat ')'
    self._get_next_tok()

    return expr

