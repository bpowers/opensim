//===--- Simulator.cpp - Base class for interacting with models ----------===//
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

#include <map>
#include <vector>
#include <string>

#include "globals.h"
using std::map;
using std::string;
using std::vector;

#include "CodeGen/SimBuilder.h"
using OpenSim::SimBuilder;

#include "model-simulator.h"
#include "model-variable.h"
#include "IO/model-ioxml.h"

#define PARAM_READWRITE (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT)
#define MODEL_SIMULATOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), MODEL_TYPE_SIMULATOR, ModelSimulatorPrivate))

static gpointer model_simulator_parent_class = NULL;
static void model_simulator_init(ModelSimulator *self);
static void model_simulator_class_init(ModelSimulatorClass *klass);
static void model_simulator_dispose(GObject *gobject);
static void model_simulator_finalize(GObject *gobject);

static int model_simulator_default_output_debug_info(ModelSimulator *simulator);
static int model_simulator_default_run(ModelSimulator *simulator);

enum
{
  PROP_0,

  PROP_MODEL_NAME,
  PROP_FILE_NAME,
  PROP_OUTPUT_TYPE,
  PROP_OUTPUT_FILE_NAME,
  PROP_VALID_MODEL
};


struct _ModelSimulatorPrivate
{
  gchar      *model_name;
  gchar      *file_name;
  sim_output  output_type;
  gchar      *output_file_name;
  gboolean    valid_model;
  
  GArray     *var_array;
  
  OpenSim::SimBuilder *sim_builder;
  std::map<std::string, ModelVariable *> var_map;
};


GType 
model_simulator_get_type()
{
  static GType g_define_type_id = 0; 
  if (G_UNLIKELY(g_define_type_id == 0)) 
    { 
      static const GTypeInfo g_define_type_info = { 
        sizeof (ModelSimulatorClass), 
        (GBaseInitFunc) NULL, 
        (GBaseFinalizeFunc) NULL, 
        (GClassInitFunc) model_simulator_class_init, 
        (GClassFinalizeFunc) NULL, 
        NULL,   // class_data 
        sizeof (ModelSimulator), 
        0,      // n_preallocs 
        (GInstanceInitFunc) model_simulator_init, 
      }; 
      g_define_type_id = g_type_register_static(G_TYPE_OBJECT, 
                                                "ModelSimulatorType", 
                                                &g_define_type_info, 
                                                (GTypeFlags) 0); 
    } 
  return g_define_type_id; 

}



