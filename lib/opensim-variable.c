//===--- opensim-variable.c - Base class for interacting with opensims --===//
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

#include "stdio.h"
#include "string.h"
#include "ctype.h"

#include "opensim-simulator.h"
#include "opensim-variable.h"

#define PARAM_READWRITE (GParamFlags) (G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT)
#define OPENSIM_VARIABLE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), OPENSIM_TYPE_VARIABLE, OpensimVariablePrivate))

static gpointer      opensim_variable_parent_class = NULL;
static void          opensim_variable_init      (OpensimVariable *self);
static void          opensim_variable_class_init
                                                (OpensimVariableClass *klass);
static void          opensim_variable_dispose   (GObject *gobject);
static void          opensim_variable_finalize  (GObject *gobject);
static GList        *opensim_variable_default_get_influences
                                                (OpensimVariable *variable);
static const GArray *opensim_variable_default_get_tokens
                                                (OpensimVariable *variable);
static int           opensim_variable_tokenize  (OpensimVariable *variable);
extern gchar        *clean_string               (gchar *str);
static void          opensim_variable_delete_tokens 
                                                (OpensimVariable *variable);

/* for object properties */
enum
{
  PROP_0,
  PROP_NAME,
  PROP_EQUATION,
  PROP_UNITS,
  PROP_COMMENTS,
  PROP_TYPE,
  PROP_SIM,
  PROP_VALID
};


enum
{
  OS_SIG_EQN_CHANGED,

  VAR_LAST_SIGNAL
};


static guint variable_signal[VAR_LAST_SIGNAL] = {0};


struct _OpensimVariablePrivate
{
  gchar    *name;
  gchar    *equation;
  gchar    *units;
  gchar    *comments;
  var_type  type;
  
  gboolean  valid;
  
  gpointer  sim;
  GArray   *toks;
};



GType 
opensim_variable_get_type()
{
  static GType g_define_type_id = 0; 
  if (G_UNLIKELY(g_define_type_id == 0)) 
  { 
    static const GTypeInfo g_define_type_info = { 
      sizeof (OpensimVariableClass), 
      (GBaseInitFunc) NULL, 
      (GBaseFinalizeFunc) NULL, 
      (GClassInitFunc) opensim_variable_class_init, 
      (GClassFinalizeFunc) NULL, 
      NULL,   // class_data 
      sizeof (OpensimVariable), 
      0,      // n_preallocs 
      (GInstanceInitFunc) opensim_variable_init, 
    }; 
    g_define_type_id = g_type_register_static(G_TYPE_OBJECT, 
                                              "OpensimVariableType", 
                                              &g_define_type_info, 
                                              (GTypeFlags) 0); 
  } 
  return g_define_type_id; 

}



void
opensim_variable_set_property(GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  OpensimVariable *self = OPENSIM_VARIABLE(object);

  switch (property_id)
  {
  case PROP_NAME:
    g_return_if_fail(G_VALUE_HOLDS_STRING(value));
    g_free (self->priv->name);
    gchar *unclean_name = (gchar *)g_value_get_string (value);
    self->priv->name = clean_string (unclean_name);
    break;

  case PROP_EQUATION:
    g_return_if_fail(G_VALUE_HOLDS_STRING(value));

    // hold on to equation for now so that we can pass it out
    // along with our signal
    gchar *equation = self->priv->equation;
    self->priv->equation = g_value_dup_string(value);
    opensim_variable_delete_tokens (self);
    g_signal_emit_by_name (object, "equation_changed", equation);
    g_free(equation);
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

  case PROP_SIM:
    g_return_if_fail (G_VALUE_HOLDS_OBJECT (value));
    /* we only want to set this once */
    g_return_if_fail (self->priv->sim == NULL);

    self->priv->sim = g_value_get_object (value);
    break;

  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}



void
opensim_variable_get_property (GObject    *object,
                               guint       property_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  OpensimVariable *self = OPENSIM_VARIABLE(object);

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
    // we determine type when we analyze the tokens
    if (!self->priv->toks) opensim_variable_tokenize(self);
    g_value_set_int(value, (int)self->priv->type);
    break;

  case PROP_VALID:
    g_value_set_boolean(value, self->priv->valid);
    break;
      
  case PROP_SIM:
    g_value_set_pointer (value, self->priv->sim);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    break;
  }
}



