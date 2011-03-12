#===--- test_parse.py - Unit tests for the parser ------------------------===#
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
# This file contains tests for parse.py
#
#===----------------------------------------------------------------------===#

import opensim.engine as engine
import opensim.engine.parse as parse
import unittest


class TestParserCase(unittest.TestCase):

  def setUp(self):
    self.eqn_empty = ''
    self.eqn_none = None
    self.eqn_num = 3.14
    self.eqn_ok = "20+time"
    self.eqn_space = "  20  +  time  "
    self.tok_idens = ['20', '+', 'time']
    self.eqn_dec = ".2"
    self.eqn_bad_dec = ".2.2.2"


  def test_(self):
    '''
    test 
    '''
    pass



def suite():
  '''
  Get a unittest.TestSuite containing all the tests in this module.
  '''
  parser_suite = unittest.TestLoader().loadTestsFromTestCase(TestParserCase)

  return unittest.TestSuite([parser_suite])


if __name__ == '__main__':
  # give us more verbose output than the standard unittest.main()
  unittest.TextTestRunner(verbosity=2).run(suite())

