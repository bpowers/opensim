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

import opensim.engine as engine
import opensim.engine.variable as variable
import unittest


class TestVariableCase(unittest.TestCase):

  def setUp(self):
    self.name = 'testing'
    self.eqn = '3*time'
    self.sim = engine.Simulator()


  def test_var_created(self):
    '''
    test to make sure we create variables correctly
    '''
    var = self.sim.new_var(self.name, self.eqn)
    var2 = self.sim.get_var(self.name)

    self.assert_(var is not None)
    self.assert_(var is var2)
    self.assert_(isinstance(var, engine.Variable))


  def test_name_set(self):
    '''
    test to make sure we set the name correctly
    '''
    var = self.sim.new_var(self.name, self.eqn)
    self.assert_(var.props.name == self.name)


  def test_bad_parent_None(self):
    '''
    test to make sure we raise an error on passing a bad parent
    '''
    self.assertRaises(AttributeError, variable.Variable, None, 'test_bad')


  def test_bad_parent_typed(self):
    '''
    test to make sure we raise an error on passing a bad parent
    '''
    self.assertRaises(AttributeError, variable.Variable, 2.5, 'test_bad')


  def test_num_eqn(self):
    '''
    test to make sure we handle number equations gracefully

    should implicitly convert them to a string
    '''
    num = 2.5
    var = self.sim.new_var(self.name, num)
    self.assert_(isinstance(var.props.equation, str))
    self.assert_(var.props.equation == str(num))



def suite():
  '''
  Get a unittest.TestSuite containing all the tests in this module.
  '''
  var_suite = unittest.TestLoader().loadTestsFromTestCase(TestVariableCase)

  return unittest.TestSuite([var_suite])


if __name__ == '__main__':
  # give us more verbose output than the standard unittest.main()
  unittest.TextTestRunner(verbosity=2).run(suite())