static void
model_simulator_set_property(GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  ModelSimulator *self = MODEL_SIMULATOR(object);

  switch (property_id)
  {
  case PROP_MODEL_NAME:
    g_return_if_fail(G_VALUE_HOLDS_STRING(value));
    g_free(self->priv->model_name);
    self->priv->model_name = g_value_dup_string(value);
    //g_print("model_name: %s\n", self->priv->model_name);
    break;

  case PROP_FILE_NAME:
    g_return_if_fail(G_VALUE_HOLDS_STRING(value));
    g_free(self->priv->file_name);
    self->priv->file_name = g_value_dup_string(value);
    //g_print("file_name: %s\n", self->priv->sketch_name);
    break;

  case PROP_OUTPUT_TYPE:
    g_return_if_fail(G_VALUE_HOLDS_INT(value));
    self->priv->output_type = (sim_output)g_value_get_int(value);
    //g_print("file_name: %s\n", self->priv->sketch_name);
    break;

  case PROP_OUTPUT_FILE_NAME:
    g_return_if_fail(G_VALUE_HOLDS_STRING(value));
    g_free(self->priv->output_file_name);
    self->priv->output_file_name = g_value_dup_string(value);
    //g_print("file_name: %s\n", self->priv->sketch_name);
    break;

  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}



static void
model_simulator_get_property (GObject    *object,
                              guint       property_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  ModelSimulator *self = MODEL_SIMULATOR(object);

  switch (property_id)
  {
  case PROP_MODEL_NAME:
    g_value_set_string(value, self->priv->model_name);
    break;

  case PROP_FILE_NAME:
    g_value_set_string(value, self->priv->file_name);
    break;

  case PROP_OUTPUT_TYPE:
    g_value_set_int(value, (int)self->priv->output_type);
    break;

  case PROP_OUTPUT_FILE_NAME:
    g_value_set_string(value, self->priv->output_file_name);
    break;

  case PROP_VALID_MODEL:
    g_value_set_boolean(value, self->priv->valid_model);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}



static void
model_simulator_class_init(ModelSimulatorClass *klass)
{
  g_print("class init\n");

  model_simulator_parent_class = g_type_class_peek_parent(klass);

  g_type_class_add_private(klass, sizeof (ModelSimulatorPrivate));

  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *model_param_spec;
  
  gobject_class->set_property = model_simulator_set_property;
  gobject_class->get_property = model_simulator_get_property;
  gobject_class->dispose      = model_simulator_dispose;
  gobject_class->finalize     = model_simulator_finalize;

  klass->output_debug_info    = model_simulator_default_output_debug_info;
  klass->run                  = model_simulator_default_run;

  model_param_spec = g_param_spec_string("model_name",
                                         "model name",
                                         "Set model's name",
                                         "unnamed model" /* default value */,
                                         PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_MODEL_NAME,
                                  model_param_spec);

  model_param_spec = g_param_spec_string("file_name",
                                         "full path to file",
                                         "Where the model is saved to",
                                         "" /* default value */,
                                         PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_FILE_NAME,
                                  model_param_spec);

  model_param_spec = g_param_spec_int("output_type",
                                      "type of output",
                                      "What kind of output to generate",
                                      0, 
                                      sizeof(sim_output)+1,
                                      sim_emit_Output /* default value */,
                                      PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_OUTPUT_TYPE,
                                  model_param_spec);

  model_param_spec = g_param_spec_string("output_file_name",
                                         "full path to output file",
                                         "Where the model output is saved to",
                                         "" /* default value */,
                                         PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_OUTPUT_FILE_NAME,
                                  model_param_spec);

  model_param_spec = g_param_spec_boolean("valid_model",
                                          "is model valid",
                                          "True if the model can be simulated",
                                          TRUE /* default value */,
                                          (GParamFlags) (G_PARAM_READABLE));
  g_object_class_install_property(gobject_class,
                                  PROP_VALID_MODEL,
                                  model_param_spec);
}



static void
model_simulator_init(ModelSimulator *self)
{
  self->priv = MODEL_SIMULATOR_GET_PRIVATE(self);
  
  g_print("sim init\n");
  self->priv->valid_model = FALSE;
  self->priv->sim_builder = NULL;
  /*
  self->priv->model_name       = NULL;
  self->priv->file_name        = NULL;
  self->priv->output_type      = sim_emit_Output;
  self->priv->output_file_name = NULL;
  self->priv->valid_model      = FALSE;
  */
}



static void
model_simulator_dispose(GObject *gobject)
{
  ModelSimulator *self = MODEL_SIMULATOR(gobject);

  /* 
   * In dispose, you are supposed to free all typesecifier before 'IOVenText'
model-simulator.cpp:343: error: cannot convert 'in referenced from this
   * object which might themselves hold a reference to self. Generally,
   * the most simple solution is to unref all members on which you own a 
   * reference.
   */

  /* dispose might be called multiple times, so we must guard against
   * calling g_object_unref() on an invalid GObject.
   */
  if (self->priv->var_array)
  {
    GArray *array = self->priv->var_array;
    
    int i;
    for (i=0; i<array->len; i++)
    {
      //g_fprintf(stderr, "freeing some var\n");
      ModelVariable *var = NULL;
      var = g_array_index(array, ModelVariable *, i);
      if (var)
      {
        g_object_unref(var);
        array->data[i*sizeof(ModelVariable *)] = 0;
      }
    }
  }

  /* Chain up to the parent class */
  G_OBJECT_CLASS(model_simulator_parent_class)->dispose(gobject);
}



static void
model_simulator_finalize(GObject *gobject)
{
  ModelSimulator *self = MODEL_SIMULATOR(gobject);

  // free g_values and such.
  g_free(self->priv->model_name);
  g_free(self->priv->file_name);
  g_free(self->priv->output_file_name);
  
  if (self->priv->var_array)
    g_array_free(self->priv->var_array, TRUE);

  /* Chain up to the parent class */
  G_OBJECT_CLASS(model_simulator_parent_class)->finalize(gobject);
}



extern "C" int 
model_simulator_load(ModelSimulator *simulator, gchar *model_path)
{
  ModelIOxml *gio = MODEL_IOXML(g_object_new(MODEL_TYPE_IOXML, 
                                             NULL));
  gboolean valid_model = FALSE;
  gchar *prop;
  
  model_ioxml_load(gio, (gchar *)model_path);

  g_object_get(G_OBJECT(gio), "model_name", &prop,
                              "valid",      &valid_model, NULL);
  g_object_set(G_OBJECT(simulator), "model_name", prop, NULL);
  g_free(prop);

  GArray *vars = model_ioxml_get_variables(gio);
  
  if (!vars) fprintf(stderr, "Warning: variable array not available from IOxml.\n");
  
  simulator->priv->var_array = vars;

  g_object_unref(gio);
  
  SimBuilder *_sim_builder = simulator->priv->sim_builder;
  std::map<std::string, ModelVariable *> _variables;

  // turn our nice list into an ugly map.
  int i;
  for (i=0; i<vars->len; i++)
  {
    //g_fprintf(stderr, "freeing some var\n");
    ModelVariable *var = g_array_index(vars, ModelVariable *, i);
    gchar *var_name = NULL;

    g_object_get(G_OBJECT(var), "name", &var_name, NULL);

    _variables[var_name] = var;
    
    g_free(var_name);
  }

  if (_sim_builder)
  {
    delete _sim_builder;
    _sim_builder = NULL;
  }
  
  if (valid_model)
  {
    fprintf(stdout, "Info: We seem to have a valid model so far.\n");
    _sim_builder = new SimBuilder(_variables);
  }
  
  simulator->priv->var_map     = _variables;
  simulator->priv->sim_builder = _sim_builder;
}



extern "C" int 
model_simulator_output_debug_info(ModelSimulator *simulator)
{
  return MODEL_SIMULATOR_GET_CLASS(simulator)->output_debug_info(simulator);
}



int 
model_simulator_default_output_debug_info(ModelSimulator *simulator)
{
  fprintf(stdout, "Info: outputting debugging info\n");
  
  if (simulator->priv->var_array)
  { 
    GArray *array = simulator->priv->var_array;

    fprintf(stdout, "  found variable array of size %d (%d)\n", array->len,
            simulator->priv->var_map.size());
    
    int i;
    for (i=0; i<array->len; i++)
    {
      //g_fprintf(stderr, "freeing some var\n");
      ModelVariable *var = NULL;
      var = g_array_index(array, ModelVariable *, i);
      gchar *var_name = NULL;
      gchar *equation = NULL;
      
      g_object_get(G_OBJECT(var), "name",     &var_name, 
                                  "equation", &equation, NULL);
      fprintf(stdout, "    var '%s'\n    '%s'\n", var_name, equation);
      
      const GArray *toks = model_variable_get_tokens(var);
      
      int i;
      for (i=0; i<toks->len; i++)
      {
        //g_fprintf(stderr, "freeing some var\n");
        equ_token tok = g_array_index(toks, equ_token, i);
        
        fprintf(stdout, "      tok ('%c' '%d') '%s' (%f)\n", 
                tok.op, tok.type, 
                tok.identifier, tok.num_val);
      }
      
      g_free(var_name);
      g_free(equation);
    }
  }
  else
  {
    fprintf(stdout, "  no array of variables.\n");
  }
  
  return 0;
}



extern "C" int
model_simulator_run(ModelSimulator *simulator)
{
  return MODEL_SIMULATOR_GET_CLASS(simulator)->run(simulator);
}

int 
model_simulator_default_run(ModelSimulator *self)
{
  fprintf(stdout, "simulating the model\n");
  return 0;
}
