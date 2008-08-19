//===--- Variable.cpp - Base class for interacting with models ----------===//
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

// libxml parsing
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "string.h"

#include "model-variable.h"

#define PARAM_READWRITE (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT)
#define MODEL_VARIABLE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), MODEL_TYPE_VARIABLE, ModelVariablePrivate))

static gpointer model_variable_parent_class = NULL;
static void model_variable_init(ModelVariable *self);
static void model_variable_class_init(ModelVariableClass *kclass);
static void model_variable_dispose(GObject *gobject);
static void model_variable_finalize(GObject *gobject);
static int parse_input(xmlDocPtr doc, xmlNodePtr mod);

/* for object properties */
enum
{
  PROP_0,
  PROP_MODEL_NAME,
  PROP_FILE_NAME
};


struct _ModelVariablePrivate
{
  gchar    *model_name;
  gchar    *file_name;
  
  gboolean  valid;
};



GType 
model_variable_get_type()
{
  static GType g_define_type_id = 0; 
  if (G_UNLIKELY(g_define_type_id == 0)) 
  { 
    static const GTypeInfo g_define_type_info = { 
      sizeof (ModelVariableClass), 
      (GBaseInitFunc) NULL, 
      (GBaseFinalizeFunc) NULL, 
      (GClassInitFunc) model_variable_class_init, 
      (GClassFinalizeFunc) NULL, 
      NULL,   // class_data 
      sizeof (ModelVariable), 
      0,      // n_preallocs 
      (GInstanceInitFunc) model_variable_init, 
    }; 
    g_define_type_id = g_type_register_static(G_TYPE_OBJECT, 
                                              "ModelVariableType", 
                                              &g_define_type_info, 
                                              (GTypeFlags) 0); 
  } 
  return g_define_type_id; 

}



void
model_variable_set_property(GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  ModelVariable *self = MODEL_VARIABLE(object);

  switch (property_id)
  {
  case PROP_MODEL_NAME:
    g_return_if_fail(G_VALUE_HOLDS_STRING(value));
    g_free(self->priv->model_name);
    self->priv->model_name = g_value_dup_string(value);
    break;
  case PROP_FILE_NAME:
    g_return_if_fail(G_VALUE_HOLDS_STRING(value));
    g_free(self->priv->file_name);
    self->priv->file_name = g_value_dup_string(value);
    break;
  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}



void
model_variable_get_property (GObject    *object,
                              guint       property_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  ModelVariable *self = MODEL_VARIABLE(object);

  switch (property_id)
  {
  case PROP_MODEL_NAME:
    g_value_set_string(value, self->priv->model_name);
    break;
  case PROP_FILE_NAME:
    g_value_set_string(value, self->priv->file_name);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}



static void
model_variable_class_init(ModelVariableClass *kclass)
{
  model_variable_parent_class = g_type_class_peek_parent(kclass);

  g_type_class_add_private(kclass, sizeof (ModelVariablePrivate));

  GObjectClass *gobject_class = G_OBJECT_CLASS(kclass);
  GParamSpec *model_param_spec;
  
  gobject_class->set_property = model_variable_set_property;
  gobject_class->get_property = model_variable_get_property;
  gobject_class->dispose      = model_variable_dispose;
  gobject_class->finalize     = model_variable_finalize;

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

}



static void
model_variable_init(ModelVariable *self)
{
  self->priv = MODEL_VARIABLE_GET_PRIVATE(self);
  
  self->priv->valid = TRUE;
}



static void
model_variable_dispose(GObject *gobject)
{
  ModelVariable *self = MODEL_VARIABLE(gobject);

  /* 
   * In dispose, you are supposed to free all typesecifier before 'IOVenText'
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
  G_OBJECT_CLASS(model_variable_parent_class)->dispose(gobject);
}



static void
model_variable_finalize(GObject *gobject)
{
  ModelVariable *self = MODEL_VARIABLE(gobject);

  // free g_values and such.
  g_free(self->priv->file_name);

  /* Chain up to the parent class */
  G_OBJECT_CLASS(model_variable_parent_class)->finalize(gobject);
}




