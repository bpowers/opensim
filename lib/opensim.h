//===--- opensim.h - Defines the interface to the shared library ---------===//
//
// Copyright 2008 Bobby Powers
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
// This file defines the interface to the libsim shared library, which 
// can be used to create, edit and simulate System Dynamics models.
//
//===---------------------------------------------------------------------===//

#ifndef LIBSIM_H
#define LIBSIM_H

#ifdef __cplusplus
extern "C" {
#endif


/// sim_output - This enum is for specifying the kind of output the simulate 
/// function produces
///
enum sim_output 
{
  sim_emit_IR = 1,      // not supported
  sim_emit_Python = 2,  // full Python implementation of model
  sim_emit_Fortran = 3, // not implemented yet
  sim_emit_Output = 4,  // results of interpreting model
  sim_emit_AS3 = 5,     // full AS3 implementation of model
};

extern int opensim_load_model(const char *filename);
extern int opensim_save_model();

extern int opensim_set_output_type(enum sim_output output_type);
extern int opensim_set_output_file(const char *file_name);

extern int opensim_simulate();


#ifdef __cplusplus
}
#endif

#endif // LIBSIM_H

