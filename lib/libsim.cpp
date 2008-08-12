//===--- libsim.cpp - Implements the shared library ----------------------===//
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

// for the windows ifdef
#include "globals.h"
#include <cstdio>

#include "model-simulator.h"
#include "Simulator.h"
using OpenSim::Simulator;
//using OpenSim::sim_output;

Simulator *model;

#ifdef _WIN32
BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch( ul_reason_for_call ) 
	{
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
		model = NULL;
		break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
		// Bah cases errors.  Just leave junk around for now.
		//delete model;
		break;
    }
    return TRUE;
}

#else

void __attribute__ ((constructor)) 
my_init(void)
{
  model = NULL;
  
  fprintf(stderr, "creating model\n");
  
  g_type_init_with_debug_flags((GTypeDebugFlags) G_TYPE_DEBUG_MASK);
  
  ModelSimulator *gsim = MODEL_SIMULATOR(g_object_new(MODEL_TYPE_SIMULATOR, NULL)); 
  
  g_object_unref(gsim);
  
  fprintf(stderr, "done creating model\n");
}



void __attribute__ ((destructor)) 
my_fini(void)
{
  delete model;
}
#endif


extern "C" int WIN_DLL
opensim_load_model(const char *filename)
{
  delete model;
  
  model = new Simulator(filename);
  
  return 0;
}


extern "C" int WIN_DLL
opensim_save_model()
{
  return 0;
}



extern "C" int WIN_DLL
opensim_set_output_type(OpenSim::sim_output output_type)
{
  if (model) return model->set_output_type(output_type);

  return -1;
}



extern "C" int WIN_DLL
opensim_set_output_file(const char *file_name)
{
  if (model) return model->set_output_file(file_name);
  
  return -1;
}



extern "C" int WIN_DLL
opensim_simulate()
{
  if (model) return model->simulate();
  
  return -1;
}
