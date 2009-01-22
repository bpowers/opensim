#===--- test_variable.py - Variable unit tests ----------------------------===#
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
# This file contains tests for variable.py, mostly validating getting and
# setting of properties
#
#===----------------------------------------------------------------------===#

import opensim.ng as engine
import unittest


class TestVariableCase(unittest.TestCase):

  def setUp(self):
    self.name = 'testing'
    self.eqn = '3*time'
    self.sim = engine.Simulator()
    self.var = self.sim.new_variable(self.name, self.eqn)


  def test_var_created(self):
    '''
    test to make sure we create variables correctly
    '''
    var = self.sim.get_variable(self.name)

    self.assert_(var is not None)
    self.assert_(self.var is var)
    self.assert_(isinstance(var, engine.Variable))


  def test_name_set(self):
    '''
    test to make sure we set the name correctly
    '''
    self.assert_(self.var.props.name == self.name)



def suite():
  '''
  Get a unittest.TestSuite containing all the tests in this module.
  '''
  var_suite = unittest.TestLoader().loadTestsFromTestCase(TestVariableCase)

  return unittest.TestSuite([var_suite])


if __name__ == '__main__':
  # give us more verbose output than the standard unittest.main()
  unittest.TextTestRunner(verbosity=2).run(suite())
