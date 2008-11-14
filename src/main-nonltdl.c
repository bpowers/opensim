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
#include "config.h"

// standard library
#include "stdio.h"
#include "string.h"
#include "getopt.h"
#include "unistd.h"
#include "sys/time.h"

// openSim headers
#include "opensim.h"


const char *program_name;

static const struct option longopts[] =
{
  { "target", required_argument, NULL, 't' },
  { "output", required_argument, NULL, 'o' },
  { "help", no_argument, NULL, 'h' },
  { "performance", no_argument, NULL, 'p' },
  { "version", no_argument, NULL, 'v' },
  { NULL, 0, NULL, 0 }
};



static void print_help();
static void print_version();



int 
main (int argc, const char * argv[]) 
{
  // definitions for argument parsing
  int optc;
  int lose = 0;
  int got_input_file = 0;
  int too_many_inputs = 0;
  int specify_target = 0;
  int specify_output = 0;
  int test = 0;
  struct timeval time_start;
  struct timeval time_end;
  const char *input_file = NULL;
  const char *target = NULL;
  const char *output = NULL;
  
  program_name = argv[0];
  
  // main processing loop for argument parsing.
  // getopt will loop through all of the arguments,
  // returning the ones it knows.
  while ((optc = getopt_long (argc, (char * const *)argv, 
                              "t:o:hvp", longopts, NULL)) != -1)
  {
    switch (optc)
    {
      // GNU standards have --help and --version exit immediately.
      case 'v':
        print_version();
        return 0;
        break;
      case 't':
        target = (const char *) optarg;
        specify_target = 1;
        break;
      case 'h':
        print_help ();
        return 0;
        break;
      case 'o':
        output = (const char *) optarg;
        specify_output = 1;
        break;
      case 'p':
        test = 1;
        break;
      default:
        lose = 1;
        break;
    }
  }
  
  if (optind == argc-1)
  {
    input_file = argv[optind];
    got_input_file = 1;
  }
  else if (optind < argc-1 && !lose)
    too_many_inputs = 1;
  
  // either we have an unknown operand or something similarly
  // bad happened.
  if (lose || !got_input_file)
  {
    // output a slightly different message depending on what the user
    // did to get here.  They could have either listed too many files,
    // too few files, or a bad option
    if (too_many_inputs)
      fprintf (stderr, "%s: only one input file may be specified.\n",
               program_name);
    else if (!got_input_file && !lose)
      fprintf (stderr, "%s: no input file specified.\n",
               program_name);
    
    fprintf (stderr, "Try `%s --help' for more information.\n",
             program_name);
    return -1;
  }
   
  // default output target (for now) is python
  enum sim_output new_walk = sim_emit_Output;
  
  // handle explicitly requesting a target type
  if (specify_target)
  {
    if (!strcmp (target, "python"))
    {
      new_walk = sim_emit_Python;
    }
    else if (!strcmp (target, "as3"))
    {
      new_walk = sim_emit_AS3;
    }
    else if (!strcmp (target, "llvm-ir"))
    {
      new_walk = sim_emit_IR;
    }
    else if (!strcmp (target, "fortran"))
    {
      new_walk = sim_emit_Fortran;
      
      // just remove these lines in a few days when we support Fortran
      fprintf (stderr, "%s: Fortran support will be here soon!.\n",
               program_name);
      
      fprintf (stderr, "Try `%s --help' for more information.\n",
               program_name);
      return -1;
    }
    else if (!strcmp (target, "interpret"))
    {
      new_walk = sim_emit_Output;
    }
    else
    {
      fprintf (stderr, "%s: invalid target language:%s.\n",
               program_name, target);
      
      fprintf (stderr, "Try `%s --help' for more information.\n",
               program_name);
      return -1;
    }
  }
  
  if (test) gettimeofday (&time_start, NULL);  
  
  opensim_load_model (input_file);
  opensim_set_output_type (new_walk);
  
  if (specify_output)
  {
    opensim_set_output_file (output);
  }

  // for a performance test, we should direct output to /dev/null
  if (test) opensim_set_output_file ("/dev/null");

  
  opensim_simulate ();
  
  if (test)
  {
    gettimeofday (&time_end, NULL);
    
    int time_ms = (time_end.tv_usec - time_start.tv_usec) + 
                  (time_end.tv_sec - time_start.tv_sec) * 1e6;
    
    fprintf (stdout, "%d\n", time_ms);
  }
  return 0;
}



static void
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
}



static void
print_version (void)
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
}
