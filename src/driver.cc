//===--- main.cpp - OpenSim command line driver --------------------------===//
//
// Copyright 2008 Bobby Powers, portions copyright Free Software 
//   Foundation, Inc.
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
// The argument parsing is largly copied from the GNU Hello World project.
//
//===---------------------------------------------------------------------===//

// project specific defines
#include "opensim/config.h"
#include "opensim/runtime.h"

// standard library
#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "sys/time.h"

#include "llvm/Support/ManagedStatic.h"
#include "llvm/System/Signals.h"
using namespace llvm;

#include <iostream>
#include <fstream>
#include <memory>
using std::cerr;

const char *program_name;


static int print_help ();
static int print_version ();

int 
main (int argc, char *argv[]) 
{
  program_name = argv[0];

  // Call llvm_shutdown() on exit.
  llvm_shutdown_obj shutdown;
  try
  {
    if (argc == 1)
      return print_help ();

    if (!strcmp (argv[1], "--version"))
      return print_version ();

    opensim::Runtime model;
    for (int i=1; i<argc; ++i)
      model.loadFile(argv[i]);

    model.simulate();

  }
  catch (const std::string& msg)
  {
    cerr << argv[0] << ": " << msg << "\n";
  }
  catch (...)
  {
    cerr << argv[0] << ": Unexpected unknown exception occurred.\n";
  }

  return 0;
}



static int
print_help()
{
  printf("\
Usage: %s [-htvop] input_file\n", program_name);

  fputs("\
Simulate system dynamics models.\n\n\
Options:\n", stdout);
  
  puts("");
  
  fputs("\
  -h, --help          display this help and exit\n\
  -v, --version       display version information and exit\n", stdout);
  
  puts("");

  fputs("\
  -t, --target=LANG   output model in the specified language\n\
                        Supported langages:\n\
                          python\n\
                          as3\n\
                          llvm-ir\n\
                          interpret [DEFUALT]\n", stdout);
  
  fputs("\
  -o, --output=FILE   output model to the specified file\n", stdout);  

  fputs("\
  -p, --performance   output time taken to run model in ms\n", stdout);

  printf("\n");

  printf("\
Report bugs to <%s>.\n", PACKAGE_BUGREPORT);

  return 0;
}



static int
print_version ()
{
  printf("opensim (%s) %s\n", PACKAGE, VERSION);
  
  puts("");
  
  // FSF recommends seperating out the year, for ease in translations.
  printf("\
Copyright (C) %s Bobby Powers.\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\n",
"2008");

  return 0;
}
