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

#include <string>
#include <map>
#include <vector>


enum sim_output 
{
  sim_emit_IR = 1,      // not supported
  sim_emit_Python = 2,  // full Python implementation of model
  sim_emit_Fortran = 3, // not implemented yet
  sim_emit_Output = 4,  // results of interpreting model
  sim_emit_AS3 = 5,     // full AS3 implementation of model
};

/*
 * Type macros.
 */
#define MODEL_TYPE_SIMULATOR                  (model_simulator_get_type())
#define MODEL_SIMULATOR(obj)                  (G_TYPE_CHECK_INSTANCE_CAST((obj), MODEL_TYPE_SIMULATOR, ModelSimulator))
#define MODEL_IS_SIMULATOR(obj)               (G_TYPE_CHECK_INSTANCE_TYPE((obj), MODEL_TYPE_SIMULATOR))
#define MODEL_SIMULATOR_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST((klass), MODEL_TYPE_SIMULATOR, ModelSimulatorClass))
#define MODEL_IS_SIMULATOR_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE((klass), MODEL_TYPE_SIMULATOR))
#define MODEL_SIMULATOR_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS((obj), MODEL_TYPE_SIMULATOR, ModelSimulatorClass))

typedef struct _ModelSimulator        ModelSimulator;
typedef struct _ModelSimulatorClass   ModelSimulatorClass;

struct _ModelSimulator
{
  GObject parent_instance;
  
  /* instance members */
  /* private */
  gchar *_model_name;
  gchar *_model_file_name;
  gchar *_output_file_name;
  sim_output _output_type;
  OpenSim::SimBuilder *_sim_builder;
  std::map<std::string, OpenSim::Variable *> _variables;
  std::map<std::string, std::vector<double> > _results;
  FILE *_output_stream;
  int _parse_status;
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

/* private */
gchar *clean_name(gchar *varName);
void model_simulator_init(gchar *fileName);
void model_simulator_sim_thread();

/* public */
int model_simulator_set_name(gchar *modelName);
gchar *model_simulator_name();

int model_simulator_set_model_file(gchar *modelFileName);
gchar *model_simulator_model_file();

int model_simulator_save();
int model_simulator_save(gboolean partial);

int model_simulator_set_output_type(sim_output newType);

int model_simulator_set_output_file(gchar *outputFileName);
gchar *model_simulator_output_file();

int model_simulator_set_variable_equation(gchar *varName, gchar *varEqn);
gchar *model_simulator_get_variable_equation(gchar *varName);

int model_simulator_new_variable(gchar *varName);
int model_simulator_rename_variable(gchar *varName, gchar *newName);
int model_simulator_delete_variable(gchar *varName);

int info();

int simulate();

#endif /* __MODEL_SIMULATOR_H__ */
