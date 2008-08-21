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

#include "stdio.h"
#include "string.h"

#include "model-variable.h"

#define PARAM_READWRITE (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT)
#define MODEL_VARIABLE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), MODEL_TYPE_VARIABLE, ModelVariablePrivate))

static gpointer model_variable_parent_class = NULL;
static void model_variable_init(ModelVariable *self);
static void model_variable_class_init(ModelVariableClass *klass);
static void model_variable_dispose(GObject *gobject);
static void model_variable_finalize(GObject *gobject);

static GArray *model_variable_default_get_tokens(ModelVariable *variable);

/* for object properties */
enum
{
  PROP_0,
  PROP_NAME,
  PROP_EQUATION,
  PROP_UNITS,
  PROP_COMMENTS,
  PROP_TYPE,
  PROP_VALID
};


struct _ModelVariablePrivate
{
  gchar         *name;
  gchar         *equation;
  gchar         *units;
  gchar         *comments;
  enum var_type  type;
  
  gboolean       valid;
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
  case PROP_NAME:
    g_return_if_fail(G_VALUE_HOLDS_STRING(value));
    g_free(self->priv->name);
    self->priv->name = g_value_dup_string(value);
    break;

  case PROP_EQUATION:
    g_return_if_fail(G_VALUE_HOLDS_STRING(value));
    g_free(self->priv->equation);
    self->priv->equation = g_value_dup_string(value);
    break;

  case PROP_UNITS:
    g_return_if_fail(G_VALUE_HOLDS_STRING(value));
    g_free(self->priv->units);
    self->priv->units = g_value_dup_string(value);
    break;

  case PROP_COMMENTS:
    g_return_if_fail(G_VALUE_HOLDS_STRING(value));
    g_free(self->priv->comments);
    self->priv->comments = g_value_dup_string(value);
    break;

  case PROP_TYPE:
    g_return_if_fail(G_VALUE_HOLDS_INT(value));
    self->priv->type = g_value_get_int(value);
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
  case PROP_NAME:
    g_value_set_string(value, self->priv->name);
    break;

  case PROP_EQUATION:
    g_value_set_string(value, self->priv->equation);
    break;

  case PROP_UNITS:
    g_value_set_string(value, self->priv->units);
    break;

  case PROP_COMMENTS:
    g_value_set_string(value, self->priv->comments);
    break;

  case PROP_TYPE:
    g_value_set_int(value, (int)self->priv->type);
    break;

  case PROP_VALID:
    g_value_set_boolean(value, self->priv->valid);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}



static void
model_variable_class_init(ModelVariableClass *klass)
{
  model_variable_parent_class = g_type_class_peek_parent(klass);

  g_type_class_add_private(klass, sizeof (ModelVariablePrivate));

  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *model_param_spec;
  
  gobject_class->set_property = model_variable_set_property;
  gobject_class->get_property = model_variable_get_property;
  gobject_class->dispose      = model_variable_dispose;
  gobject_class->finalize     = model_variable_finalize;

  klass->get_tokens           = model_variable_default_get_tokens;

  model_param_spec = g_param_spec_string("name",
                                         "variable name",
                                         "Set variable's name",
                                         NULL /* default value */,
                                         PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_NAME,
                                  model_param_spec);

  model_param_spec = g_param_spec_string("equation",
                                         "variable's equation",
                                         "Get/set variable's equation",
                                         NULL /* default value */,
                                         PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_EQUATION,
                                  model_param_spec);

  model_param_spec = g_param_spec_string("units",
                                         "variable's units",
                                         "Get/set variable's unit",
                                         "" /* default value */,
                                         PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_UNITS,
                                  model_param_spec);

  model_param_spec = g_param_spec_string("comments",
                                         "notes on variable",
                                         "Get/set information about variable",
                                         NULL /* default value */,
                                         PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_COMMENTS,
                                  model_param_spec);

  model_param_spec = g_param_spec_int("type",
                                      "type of variable",
                                      "What kind of variable we have",
                                      -1, 
                                      4, /* max enum number */
                                      var_undef /* default value */,
                                      PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_TYPE,
                                  model_param_spec);

  model_param_spec = g_param_spec_boolean("valid",
                                          "is equation valid",
                                          "True if the equation is good",
                                          TRUE /* default value */,
                                          (GParamFlags) (G_PARAM_READABLE));
  g_object_class_install_property(gobject_class,
                                  PROP_VALID,
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

  //g_fprintf(stderr, "Info: Variable '%s' finalized!\n", self->priv->name);

  // free g_values and such.
  g_free(self->priv->name);
  g_free(self->priv->equation);
  g_free(self->priv->units);
  g_free(self->priv->comments);

  /* Chain up to the parent class */
  G_OBJECT_CLASS(model_variable_parent_class)->finalize(gobject);
}



GArray *
model_variable_get_tokens(ModelVariable *variable)
{
  MODEL_VARIABLE_GET_CLASS(variable)->get_tokens(variable);
}



static GArray *
model_variable_default_get_tokens(ModelVariable *variable)
{
  g_fprintf(stdout, "get tokens\n");
}