static void
opensim_variable_class_init(OpensimVariableClass *klass)
{
  opensim_variable_parent_class = g_type_class_peek_parent(klass);

  g_type_class_add_private(klass, sizeof (OpensimVariablePrivate));

  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *opensim_param_spec;
  
  gobject_class->set_property = opensim_variable_set_property;
  gobject_class->get_property = opensim_variable_get_property;
  gobject_class->dispose      = opensim_variable_dispose;
  gobject_class->finalize     = opensim_variable_finalize;

  klass->get_influences       = opensim_variable_default_get_influences;
  klass->get_tokens           = opensim_variable_default_get_tokens;

  opensim_param_spec = g_param_spec_string("name",
                                           "variable name",
                                           "Set variable's name",
                                           NULL /* default value */,
                                           PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_NAME,
                                  opensim_param_spec);

  opensim_param_spec = g_param_spec_string("equation",
                                           "variable's equation",
                                           "Get/set variable's equation",
                                           NULL /* default value */,
                                           PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_EQUATION,
                                  opensim_param_spec);

  opensim_param_spec = g_param_spec_string("units",
                                           "variable's units",
                                           "Get/set variable's unit",
                                           "" /* default value */,
                                           PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_UNITS,
                                  opensim_param_spec);

  opensim_param_spec = g_param_spec_string("comments",
                                           "notes on variable",
                                           "Get/set information about variable",
                                           NULL /* default value */,
                                           PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_COMMENTS,
                                  opensim_param_spec);

  opensim_param_spec = g_param_spec_int("type",
                                        "type of variable",
                                        "What kind of variable we have",
                                        -1, 
                                        4, /* max enum number */
                                        var_undef /* default value */,
                                        PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_TYPE,
                                  opensim_param_spec);
  
  opensim_param_spec = g_param_spec_object("sim",
                                           "parent simulation",
                                           "Pointer to parent simulation",
                                           OPENSIM_TYPE_SIMULATOR,
                                           PARAM_READWRITE);
  g_object_class_install_property(gobject_class,
                                  PROP_SIM,
                                  opensim_param_spec);

  opensim_param_spec = g_param_spec_boolean("valid",
                                            "is equation valid",
                                            "True if the equation is good",
                                            TRUE /* default value */,
                                            (GParamFlags) (G_PARAM_READABLE));
  g_object_class_install_property(gobject_class,
                                  PROP_VALID,
                                  opensim_param_spec);

  // time for signals!
  variable_signal[OS_SIG_EQN_CHANGED] = 
    g_signal_new ("equation_changed",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                  NULL /* closure */,
                  NULL /* accumulator */,
                  NULL /* accumulator data */,
                  g_cclosure_marshal_VOID__STRING,
                  G_TYPE_NONE /* return_type */,
                  1     /* n_params */,
                  G_TYPE_STRING  /* param_types */);
}



static void
opensim_variable_init(OpensimVariable *self)
{
  self->priv = OPENSIM_VARIABLE_GET_PRIVATE(self);
  
  self->priv->valid = TRUE;
  
  // lazily initialize toks when needed
  self->priv->toks  = NULL;
}



static void
opensim_variable_dispose(GObject *gobject)
{
  //OpensimVariable *self = OPENSIM_VARIABLE(gobject);

  /* 
   * In dispose, you are supposed to free all typesecifier before 'IOVenText'
   * object which might themselves hold a reference to self. Generally,
   * the most simple solution is to unref all members on which you own a 
   * reference.
   */

  /* dispose might be called multiple times, so we must guard against
   * calling g_object_unref() on an invalid GObject.
   */

  /* Chain up to the parent class */
  G_OBJECT_CLASS(opensim_variable_parent_class)->dispose(gobject);
}



