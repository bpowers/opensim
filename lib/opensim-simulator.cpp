//===--- Simulator.cpp - Base class for interacting with opensims ----------===//
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
// This class represents opensims at a high level, suitible for inclusion
// in projects as part of a library.
// TODO: implement features for dynamically changing opensims.
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

#include "opensim-simulator.h"
#include "opensim-variable.h"
#include "IO/opensim-ioxml.h"

#define PARAM_READWRITE (GParamFlags) \
        (G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT)
#define OPENSIM_SIMULATOR_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), OPENSIM_TYPE_SIMULATOR, \
                                            OpensimSimulatorPrivate))

static gpointer opensim_simulator_parent_class = NULL;
static void opensim_simulator_init(OpensimSimulator *self);
static void opensim_simulator_class_init(OpensimSimulatorClass *klass);
static void opensim_simulator_dispose(GObject *gobject);
static void opensim_simulator_finalize(GObject *gobject);

static int opensim_simulator_default_run(OpensimSimulator *simulator);
static int opensim_simulator_default_load(OpensimSimulator *simulator, 
                                          gchar *model_path);
static int opensim_simulator_default_save(OpensimSimulator *simulator);

static OpensimVariable *opensim_simulator_default_new_variable(
                                         OpensimSimulator *simulator, 
                                         gchar *var_name, 
                                         gchar *var_eqn);
static OpensimVariable *opensim_simulator_default_get_variable(
                                         OpensimSimulator *simulator, 
                                         gchar *var_name);
static int opensim_simulator_default_remove_variable(
                                         OpensimSimulator *simulator, 
                                         gchar *var_name);

static int opensim_simulator_default_output_debug_info(
                                         OpensimSimulator *simulator);


extern "C" GType
opensim_output_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if (g_once_init_enter (&g_define_type_id__volatile))
  {
    static const GEnumValue values[] = {
      { sim_emit_IR, "sim_emit_IR", "llvm" },
      { sim_emit_Python, "sim_emit_Python", "python" },
      { sim_emit_Fortran, "sim_emit_Fortran", "fortran" },
      { sim_emit_Output, "sim_emit_Output", "output" },
      { sim_emit_AS3, "sim_emit_AS3", "as3" },
      { 0, NULL, NULL }
    };
    GType g_define_type_id =
      g_enum_register_static (g_intern_static_string ("OpensimOutput"), values);
    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }

  return g_define_type_id__volatile;
}



enum
{
  PROP_0,

  PROP_MODEL_NAME,
  PROP_FILE_NAME,
  PROP_OUTPUT_TYPE,
  PROP_OUTPUT_FILE_NAME,
  PROP_VALID_MODEL
};


struct _OpensimSimulatorPrivate
{
  gchar      *model_name;
  gchar      *file_name;
  int  output_type;
  gchar      *output_file_name;
  gboolean    valid_model;
  
  GArray     *var_array;
  
  OpenSim::SimBuilder *sim_builder;
  std::map<std::string, OpensimVariable *> var_map;
};


GType 
opensim_simulator_get_type()
{
  static GType g_define_type_id = 0; 
  if (G_UNLIKELY(g_define_type_id == 0)) 
    { 
      static const GTypeInfo g_define_type_info = { 
        sizeof (OpensimSimulatorClass), 
        (GBaseInitFunc) NULL, 
        (GBaseFinalizeFunc) NULL, 
        (GClassInitFunc) opensim_simulator_class_init, 
        (GClassFinalizeFunc) NULL, 
        NULL,   // class_data 
        sizeof (OpensimSimulator), 
        0,      // n_preallocs 
        (GInstanceInitFunc) opensim_simulator_init, 
      }; 
      g_define_type_id = g_type_register_static(G_TYPE_OBJECT, 
                                                "OpensimSimulatorType", 
                                                &g_define_type_info, 
                                                (GTypeFlags) 0); 
    } 
  return g_define_type_id; 

}



