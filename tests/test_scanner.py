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


class TestIntegralTokens(unittest.TestCase):

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
    Test to make sure we handle integral equations with leading spaces.
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



if __name__ == '__main__':
  # give us more verbose output than the standard unittest.main()
  suite = unittest.TestLoader().loadTestsFromTestCase(TestIntegralTokens)

  unittest.TextTestRunner(verbosity=2).run(suite)

