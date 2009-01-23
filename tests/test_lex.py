#===--- test_lex.py - Token manipulation function tests --------------===#
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
# This file contains tests for lex.py, most importantly testing
# the tokenize function and the creation of valid tokens
#
#===----------------------------------------------------------------------===#

import opensim.ng as engine
import opensim.ng.lex as lex
import unittest


class TestScannerCase(unittest.TestCase):

  def setUp(self):
    self.sim = engine.Simulator()
    self.eqn_empty = ''
    self.eqn_none = None
    self.eqn_num = 3.14
    self.eqn_ok = "20+time"
    self.eqn_space = "  20  +  time  "
    self.tok_idens = ['20', '+', 'time']
    self.eqn_dec = ".2"
    self.eqn_bad_dec = ".2.2.2"


  def test_empty_eqn(self):
    '''
    test to make sure we handle empty equations
    '''
    scanner_1 = lex.Scanner(self.sim.new_var('test_1'))
    scanner_2 = lex.Scanner(self.sim.new_var('test_2', ''))
    scanner_3 = lex.Scanner(self.sim.new_var('test_3', None))
    scanner_4 = lex.Scanner(self.sim.new_var('test_4', '       '))

    # should return none, since we have only whitespace
    self.assert_(scanner_1.get_tok() is None)
    self.assert_(scanner_2.get_tok() is None)
    self.assert_(scanner_3.get_tok() is None)
    self.assert_(scanner_4.get_tok() is None)


  def test_extra_get_tok_1(self):
    '''
    test to make sure we handle extra calls to get_tok gracefully
    '''
    scanner_1 = lex.Scanner(self.sim.new_var('test_1'))
    scanner_2 = lex.Scanner(self.sim.new_var('test_2', ''))
    scanner_3 = lex.Scanner(self.sim.new_var('test_3', None))

    for i in range(0,10):
      # should return an empty list and not throw an error
      self.assert_(scanner_1.get_tok() is None)
      self.assert_(scanner_2.get_tok() is None)
      self.assert_(scanner_3.get_tok() is None)


  def test_ok_eqn(self):
    '''
    test to make sure we handle good equations
    '''
    var = self.sim.new_var('test', self.eqn_ok)
    scanner = lex.Scanner(var)
    num_toks = 0
    tok = scanner.get_tok()
    while tok is not None:
      self.assert_(isinstance(tok, lex.Token))
      num_toks += 1
      tok = scanner.get_tok()

    self.assert_(num_toks == 3)


  def test_spaced_eqn(self):
    '''
    test to make sure we handle good equations with spaces
    '''
    var = self.sim.new_var('test', self.eqn_space)
    scanner = lex.Scanner(var)
    num_toks = 0
    tok = scanner.get_tok()
    while tok is not None:
      self.assert_(isinstance(tok, lex.Token))
      num_toks += 1
      tok = scanner.get_tok()

    self.assert_(num_toks == 3)


  def test_tok_contents(self):
    '''
    test to make sure we pass along the contents of the tokens correctly
    '''
    var = self.sim.new_var('test', self.eqn_space)
    scanner = lex.Scanner(var)
    i = 0
    tok = scanner.get_tok()
    while tok is not None:
      self.assert_(tok.iden == self.tok_idens[i],
                   '%s (%s) should be %s (%s)' % (tok.iden,
                     type(tok.iden), self.tok_idens[i],
                     type(self.tok_idens[i])))
      i += 1
      tok = scanner.get_tok()


  def test_leading_decimal(self):
    '''
    test to make sure we handle numbers with leading decimal points
    '''
    var = self.sim.new_var('test', self.eqn_dec)
    scanner = lex.Scanner(var)
    num_toks = 0
    tok = scanner.get_tok()
    while tok is not None:
      self.assert_(tok.kind is lex.NUMBER)
      num_toks += 1
      tok = scanner.get_tok()

    self.assert_(num_toks == 1)


  def test_number_value(self):
    '''
    test to make sure we handle numbers with leading decimal points
    '''
    var = self.sim.new_var('test', self.eqn_dec)
    scanner = lex.Scanner(var)
    num_toks = 0
    tok = scanner.get_tok()
    while tok is not None:
      self.assert_(tok.iden == self.eqn_dec, 
                   '%s should equal %s' % (tok.iden, self.eqn_dec))
      num_toks += 1
      tok = scanner.get_tok()


  def test_extra_decimal(self):
    '''
    test to make sure we handle numbers with leading decimal points

    we should handle them by putting a string in the error field
    of the token
    '''
    var = self.sim.new_var('test', self.eqn_bad_dec)
    scanner = lex.Scanner(var)
    toks = []
    tok = scanner.get_tok()
    while tok is not None:
      toks.append(tok)
      tok = scanner.get_tok()

    self.assert_(len(toks) is 1)
    self.assert_(toks[0].error is not None)
    self.assert_(isinstance(toks[0].error, str))



