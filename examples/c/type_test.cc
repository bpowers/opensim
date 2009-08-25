//===--- type_test.cpp - test the Types implementation ------------------===//
//
// Copyright 2008 Bobby Powers.
//
// This file is part of OpenSim.
//
// OpenSim is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenSim is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenSim.  If not, see <http://www.gnu.org/licenses/>.
//
//===---------------------------------------------------------------------===//
//
// This isn't unit tests, but a testbed to get the Object/Type system
// functioning correctly.
//
//===---------------------------------------------------------------------===//

#include "opensim/types.h"

#include <cstdio>
#include <unistd.h>


const char *program_name;


int
main (int argc, char *argv[])
{
  program_name = argv[0];

  try
  {
    printf("testing types\n");

  }
  catch (const std::string& msg)
  {
    fprintf(stderr, "%s: %s\n", program_name, msg.c_str());
  }
  catch (...)
  {
    fprintf(stderr, "%s: Unexpected unknown exception occurred.\n",
            program_name);
  }

  return 0;
}