static void 
opensim_variable_delete_tokens (OpensimVariable *variable)
{
  if (variable->priv->toks)
  {
    GArray *array = variable->priv->toks;
    
    int i;
    for (i=0; i<array->len; i++)
    {
      equ_token tok = g_array_index(array, equ_token, i);
 
      if (tok.identifier) g_free(tok.identifier);
    }
  }
  
  if (variable->priv->toks) g_array_free(variable->priv->toks, TRUE);
  
  variable->priv->toks = NULL;
}



static void
opensim_variable_finalize(GObject *gobject)
{
  OpensimVariable *self = OPENSIM_VARIABLE(gobject);

  //g_fprintf(stderr, "Info: Variable '%s' finalized!\n", self->priv->name);

  // free g_values and such.
  g_free(self->priv->name);
  g_free(self->priv->equation);
  g_free(self->priv->units);
  g_free(self->priv->comments);

  opensim_variable_delete_tokens (self);  

  /* Chain up to the parent class */
  G_OBJECT_CLASS(opensim_variable_parent_class)->finalize(gobject);
}



GList *
opensim_variable_get_influences (OpensimVariable *variable)
{
  return OPENSIM_VARIABLE_GET_CLASS (variable)->get_influences (variable);
}



static GList *
opensim_variable_default_get_influences (OpensimVariable *variable)
{
  OpensimVariablePrivate *self = variable->priv;
  OpensimSimulator *sim = self->sim;

  // if we don't have a simulator listed, we can't 
  // get references to other variables
  if (!sim) 
  {
    fprintf (stderr, "Warning: Missing sim reference in get_influences\n");
    return;
  }
  
  const GArray *toks = opensim_variable_get_tokens (variable);
  
  GList *influences = NULL;
  
  GArray *array = self->toks;
  OpensimVariable *in_var = NULL;
  int i;
  for (i=0; i<array->len; i++)
  {
    equ_token tok = g_array_index(array, equ_token, i);
    
    if (tok.type == tok_identifier) 
    {
      in_var = opensim_simulator_get_variable (sim, tok.identifier);
      if (in_var)
      {
        g_object_ref (in_var);
        influences = g_list_prepend (influences, in_var);
      }
    }
  } 
  
  return influences;
}



const GArray *
opensim_variable_get_tokens(OpensimVariable *variable)
{
  return OPENSIM_VARIABLE_GET_CLASS (variable)->get_tokens (variable);
}



static const GArray *
opensim_variable_default_get_tokens(OpensimVariable *variable)
{
  if (!variable->priv->toks) opensim_variable_tokenize (variable);
  
  return variable->priv->toks;
}



static int
opensim_variable_tokenize(OpensimVariable *variable)
{
  GArray *tokens = g_array_new(FALSE, FALSE, sizeof(equ_token));

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
    new_tok.identifier = NULL;
    
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
      
      g_array_append_val(tokens, new_tok);
    }
    else if (isdigit(last_char) || last_char == '.')
    {
      // we've got a number, but I'm pretty sure we 
      // don't catch negative numbers.
      
      new_tok.type = tok_number;
      
      // build the string like we did for identifiers.
      //while (isdigit((last_char = equation[char_pos++])) || last_char == '.') 
      //  continue;
      
      // build the string like we did for identifiers.
      pos1 = char_pos-1;
      while (isdigit((last_char = equation[char_pos++])) || last_char == '.') 
        continue;
      
      // convert it to a floating point value.
      // *** FIXME: error checking, please.
      new_tok.num_val = g_ascii_strtod(&equation[pos1], NULL);
      
      tokens = g_array_append_val(tokens, new_tok);
    }
    else
    {
      new_tok.type = tok_operator;
      new_tok.op = last_char;
      new_tok.identifier = NULL;
      
      
      if ((tokens->len == 0) && (new_tok.op == '[')) 
        variable->priv->type = var_lookup;

      // prime last_char
      // ** FIXME - this could cause problems if the string ends
      // on an operator?
      last_char = equation[char_pos++];
      
      g_array_append_val(tokens, new_tok);
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

