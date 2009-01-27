#===--- test_simulator.py - Simulator unit tests -------------------------===#
#
# Copyright 2009 Bobby Powers
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
# This file contains tests for simulator.py, such as validating the loading
# and saving of models.
#
#===----------------------------------------------------------------------===#

import unittest
import opensim
import opensim.engine as engine


class TestSimInfectionCase(unittest.TestCase):

  def setUp(self):
    self.sim = engine.Simulator(file_name="infection.osm")
    self.num_vars = 14


  def test_sim_created(self):
    '''
    test to make sure a simulator is being correctly created from file.
    '''
    self.assert_(self.sim is not None)
    self.assert_(isinstance(self.sim, engine.Simulator))


  def test_loaded_vars(self):
    '''
    test to make sure we're loading the correct number of variables.
    '''
    sim_vars = self.sim.get_vars()

    self.assert_(sim_vars is not None)
    self.assert_(isinstance(sim_vars, list))
    self.assert_(len(sim_vars) == self.num_vars, 'expected %d, not %d vars' \
                 % (self.num_vars, len(sim_vars)))



def suite():
  '''
  Get a unittest.TestSuite containing all the tests in this module.
  '''
  sim_suite = unittest.TestLoader().loadTestsFromTestCase(TestSimInfectionCase)

  return unittest.TestSuite([sim_suite])


if __name__ == '__main__':
  # give us more verbose output than the standard unittest.main()
  unittest.TextTestRunner(verbosity=2).run(suite())

