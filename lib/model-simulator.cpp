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

#include "model-simulator.h"


#define PARAM_READWRITE (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE)
#define MODEL_SIMULATOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), MODEL_TYPE_SIMULATOR, ModelSimulatorPrivate))

extern "C" void model_simulator_init(ModelSimulator *self);
extern "C" void model_simulator_class_init(ModelSimulatorClass *kclass);

enum
{
  PROP_0,

  PROP_MODEL_NAME,
  PROP_SKETCH_NAME,
  PROP_FILE_NAME,
  PROP_OUTPUT_TYPE,
  PROP_OUTPUT_FILE_NAME,
  PROP_VALID_MODEL
};


struct _ModelSimulatorPrivate
{
  gchar      *model_name;
  gchar      *sketch_name;
  gchar      *file_name;
  sim_output  output_type;
  gchar      *output_file_name;
  gboolean    valid_model;
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
    g_free(self->priv->model_name);
    self->priv->model_name = g_value_dup_string(value);
    g_print("model_name: %s\n", self->priv->model_name);
    break;

  case PROP_SKETCH_NAME:
    g_free(self->priv->sketch_name);
    self->priv->sketch_name = g_value_dup_string(value);
    g_print("sketch_name: %s\n", self->priv->sketch_name);
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

  case PROP_SKETCH_NAME:
    g_value_set_string(value, self->priv->sketch_name);
    break;

  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}



extern "C" void
model_simulator_class_init(ModelSimulatorClass *kclass)
{
  g_print("class init\n");
  
  g_type_class_add_private(kclass, sizeof (ModelSimulatorPrivate));

  GObjectClass *gobject_class = G_OBJECT_CLASS(kclass);
  GParamSpec *model_param_spec;
  
  gobject_class->set_property = model_simulator_set_property;
  gobject_class->get_property = model_simulator_get_property;

  model_param_spec = g_param_spec_string("model_name",
                                         "model name",
                                         "Set model's name",
                                         "unnamed model" /* default value */,
                                         PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_MODEL_NAME,
                                  model_param_spec);

  model_param_spec = g_param_spec_string("sketch_name",
                                         "current sketch name",
                                         "Set the name of the current sketch",
                                         "unnamed sketch" /* default value */,
                                         PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_SKETCH_NAME,
                                  model_param_spec);
}



extern "C" void
model_simulator_init(ModelSimulator *self)
{
  self->priv = MODEL_SIMULATOR_GET_PRIVATE(self);
  
  g_print("sim init\n");
  /*
  self->priv->model_name       = NULL;
  self->priv->sketch_name      = NULL;
  self->priv->file_name        = NULL;
  self->priv->output_type      = sim_emit_Output;
  self->priv->output_file_name = NULL;
  self->priv->valid_model      = FALSE;
  */
}

