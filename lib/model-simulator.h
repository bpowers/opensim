//===--- Simulator.h - Base class for interacting with models  -*- C++ -*-===//
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
// This class represents models at a high level, suitible for inclusion
// in projects as part of a library.
// TODO: implement features for dynamically changing models.
//
//===---------------------------------------------------------------------===//

#ifndef __MODEL_SIMULATOR_H__
#define __MODEL_SIMULATOR_H__

#include <glib.h>
#include <glib-object.h>


G_BEGIN_DECLS


enum sim_output 
{
  sim_emit_IR      = 1, // not supported
  sim_emit_Python  = 2, // full Python implementation of model
  sim_emit_Fortran = 3, // not implemented yet
  sim_emit_Output  = 4, // results of interpreting model
  sim_emit_AS3     = 5, // full AS3 implementation of model
};

/*
 * Type macros.
 */
#define MODEL_TYPE_SIMULATOR            (model_simulator_get_type())
#define MODEL_SIMULATOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), MODEL_TYPE_SIMULATOR, ModelSimulator))
#define MODEL_SIMULATOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), MODEL_TYPE_SIMULATOR, ModelSimulatorClass))
#define MODEL_IS_SIMULATOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), MODEL_TYPE_SIMULATOR))
#define MODEL_IS_SIMULATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), MODEL_TYPE_SIMULATOR))
#define MODEL_SIMULATOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), MODEL_TYPE_SIMULATOR, ModelSimulatorClass))

typedef struct _ModelSimulator        ModelSimulator;
typedef struct _ModelSimulatorClass   ModelSimulatorClass;
typedef struct _ModelSimulatorPrivate ModelSimulatorPrivate;

struct _ModelSimulator
{
  GObject parent_instance;
  
  /*
   * Properties:
   *   - model_name       (gchar *)    get/set
   *   - sketch_name      (gchar *)    get/set
   *   - file_name        (gchar *)    get/set
   *   - output_type      (sim_output) get/set
   *   - output_file_name (gchar *)    get/set
   *   - valid_model      (gboolean)   get
   */
   
  /* instance members */
  ModelSimulatorPrivate *priv;
};

struct _ModelSimulatorClass
{
  GObjectClass parent_class;
  
  /* class members */
};

/* used by MODEL_TYPE_SIMULATOR */
GType model_simulator_get_type();

/*
 * Method definitions.
 */

/* public */
int model_simulator_save(ModelSimulator *simulator);
//int model_simulator_save(ModelSimulator *simulator, gboolean partial);

int model_simulator_new_variable(ModelSimulator *simulator, gchar *var_name, 
                                 gpointer var_pointer);
int model_simulator_get_variable(ModelSimulator *simulator, gchar *var_name, 
                                 gpointer var_pointer);
int model_simulator_remove_variable(ModelSimulator *simulator, 
                                    gchar *var_name);

int model_simulator_run(ModelSimulator *simulator);


G_END_DECLS

#endif /* __MODEL_SIMULATOR_H__ */

