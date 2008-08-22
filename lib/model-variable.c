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

static gpointer      model_variable_parent_class = NULL;
static void          model_variable_init(ModelVariable *self);
static void          model_variable_class_init(ModelVariableClass *klass);
static void          model_variable_dispose(GObject *gobject);
static void          model_variable_finalize(GObject *gobject);
static const GArray *model_variable_default_get_tokens(ModelVariable 
                                                         *variable);
static int           model_variable_tokenize(ModelVariable *variable);

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
  
  GArray        *toks;
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
  
  // lazily initialize toks when needed
  self->priv->toks  = NULL;
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
  if (self->priv->toks)
  {
    GArray *array = self->priv->toks;
    
    int i;
    for (i=0; i<array->len; i++)
    {
      //g_fprintf(stderr, "freeing some var\n");
      //ModelVariable *var = NULL;
      //var = g_array_index(array, ModelVariable *, i);
      //if (var)
      //{
      //  g_object_unref(var);
      //  array->data[i*sizeof(ModelVariable *)] = 0;
      //}
    }
  }

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
  
  if (self->priv->toks) g_array_free(self->priv->toks, TRUE);

  /* Chain up to the parent class */
  G_OBJECT_CLASS(model_variable_parent_class)->finalize(gobject);
}



const GArray *
model_variable_get_tokens(ModelVariable *variable)
{
  MODEL_VARIABLE_GET_CLASS(variable)->get_tokens(variable);
}



static const GArray *
model_variable_default_get_tokens(ModelVariable *variable)
{
  if (!variable->priv->toks) model_variable_tokenize(variable);
  
  return variable->priv->toks;
  g_fprintf(stdout, "get tokens\n");
}



static int
model_variable_tokenize(ModelVariable *variable)
{
  GArray *tokens = g_array_new(FALSE, TRUE, sizeof(equ_token));

  if (variable->priv->type == var_undef) variable->priv->type = var_aux;
  
  const gchar *equation = variable->priv->equation;  
  const int length      = g_utf8_strlen(equation, -1);
  
  // if the equation is empty return an error
  if (length == 0) return -1;
  
  // keep track of where we are in the equation
  // and prime our character buffer
  int char_pos = 0;
  gchar last_char = equation[char_pos++];
  
  int pos1, pos2 = 0;
  
  // now we process the equation string one character at a time,
  // testing to see if it fits into any of the patterns we recognize.
  while (char_pos <= length)
  {
    equ_token new_tok;
    new_tok.op = 0;
    new_tok.num_val = 0.0;
    
    // skip whitespace
    while (isspace(last_char)) last_char = equation[char_pos++];
    
    // only identifiers start with characters
    if (isalpha(last_char))
    {
      new_tok.type = tok_identifier;
      
      // build the string /[a-zA-Z][a-zA-Z0-9_]* /
      // (I'm rusty with regexs, but I think thats right)
      pos1 = char_pos-1;
      while (isalnum((last_char = equation[char_pos++])) || (last_char == '_'))
        continue;
      pos2 = char_pos-1;
      
      new_tok.identifier = g_strndup(&equation[pos1], pos2-pos1);
      
      if ((tokens->len == 0) && (!g_strcmp0(new_tok.identifier,  "INTEG")))
        variable->priv->type = var_stock;
      
      g_array_prepend_val(tokens, new_tok);
    }
    else if (isdigit(last_char) || last_char == '.')
    {
      // we've got a number, but I'm pretty sure we 
      // don't catch negative numbers.
      
      new_tok.type = tok_number;
      
      // build the string like we did for identifiers.
      pos1 = char_pos-1;
      while (isdigit((last_char = equation[char_pos++])) || last_char == '.') 
        continue;
      pos2 = char_pos-1;
      
      new_tok.identifier = g_strndup(&equation[pos1], pos2-pos1);
      
      // convert it to a floating point value.
      // *** FIXME: error checking, please.
      new_tok.num_val = g_ascii_strtod(new_tok.identifier, NULL);
      
      tokens = g_array_prepend_val(tokens, new_tok);
    }
    else
    {
      new_tok.type = tok_operator;
      new_tok.op = last_char;
      new_tok.identifier = "";
      
      
      if ((tokens->len == 0) && (new_tok.op == '[')) 
        variable->priv->type = var_lookup;

      // prime last_char
      // ** FIXME - this could cause problems if the string ends
      // on an operator?
      last_char = equation[char_pos++];
      
      g_array_prepend_val(tokens, new_tok);
    }
  }
  
  if ((tokens->len == 1) && 
      (g_array_index(tokens, equ_token, 0).type == tok_number)) 
  {
    variable->priv->type = var_const;
  }
  
  variable->priv->toks = tokens;
  
  return 0;
}

