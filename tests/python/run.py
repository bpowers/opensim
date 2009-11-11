#!/usr/bin/env python
#===--- run.py - Run all our unit tests ----------------------------------===#
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

import os, re, unittest

__test_regex = re.compile('^(test_\w+)\.py$')
__tests = []

# import all tests from python files named test_*.py
for file in os.listdir('.'):
  match = __test_regex.search(file)
  if match:
    test_file = match.group(1)
    mod = __import__(test_file)
    suite = getattr(mod, 'suite')
    __tests.append(suite())

# now create a test suite for all the unit tests we imported
full_suite = unittest.TestSuite(__tests)


if __name__ == '__main__':
  # give us more verbose output than the standard unittest.main()
  unittest.TextTestRunner(verbosity=2).run(full_suite)
