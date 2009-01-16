#===--- test_scanner.py - Token manipulation function testss -------------===#
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
# This file contains tests for scanner.py, most importantly testing
# the tokenize function and the creation of valid tokens
#
#===----------------------------------------------------------------------===#

import opensim.ng as engine
import opensim.ng.scanner as scanner
import unittest


class TestTokenizerCase(unittest.TestCase):

  def setUp(self):
    self.eqn_empty = ''
    self.eqn_none = None
    self.eqn_num = 3.14
    self.eqn_ok = "20+time"
    self.eqn_space = "  20  +  time  "


  def test_empty_eqn(self):
    '''
    test to make sure we handle empty equations
    '''
    toks = scanner.tokenize(self.eqn_empty)

    # should return an empty list and not throw an error
    self.assert_(isinstance(toks, list))
    self.assert_(len(toks) is 0)


  def test_none_eqn(self):
    '''
    test to make sure we handle None equations
    '''
    # should raise a type error for None equations
    self.assertRaises(TypeError, scanner.tokenize, self.eqn_none)


  def test_num_eqn(self):
    '''
    test to make sure we handle number equations
    '''
    # should raise a type error for an equation that is not a str
    self.assertRaises(TypeError, scanner.tokenize, self.eqn_num)


  def test_ok_eqn(self):
    '''
    test to make sure we handle good equations
    '''
    toks = scanner.tokenize(self.eqn_ok)

    # should return a list of 3 toks
    self.assert_(isinstance(toks, list))
    self.assert_(len(toks) is 3)


  def test_spaced_eqn(self):
    '''
    test to make sure we handle good equations with spaces
    '''
    toks = scanner.tokenize(self.eqn_ok)

    # should return a list of 3 toks
    self.assert_(isinstance(toks, list))
    self.assert_(len(toks) is 3)



class TestIntegralTokenizerCase(unittest.TestCase):

  def setUp(self):
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
    toks = scanner.tokenize(self.eqn1)
    num_toks = len(toks)
    first_tok = toks[0]

    self.assert_(num_toks is self.num_toks,
                 'expected %d toks, got %d : %s' %
                 (self.num_toks, num_toks, toks))
    self.assert_(first_tok[0] is scanner.INTEGRAL,
                 'integral not promoted: \'%s\' is %s' %
                 (first_tok[1], scanner.name_for_tok_type(first_tok[0])))


  def test_spaces(self):
    '''
    Test to make sure we handle integral equations with spaces.
    '''
    toks = scanner.tokenize(self.eqn2)
    num_toks = len(toks)
    first_tok = toks[0]

    self.assert_(num_toks is self.num_toks,
                 'expected %d toks, got %d : %s' %
                 (self.num_toks, num_toks, toks))
    self.assert_(first_tok[0] is scanner.INTEGRAL,
                 'integral not promoted: \'%s\' is %s' %
                 (first_tok[1], scanner.name_for_tok_type(first_tok[0])))


  def test_leading_spaces(self):
    '''
    Test to make sure we handle integral equations with leading spaces.
    '''
    toks = scanner.tokenize(self.eqn3)
    num_toks = len(toks)
    first_tok = toks[0]

    self.assert_(num_toks is self.num_toks,
                 'expected %d toks, got %d : %s' %
                 (self.num_toks, num_toks, toks))
    self.assert_(first_tok[0] is scanner.INTEGRAL,
                 'integral not promoted: \'%s\' is %s' %
                 (first_tok[1], scanner.name_for_tok_type(first_tok[0])))


  def test_trailing_spaces(self):
    '''
    Test to make sure we handle integral equations with leading spaces.
    '''
    toks = scanner.tokenize(self.eqn4)
    num_toks = len(toks)
    first_tok = toks[0]

    self.assert_(num_toks is self.num_toks,
                 'expected %d toks, got %d : %s' %
                 (self.num_toks, num_toks, toks))
    self.assert_(first_tok[0] is scanner.INTEGRAL,
                 'integral not promoted: \'%s\' is %s' %
                 (first_tok[1], scanner.name_for_tok_type(first_tok[0])))


  def test_leading_and_trailing_spaces(self):
    '''
    Test to make sure we handle integral equations with l and r spaces.
    '''
    toks = scanner.tokenize(self.eqn5)
    num_toks = len(toks)
    first_tok = toks[0]

    self.assert_(num_toks is self.num_toks,
                 'expected %d toks, got %d : %s' %
                 (self.num_toks, num_toks, toks))
    self.assert_(first_tok[0] is scanner.INTEGRAL,
                 'integral not promoted: \'%s\' is %s' %
                 (first_tok[1], scanner.name_for_tok_type(first_tok[0])))



def suite():
  '''
  Get a unittest.TestSuite containing all the tests in this module.
  '''
  tok_suite = unittest.TestLoader().loadTestsFromTestCase(TestTokenizerCase)
  integral_suite = unittest.TestLoader()\
                     .loadTestsFromTestCase(TestIntegralTokenizerCase)

  return unittest.TestSuite([tok_suite, integral_suite])


if __name__ == '__main__':
  # give us more verbose output than the standard unittest.main()
  unittest.TextTestRunner(verbosity=2).run(suite())