class TestIntegralScannerCase(unittest.TestCase):

  def setUp(self):
    self.sim = engine.Simulator()
    self.eqn1 = "INTEG(0,0)"
    self.eqn2 = "INTEG ( 0, 0 )"
    self.eqn3 = " INTEG ( 0, 0 )"
    self.eqn4 = "INTEG ( 0, 0 ) "
    self.eqn5 = " INTEG ( 0, 0 ) "
    self.num_toks = 6


  def test_no_spaces(self):
    '''
    Test to make sure we handle integral equations with no spaces.
    '''
    var = self.sim.new_var('test', self.eqn1)
    scanner = lex.Scanner(var)
    toks = []
    tok = scanner.get_tok()
    while tok is not None:
      toks.append(tok)
      tok = scanner.get_tok()
    num_toks = len(toks)
    first_tok = toks[0]

    self.assert_(num_toks is self.num_toks,
                 'expected %d toks, got %d : %s' %
                 (self.num_toks, num_toks, toks))
    self.assert_(first_tok.kind is lex.INTEGRAL,
                 'integral not promoted: \'%s\' is %s' %
                 (first_tok.iden, lex.name_for_tok_type(first_tok.kind)))


  def test_spaces(self):
    '''
    Test to make sure we handle integral equations with spaces.
    '''
    var = self.sim.new_var('test', self.eqn2)
    scanner = lex.Scanner(var)
    toks = []
    tok = scanner.get_tok()
    while tok is not None:
      toks.append(tok)
      tok = scanner.get_tok()
    num_toks = len(toks)
    first_tok = toks[0]

    self.assert_(num_toks is self.num_toks,
                 'expected %d toks, got %d : %s' %
                 (self.num_toks, num_toks, toks))
    self.assert_(first_tok.kind is lex.INTEGRAL,
                 'integral not promoted: \'%s\' is %s' %
                 (first_tok.iden, lex.name_for_tok_type(first_tok.kind)))


  def test_leading_spaces(self):
    '''
    Test to make sure we handle integral equations with leading spaces.
    '''
    var = self.sim.new_var('test', self.eqn3)
    scanner = lex.Scanner(var)
    toks = []
    tok = scanner.get_tok()
    while tok is not None:
      toks.append(tok)
      tok = scanner.get_tok()
    num_toks = len(toks)
    first_tok = toks[0]

    self.assert_(num_toks is self.num_toks,
                 'expected %d toks, got %d : %s' %
                 (self.num_toks, num_toks, toks))
    self.assert_(first_tok.kind is lex.INTEGRAL,
                 'integral not promoted: \'%s\' is %s' %
                 (first_tok.iden, lex.name_for_tok_type(first_tok.kind)))


  def test_trailing_spaces(self):
    '''
    Test to make sure we handle integral equations with leading spaces.
    '''
    var = self.sim.new_var('test', self.eqn4)
    scanner = lex.Scanner(var)
    toks = []
    tok = scanner.get_tok()
    while tok is not None:
      toks.append(tok)
      tok = scanner.get_tok()
    num_toks = len(toks)
    first_tok = toks[0]

    self.assert_(num_toks is self.num_toks,
                 'expected %d toks, got %d : %s' %
                 (self.num_toks, num_toks, toks))
    self.assert_(first_tok.kind is lex.INTEGRAL,
                 'integral not promoted: \'%s\' is %s' %
                 (first_tok.iden, lex.name_for_tok_type(first_tok.kind)))


  def test_leading_and_trailing_spaces(self):
    '''
    Test to make sure we handle integral equations with l and r spaces.
    '''
    var = self.sim.new_var('test', self.eqn5)
    scanner = lex.Scanner(var)
    toks = []
    tok = scanner.get_tok()
    while tok is not None:
      toks.append(tok)
      tok = scanner.get_tok()
    num_toks = len(toks)
    first_tok = toks[0]

    self.assert_(num_toks is self.num_toks,
                 'expected %d toks, got %d : %s' %
                 (self.num_toks, num_toks, toks))
    self.assert_(first_tok.kind is lex.INTEGRAL,
                 'integral not promoted: \'%s\' is %s' %
                 (first_tok.iden, lex.name_for_tok_type(first_tok.kind)))


  def test_only_tokens_1(self):
    '''
    Test to make sure we only return Tokens.
    '''
    var = self.sim.new_var('test', self.eqn5)
    scanner = lex.Scanner(var)
    tok = scanner.get_tok()
    while tok is not None:
      self.assert_(isinstance(tok, lex.Token))
      tok = scanner.get_tok()



def suite():
  '''
  Get a unittest.TestSuite containing all the tests in this module.
  '''
  scanner_suite = unittest.TestLoader().loadTestsFromTestCase(TestScannerCase)
  integral_suite = unittest.TestLoader()\
                     .loadTestsFromTestCase(TestIntegralScannerCase)

  return unittest.TestSuite([scanner_suite, integral_suite])


if __name__ == '__main__':
  # give us more verbose output than the standard unittest.main()
  unittest.TextTestRunner(verbosity=2).run(suite())

