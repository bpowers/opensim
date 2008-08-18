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

#include "globals.h"
using std::map;
using std::string;
using std::vector;

#include "CodeGen/SimBuilder.h"
#include "IO/IOxml.h"
#include "IO/IOVenText.h"
#include "IO/IOInterface.h"
using OpenSim::SimBuilder;
using OpenSim::IOxml;
using OpenSim::IOVenText;
using OpenSim::IOInterface;

#include "model-simulator.h"
#include "IO/model-ioxml.h"

#define PARAM_READWRITE (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT)
#define MODEL_SIMULATOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), MODEL_TYPE_SIMULATOR, ModelSimulatorPrivate))

static gpointer model_simulator_parent_class = NULL;
extern "C" void model_simulator_init(ModelSimulator *self);
extern "C" void model_simulator_class_init(ModelSimulatorClass *kclass);
extern "C" void model_simulator_dispose(GObject *gobject);
extern "C" void model_simulator_finalize(GObject *gobject);

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
  
  OpenSim::SimBuilder *sim_builder;
  std::map<std::string, OpenSim::Variable *> variables;
};


extern "C" GType 
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



extern "C" void
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



extern "C" void
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



extern "C" void
model_simulator_class_init(ModelSimulatorClass *kclass)
{
  g_print("class init\n");

  model_simulator_parent_class = g_type_class_peek_parent(kclass);

  g_type_class_add_private(kclass, sizeof (ModelSimulatorPrivate));

  GObjectClass *gobject_class = G_OBJECT_CLASS(kclass);
  GParamSpec *model_param_spec;
  
  gobject_class->set_property = model_simulator_set_property;
  gobject_class->get_property = model_simulator_get_property;
  gobject_class->dispose      = model_simulator_dispose;
  gobject_class->finalize     = model_simulator_finalize;


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



extern "C" void
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



extern "C" void
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
  //if (self->priv->an_object)
  //{
  //  g_object_unref (self->priv->an_object);
  //
  //  self->priv->an_object = NULL;
  //}

  /* Chain up to the parent class */
  G_OBJECT_CLASS(model_simulator_parent_class)->dispose(gobject);
}



extern "C" void
model_simulator_finalize(GObject *gobject)
{
  ModelSimulator *self = MODEL_SIMULATOR(gobject);

  // free g_values and such.
  g_free(self->priv->model_name);
  g_free(self->priv->file_name);
  g_free(self->priv->output_file_name);

  /* Chain up to the parent class */
  G_OBJECT_CLASS(model_simulator_parent_class)->finalize(gobject);
}



extern "C" int 
model_simulator_load(ModelSimulator *simulator, gchar *model_path)
{
  g_print("**load**\n");


  ModelIOxml *gsim = MODEL_IOXML(g_object_new(MODEL_TYPE_IOXML, 
                                                      NULL));
  gchar *prop;
    
  model_ioxml_load(gsim, (gchar *)model_path);
  
  g_object_get(G_OBJECT(gsim), "file_name", &prop, NULL);
  g_print("file_name is now: %s\n", prop);
  g_free (prop);
  
  g_object_unref(gsim);

  /*
  SimBuilder *_sim_builder = simulator->priv->sim_builder;
  std::map<std::string, OpenSim::Variable *> _variables;

  if (_sim_builder)
  {
    delete _sim_builder;
    _sim_builder = NULL;
  }

  IOInterface *file;
  string fileName = model_path; 
  
  // check if its a Vensim model.  if it isn't, assume XML.
  string extension = "";
  if (fileName.length() > 3)
    extension = fileName.substr(fileName.length()-3, 3);
  if (extension == "mdl")
  {
    file = new IOVenText(fileName);
  }
  else
  {
    file = new IOxml(fileName);
  }
  
  if (file->ValidModelParsed())
  {
    _variables = file->Variables();
    _sim_builder = new SimBuilder(_variables);
    
    g_object_set(G_OBJECT(simulator), "model_name", 
                 file->Name().c_str(), NULL);
  }
  
  delete file;
  simulator->priv->sim_builder = _sim_builder;
  */
}