static void
opensim_simulator_set_property(GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  OpensimSimulator *self = OPENSIM_SIMULATOR(object);

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
    self->priv->output_type = g_value_get_int(value);
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
opensim_simulator_get_property (GObject    *object,
                                guint       property_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  OpensimSimulator *self = OPENSIM_SIMULATOR(object);

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
opensim_simulator_class_init(OpensimSimulatorClass *klass)
{
  opensim_simulator_parent_class = g_type_class_peek_parent(klass);

  g_type_class_add_private(klass, sizeof (OpensimSimulatorPrivate));

  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *opensim_param_spec;
  
  gobject_class->set_property = opensim_simulator_set_property;
  gobject_class->get_property = opensim_simulator_get_property;
  gobject_class->dispose      = opensim_simulator_dispose;
  gobject_class->finalize     = opensim_simulator_finalize;

  klass->save                 = opensim_simulator_default_save;
  klass->new_variable         = opensim_simulator_default_new_variable;
  klass->get_variable         = opensim_simulator_default_get_variable;
  klass->output_debug_info    = opensim_simulator_default_output_debug_info;
  klass->run                  = opensim_simulator_default_run;

  opensim_param_spec = g_param_spec_string("model_name",
                                           "model name",
                                           "Set model's name",
                                           "unnamed model" /* default value */,
                                           PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_MODEL_NAME,
                                  opensim_param_spec);

  opensim_param_spec = g_param_spec_string("file_name",
                                         "full path to file",
                                         "Where the opensim is saved to",
                                         NULL /* default value */,
                                         PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_FILE_NAME,
                                  opensim_param_spec);

  opensim_param_spec = g_param_spec_int("output_type",
                                      "type of output",
                                      "What kind of output to generate",
                                      0, 
                                      5,
                                      sim_emit_Output /* default value */,
                                      PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_OUTPUT_TYPE,
                                  opensim_param_spec);

  opensim_param_spec = g_param_spec_string("output_file_name",
                                         "full path to output file",
                                         "Where the opensim output is saved to",
                                         NULL /* default value */,
                                         PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_OUTPUT_FILE_NAME,
                                  opensim_param_spec);

  opensim_param_spec = g_param_spec_boolean("valid_model",
                                          "is model valid",
                                          "True if the model can be simulated",
                                          TRUE /* default value */,
                                          (GParamFlags) (G_PARAM_READABLE));
  g_object_class_install_property(gobject_class,
                                  PROP_VALID_MODEL,
                                  opensim_param_spec);
}



static void
opensim_simulator_init(OpensimSimulator *self)
{
  self->priv = OPENSIM_SIMULATOR_GET_PRIVATE(self);
  
  self->priv->valid_model = FALSE;
  self->priv->sim_builder = NULL;
}



static void
opensim_simulator_dispose(GObject *gobject)
{
  OpensimSimulator *self = OPENSIM_SIMULATOR(gobject);

  /* 
   * In dispose, you are supposed to free all typesecifier before 'IOVenText'
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
      OpensimVariable *var = NULL;
      var = g_array_index(array, OpensimVariable *, i);
      if (var)
      {
        g_object_unref(var);
        array->data[i*sizeof(OpensimVariable *)] = 0;
      }
    }
  }

  /* Chain up to the parent class */
  G_OBJECT_CLASS(opensim_simulator_parent_class)->dispose(gobject);
}



static void
opensim_simulator_finalize(GObject *gobject)
{
  OpensimSimulator *self = OPENSIM_SIMULATOR(gobject);

  // free g_values and such.
  g_free(self->priv->model_name);
  g_free(self->priv->file_name);
  g_free(self->priv->output_file_name);
  
  if (self->priv->var_array)
    g_array_free(self->priv->var_array, TRUE);

  /* Chain up to the parent class */
  G_OBJECT_CLASS(opensim_simulator_parent_class)->finalize(gobject);
}



extern "C" int 
opensim_simulator_load(OpensimSimulator *simulator, gchar *model_path)
{
  OpensimIOxml *gio = OPENSIM_IOXML(g_object_new(OPENSIM_TYPE_IOXML, 
                                             NULL));
  gboolean valid_model = FALSE;
  gchar *prop;
  
  opensim_ioxml_load(gio, model_path);

  g_object_get(G_OBJECT(gio), "model_name", &prop,
                              "valid",      &valid_model, NULL);
  g_object_set(G_OBJECT(simulator), "model_name", prop, NULL);
  g_free(prop);

  GArray *vars = opensim_ioxml_get_variables(gio);
  
  if (!vars) fprintf(stderr, "Warning: variable array not available from IOxml.\n");
  
  simulator->priv->var_array = vars;

  g_object_unref(gio);
  
  SimBuilder *_sim_builder = simulator->priv->sim_builder;
  std::map<std::string, OpensimVariable *> _variables;

  // turn our nice list into an ugly map.
  int i;
  for (i=0; i<vars->len; i++)
  {
    //g_fprintf(stderr, "freeing some var\n");
    OpensimVariable *var = g_array_index(vars, OpensimVariable *, i);
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
    _sim_builder = new SimBuilder(_variables);
  }
  
  simulator->priv->var_map     = _variables;
  simulator->priv->sim_builder = _sim_builder;
}



extern "C" int 
opensim_simulator_output_debug_info(OpensimSimulator *simulator)
{
  return OPENSIM_SIMULATOR_GET_CLASS(simulator)->output_debug_info(simulator);
}



int 
opensim_simulator_default_output_debug_info(OpensimSimulator *simulator)
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
      OpensimVariable *var = NULL;
      var = g_array_index(array, OpensimVariable *, i);
      gchar *var_name = NULL;
      gchar *equation = NULL;
      
      g_object_get(G_OBJECT(var), "name",     &var_name, 
                                  "equation", &equation, NULL);
      fprintf(stdout, "    var '%s'\n    '%s'\n", var_name, equation);
      
      const GArray *toks = opensim_variable_get_tokens(var);
      
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
opensim_simulator_run(OpensimSimulator *simulator)
{
  return OPENSIM_SIMULATOR_GET_CLASS(simulator)->run(simulator);
}



int 
opensim_simulator_default_run(OpensimSimulator *self)
{
  int ret = 0;
  
  if (self->priv->sim_builder)
  {
    FILE *output_stream = stdout;
    gchar *output_file_name = self->priv->output_file_name;
    
    if (output_file_name) 
    {
      fprintf(stdout, "ofn: '%s' %d\n", output_file_name, g_strcmp0(output_file_name, ""));
      output_stream = fopen(output_file_name, "w+");
      
      if (!output_stream) 
      {
        fprintf(stderr, "Error: Could not open output file for writing.\n");
        return -1;
      }
    }
    
    ret = self->priv->sim_builder->Parse(self->priv->output_type, 
                                         output_stream);
    
    
    // if we opened it, close the output stream
    if (output_stream != stdout) fclose(output_stream);
  }
  
  return ret;
}



extern "C" int 
opensim_simulator_save(OpensimSimulator *simulator)
{
  return OPENSIM_SIMULATOR_GET_CLASS(simulator)->save(simulator);
}



static int 
opensim_simulator_default_save(OpensimSimulator *simulator)
{
  return -1;
}



extern "C" OpensimVariable *
opensim_simulator_new_variable(OpensimSimulator *simulator, 
                               gchar *var_name, 
                               gchar *var_eqn)
{
  return OPENSIM_SIMULATOR_GET_CLASS(simulator)->new_variable(simulator,
                                                              var_name,
                                                              var_eqn);
}



static OpensimVariable *
opensim_simulator_default_new_variable(OpensimSimulator *simulator, 
                                       gchar *var_name, 
                                       gchar *var_eqn)
{
  return NULL;
}



extern "C" OpensimVariable *
opensim_simulator_get_variable(OpensimSimulator *simulator, 
                               gchar *var_name)
{
  return OPENSIM_SIMULATOR_GET_CLASS(simulator)->get_variable(simulator,
                                                              var_name);
}



static OpensimVariable *
opensim_simulator_default_get_variable(OpensimSimulator *simulator, 
                                       gchar *var_name)
{
  return NULL;
}


                                       
extern "C" int 
opensim_simulator_remove_variable(OpensimSimulator *simulator, 
                                          gchar *var_name)
{
  return OPENSIM_SIMULATOR_GET_CLASS(simulator)->remove_variable(simulator,
                                                                 var_name);
}


                                       
static int 
opensim_simulator_default_remove_variable(OpensimSimulator *simulator, 
                                          gchar *var_name)
{
  return -1;
}

