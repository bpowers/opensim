//===--- main.cpp - OpenSim command line driver --------------------------===//
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
// This file contains the command line driver for the OpenSim library, 
// allowing you to open OpenSim XML models and translate them to standard
// output in one of several languages.
//
//===---------------------------------------------------------------------===//

// project specific defines
//#include "opensim/config.h"
#include "opensim/runtime.h"
#include "opensim/types.h"
using namespace opensim;
using namespace opensim::types;


int 
main (int argc, char *argv[]) 
{
  startup::opensim_init();
  //try
  {
    Namespace root(NULL);

    Model goats("goats");
  }
  /*
  catch (const std::string& msg)
  {
    fprintf(stderr, "%s: %s\n", argv[0], msg.c_str());
  }
  catch (...)
  {
    fprintf(stderr, "%s: Unexpected unknown exception occurred.\n", argv[0]);
  }
  */

  startup::opensim_exit();
  return 0;
}

