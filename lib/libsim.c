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
#include <stdio.h>

#include "model-simulator.h"
#include "model-variable.h"
ModelSimulator *gsim = NULL;


#ifdef _WIN32
BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
  switch( ul_reason_for_call ) 
  {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
    //model = NULL;
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
  fprintf(stderr, "creating model\n");
  
  g_type_init_with_debug_flags((GTypeDebugFlags) G_TYPE_DEBUG_MASK);
  
  gsim = MODEL_SIMULATOR(g_object_new(MODEL_TYPE_SIMULATOR, 
                                      NULL)); 
  gchar *prop;
  
  g_object_get(G_OBJECT(gsim), "model_name", &prop, NULL);
  g_print("model_name is now: %s\n", prop);
  g_free(prop);
  
  sim_output out;
  
  g_object_get(G_OBJECT(gsim), "output_type", &out, NULL);
  g_print("output_type is now: %d\n", out);

  g_object_set(G_OBJECT(gsim), "output_type", sim_emit_Output, NULL);
  g_object_get(G_OBJECT(gsim), "output_type", &out, NULL);
  g_print("output_type is now: %d\n", out);
  
  fprintf(stderr, "done creating model\n");
  
  ModelVariable *gvar = MODEL_VARIABLE(g_object_new(MODEL_TYPE_VARIABLE, 
                                                    NULL));
  g_object_unref(gvar);
}



void __attribute__ ((destructor)) 
my_fini(void)
{
  g_object_unref(gsim);
}
#endif


int WIN_DLL
opensim_load_model(const char *file_name)
{
  //delete model;
  //model = new Simulator(file_name);
  
  model_simulator_load(gsim, (gchar *)file_name);
  
  gchar *prop;
  
  g_object_get(G_OBJECT(gsim), "model_name", &prop, NULL);
  g_print("model_name is now: %s\n", prop);
  g_free(prop);
  
  model_simulator_output_debug_info(gsim);
  
  return 0;
}


int WIN_DLL
opensim_save_model()
{
  return -1;
}



int WIN_DLL
opensim_set_output_type(sim_output output_type)
{
  
  return -1;
}



int WIN_DLL
opensim_set_output_file(const char *file_name)
{
  
  return -1;
}



int WIN_DLL
opensim_simulate()
{
  if (gsim) return model_simulator_run(gsim);
  
  return -1;
}
