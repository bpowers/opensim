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

#include <glib.h>
#include <string.h>

#include "globals.h"

#include "opensim-simulator.h"
#include "opensim-variable.h"
#include "CodeGen/opensim-generator.h"
#include "IO/opensim-ioxml.h"


#define PARAM_READWRITE (GParamFlags) \
        (G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT)
#define OPENSIM_SIMULATOR_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), OPENSIM_TYPE_SIMULATOR, \
                                            OpensimSimulatorPrivate))

static gpointer opensim_simulator_parent_class = NULL;
static void opensim_simulator_init        (OpensimSimulator *self);
static void opensim_simulator_class_init  (OpensimSimulatorClass *klass);
static void opensim_simulator_dispose     (GObject *gobject);
static void opensim_simulator_finalize    (GObject *gobject);

static int opensim_simulator_default_run  (OpensimSimulator *simulator);
static int opensim_simulator_default_load (OpensimSimulator *simulator, 
                                           gchar *model_path);
static int opensim_simulator_default_save (OpensimSimulator *simulator);

static OpensimVariable *opensim_simulator_default_new_variable
                                          (OpensimSimulator *simulator, 
                                           gchar *var_name, 
                                           gchar *var_eqn);
static OpensimVariable *opensim_simulator_default_get_variable
                                          (OpensimSimulator *simulator, 
                                           gchar *var_name);
static GList *opensim_simulator_default_get_variables
                                          (OpensimSimulator *simulator);
static int opensim_simulator_default_remove_variable
                                          (OpensimSimulator *simulator, 
                                           gchar *var_name);

static int opensim_simulator_default_output_debug_info
                                          (OpensimSimulator *simulator);
static void set_sim_for_variable          (OpensimVariable *var, 
                                           OpensimSimulator *sim);
static void opensim_simulator_var_equation_changed 
                                          (OpensimVariable *variable, 
                                           gchar *old_equation,
                                           gpointer simulator);


GType
opensim_output_get_type ()
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
  PROP_VALID_MODEL,
  
  LAST_PROP
};

enum
{
  OS_SIG_SAVING,

  SIM_LAST_SIGNAL
};


static guint simulator_signal[SIM_LAST_SIGNAL] = {0};


struct _OpensimSimulatorPrivate
{
  gchar            *model_name;
  gchar            *file_name;
  int               output_type;
  gchar            *output_file_name;
  gboolean          valid_model;
  
  GList            *var_list;
  
  OpensimGenerator *generator;
  GHashTable       *var_hash;
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
opensim_simulator_class_init (OpensimSimulatorClass *klass)
{
  opensim_simulator_parent_class = g_type_class_peek_parent (klass);

  g_type_class_add_private (klass, sizeof (OpensimSimulatorPrivate));

  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GParamSpec *opensim_param_spec;
  
  gobject_class->set_property = opensim_simulator_set_property;
  gobject_class->get_property = opensim_simulator_get_property;
  gobject_class->dispose      = opensim_simulator_dispose;
  gobject_class->finalize     = opensim_simulator_finalize;

  klass->load                 = opensim_simulator_default_load;
  klass->save                 = opensim_simulator_default_save;
  klass->new_variable         = opensim_simulator_default_new_variable;
  klass->get_variable         = opensim_simulator_default_get_variable;
  klass->get_variables        = opensim_simulator_default_get_variables;
  klass->remove_variable      = opensim_simulator_default_remove_variable;
  klass->output_debug_info    = opensim_simulator_default_output_debug_info;
  klass->run                  = opensim_simulator_default_run;

  opensim_param_spec = g_param_spec_string ("model_name",
                                            "model name",
                                            "Set model's name",
                                            "unnamed model" /* default value */,
                                            PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_MODEL_NAME,
                                   opensim_param_spec);

  opensim_param_spec = g_param_spec_string ("file_name",
                                            "full path to file",
                                            "Where the opensim is saved to",
                                            NULL /* default value */,
                                            PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_FILE_NAME,
                                   opensim_param_spec);

  opensim_param_spec = g_param_spec_int ("output_type",
                                         "type of output",
                                         "What kind of output to generate",
                                         0, 
                                         5,
                                         sim_emit_Output /* default value */,
                                         PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_OUTPUT_TYPE,
                                   opensim_param_spec);

  opensim_param_spec = g_param_spec_string ("output_file_name",
                                            "full path to output file",
                                            "Where to save output",
                                            NULL /* default value */,
                                            PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_OUTPUT_FILE_NAME,
                                   opensim_param_spec);

  opensim_param_spec = g_param_spec_boolean ("valid_model",
                                             "is model valid",
                                             "True if simulatable",
                                             TRUE /* default value */,
                                             (GParamFlags) (G_PARAM_READABLE));
  g_object_class_install_property (gobject_class,
                                   PROP_VALID_MODEL,
                                   opensim_param_spec);
  
  // time for signals!
  simulator_signal[OS_SIG_SAVING] = 
    g_signal_new ("saving",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                  0 /* closure */,
                  NULL /* accumulator */,
                  NULL /* accumulator data */,
                  g_cclosure_marshal_VOID__POINTER,
                  G_TYPE_NONE /* return_type */,
                  1     /* n_params */,
                  G_TYPE_POINTER  /* param_types */);
}



static int
opensim_simulator_init_blank_model (OpensimSimulator *simulator)
{
  OpensimSimulatorPrivate *self = simulator->priv;
  
  if (!self->var_hash)
  {
    //fprintf (stdout, "Error: you can't initialize a blank model w/o hash\n");
  }
  
  // time variables we need
  const gchar *names[] = {"time_start", "time_end", 
                          "time_step", "time_savestep"};
  const gchar *eqns[] = {"0", "100", "1", "1"};

  for (guint i=0; i<4; ++i)
  {
    OpensimVariable *new_var = 
      OPENSIM_VARIABLE (g_object_new (OPENSIM_TYPE_VARIABLE, NULL));

    g_object_set (G_OBJECT (new_var), "name", names[i], 
                                      "equation", eqns[i], NULL);

    set_sim_for_variable (new_var, simulator);

    self->var_list = g_list_prepend (self->var_list, new_var);
    gchar *name = g_strdup (names[i]);
    g_hash_table_insert (self->var_hash, name, new_var);
    g_free (name);
  }
  
  return 0;
}



static void
opensim_simulator_init (OpensimSimulator *simulator)
{
  simulator->priv = OPENSIM_SIMULATOR_GET_PRIVATE (simulator);
  
  OpensimSimulatorPrivate *self = simulator->priv;
  
  self->valid_model = TRUE;
  self->var_list    = NULL;
  self->var_hash    = g_hash_table_new (g_str_hash, g_str_equal);
  opensim_simulator_init_blank_model (simulator);
  // FIXME: initialize generator here
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
  GList *itr = OPENSIM_SIMULATOR_GET_PRIVATE (self)->var_list;
  while (itr)
  {
    //g_fprintf(stderr, "freeing some var\n");
    OpensimVariable *var = NULL;
    var = OPENSIM_VARIABLE (itr->data);
    if (var)
    {
      g_object_unref (var);
      itr->data = NULL;
    }

    itr = g_list_next (itr);
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
  
  if (self->priv->var_list)
    g_list_free (self->priv->var_list);

  /* Chain up to the parent class */
  G_OBJECT_CLASS(opensim_simulator_parent_class)->finalize(gobject);
}



static void 
set_sim_for_variable (OpensimVariable *var, OpensimSimulator *sim)
{
  g_object_set (G_OBJECT (var), "sim", sim, NULL);
  g_signal_connect_object (var, "equation_changed", 
                           G_CALLBACK (opensim_simulator_var_equation_changed),
                           sim, G_CONNECT_AFTER);
}



int 
opensim_simulator_load (OpensimSimulator *simulator,
                        gchar *model_path)
{
  return OPENSIM_SIMULATOR_GET_CLASS (simulator)->load (simulator, 
                                                        model_path);
}



static int 
opensim_simulator_default_load (OpensimSimulator *simulator, 
                                gchar *model_path)
{
  OpensimSimulatorPrivate *self = simulator->priv;
  OpensimIOxml *gio = OPENSIM_IOXML(g_object_new(OPENSIM_TYPE_IOXML, 
                                             NULL));
  gboolean valid_model = FALSE;
  gchar *prop;
  
  //FIXME: need to delete and reinitialize var_hash

  int load_status = opensim_ioxml_load(gio, model_path);
  
  if (load_status != 0)
  {
    fprintf(stderr, "Error: couldn't load model.\n");
    self->valid_model = FALSE;
    return -1;
  }
  
  g_object_get(G_OBJECT(gio), "model_name", &prop,
                              "valid",      &valid_model, NULL);
  g_object_set(G_OBJECT(simulator), "model_name", prop, NULL);
  g_free(prop);

  GList *vars = opensim_ioxml_get_variables(gio);
  
  if (!vars) 
    fprintf(stderr, "Warning: variable array not available from IOxml.\n");
  
  simulator->priv->var_list = vars;
  
  g_object_unref(gio);

  // turn our nice list into an ugly map AND set simulator
  GList *itr = vars;
  while (itr)
  {
    OpensimVariable *var = OPENSIM_VARIABLE (itr->data);
    gchar *var_name = NULL;

    set_sim_for_variable (var, simulator);
    g_object_get (G_OBJECT (var), "name", &var_name, NULL);
    
    g_hash_table_insert (self->var_hash, var_name, var);

    g_free(var_name);

    itr = g_list_next (itr);
  }

  if (self->generator)
  {
    g_object_unref (self->generator);
    self->generator = NULL;
  }
  
  if (valid_model)
  {
    //self->sim_builder = new SimBuilder(_variables);
  }

  return 0;
}



int 
opensim_simulator_output_debug_info(OpensimSimulator *simulator)
{
  return OPENSIM_SIMULATOR_GET_CLASS(simulator)->output_debug_info(simulator);
}



int 
opensim_simulator_default_output_debug_info(OpensimSimulator *simulator)
{
  OpensimSimulatorPrivate *self = OPENSIM_SIMULATOR_GET_PRIVATE (simulator);

  fprintf (stdout, "Info: outputting debugging info\n");

  if (self->var_list)
  {
    guint len = g_list_length (self->var_list);
    fprintf(stdout, "  found variable list of size %d\n", len);
    
    GList *itr = self->var_list;
    while (itr)
    {
      OpensimVariable *var = OPENSIM_VARIABLE (itr->data);
      gchar *var_name = NULL;
      gchar *equation = NULL;
      
      g_object_get(G_OBJECT(var), "name",     &var_name, 
                                  "equation", &equation, NULL);
      fprintf(stdout, "    var '%s'\n    '%s'\n", var_name, equation);
      
      const GArray *toks = opensim_variable_get_tokens(var);
      
      guint j;
      for (j=0; j<toks->len; j++)
      {
        //g_fprintf(stderr, "freeing some var\n");
        equ_token tok = g_array_index(toks, equ_token, j);
        
        fprintf(stdout, "      tok ('%c' '%d') '%s' (%f)\n", 
                tok.op, tok.type, 
                tok.identifier, tok.num_val);
      }
      
      g_free(var_name);
      g_free(equation);

      itr = g_list_next (itr);
    }
  }
  else
  {
    fprintf(stdout, "  no array of variables.\n");
  }
  
  return 0;
}



int
opensim_simulator_run(OpensimSimulator *simulator)
{
  return OPENSIM_SIMULATOR_GET_CLASS(simulator)->run(simulator);
}



int 
opensim_simulator_default_run(OpensimSimulator *simulator)
{
  OpensimSimulatorPrivate *self = simulator->priv;
  int ret = 0;
  
  if (!self->valid_model)
  {
    fprintf(stderr, "Warning: Model wasn't valid so it wasn't simulated\n");
    return -1;
  }
  
  if (!self->generator)
  {
    fprintf(stderr, "Error: Simulator doesn't have generator.\n");
    return -1;
  }
  
  FILE *output_stream = stdout;
  gchar *output_file_name = self->output_file_name;
  
  if (output_file_name) 
  {
    output_stream = fopen(output_file_name, "w+");
    
    if (!output_stream) 
    {
      fprintf(stderr, "Error: Could not open output file for writing.\n");
      return -1;
    }
  }
  
  ret = opensim_generator_parse (self->generator, 
                                 self->output_type, 
                                 output_stream);
  
  
  // if we opened it, close the output stream
  if (output_stream != stdout) fclose (output_stream);
  
  return ret;
}



int 
opensim_simulator_save (OpensimSimulator *simulator)
{
  return OPENSIM_SIMULATOR_GET_CLASS (simulator)->save (simulator);
}



static int 
opensim_simulator_default_save (OpensimSimulator *simulator)
{
  OpensimSimulatorPrivate *self = simulator->priv;
  
  FILE *save_file = NULL;
  
  if (self->file_name)
    save_file = fopen (self->file_name, "w");
  else 
    save_file = stdout;
  
  if (!save_file)
  {
    fprintf (stderr, "Error: couldn't open file for writing: '%s'\n", 
             self->file_name);
    return -1;
  }
  
  OpensimIOxml *gio = OPENSIM_IOXML (g_object_new (OPENSIM_TYPE_IOXML, 
                                                   NULL));
  
  int h_ret = opensim_ioxml_write_header (gio, simulator, save_file);
  int b_ret = opensim_ioxml_write_body (gio, simulator, save_file);
  
  g_signal_emit_by_name (simulator, "saving", save_file);
  
  int f_ret = opensim_ioxml_write_footer (gio, simulator, save_file);
  
  // these should only return <= 0, so this is okay i think
  if (h_ret + b_ret + f_ret != 0)
    fprintf(stderr, "Error: there were some problems saving the model.\n");
  
  g_object_unref(gio);
  
  // clean up
  if (save_file != stdout) fclose(save_file);
  
  
  return h_ret + b_ret + f_ret;
}



OpensimVariable *
opensim_simulator_new_variable(OpensimSimulator *simulator, 
                               gchar *var_name, 
                               gchar *var_eqn)
{
  return OPENSIM_SIMULATOR_GET_CLASS(simulator)->new_variable(simulator,
                                                              var_name,
                                                              var_eqn);
}



// remove leading and trailign whitespace.
// will need to be changed to support unicode
gchar *
clean_string(gchar *str)
{
  gchar *copy = g_strdup(str);

  if (!copy) return NULL;

  size_t i;
  for (i=0; i<strlen(copy); ++i)
    if (copy[i] == ' ') copy[i] = '_';
  
  return copy;
}



static OpensimVariable *
opensim_simulator_default_new_variable (OpensimSimulator *simulator, 
                                        gchar *var_name, 
                                        gchar *var_eqn)
{
  OpensimSimulatorPrivate *self = OPENSIM_SIMULATOR_GET_PRIVATE (simulator);
  gchar *clean_name = clean_string(var_name);
  
  if (var_name == NULL || !g_strcmp0 (var_name, "")) 
  {
    fprintf(stderr, "Error: variable must have a name\n");
    return NULL;
  }
  
  OpensimVariable *new_var = 
    OPENSIM_VARIABLE (g_object_new (OPENSIM_TYPE_VARIABLE, NULL));
  
  g_object_set (G_OBJECT (new_var), "name", clean_name, 
                                    "equation", var_eqn, NULL);
  
  set_sim_for_variable (new_var, simulator);
  
  self->var_list = g_list_prepend (self->var_list, new_var);

  g_hash_table_insert (self->var_hash, clean_name, new_var);
  
  g_free(clean_name);
  
  opensim_generator_rebase (self->generator, self->var_hash);
  
  return new_var;
}



OpensimVariable *
opensim_simulator_get_variable (OpensimSimulator *simulator, 
                                gchar *var_name)
{
  return OPENSIM_SIMULATOR_GET_CLASS(simulator)->get_variable (simulator,
                                                               var_name);
}



static OpensimVariable *
opensim_simulator_default_get_variable (OpensimSimulator *simulator, 
                                        gchar *var_name)
{
  OpensimSimulatorPrivate *self = OPENSIM_SIMULATOR_GET_PRIVATE (simulator);
  if (var_name == NULL || !g_strcmp0 (var_name, "")) 
  {
    fprintf (stderr, "Error: variable must have a name\n");
    return NULL;
  }

  // replace spaces with underscores so that we can more easily match names
  gchar *var_name_clean = clean_string (var_name);
  
  gpointer var = g_hash_table_lookup (self->var_hash, var_name_clean);

  g_free (var_name_clean);
  if (!var)
  {
    return NULL;
  }

  OpensimVariable *ret = OPENSIM_VARIABLE (var);
  g_object_ref (ret);
  
  return ret;
}



GList *
opensim_simulator_get_variables (OpensimSimulator *simulator)
{
  return OPENSIM_SIMULATOR_GET_CLASS (simulator)->get_variables (simulator);
}



static GList *
opensim_simulator_default_get_variables (OpensimSimulator *simulator)
{
  OpensimSimulatorPrivate *self = OPENSIM_SIMULATOR_GET_PRIVATE (simulator);
  
  GList *ret = g_list_copy (self->var_list);

  // Increase the reference count of all of the variables
  GList *itr = ret;
  while (itr)
  {
    if (itr->data) g_object_ref (itr->data);
    
    itr = g_list_next (itr);
  }
  
  return ret;
}



int 
opensim_simulator_remove_variable (OpensimSimulator *simulator, 
                                   gchar *var_name)
{
  return OPENSIM_SIMULATOR_GET_CLASS (simulator)->remove_variable (simulator, 
                                                                   var_name);
}


                                       
static int 
opensim_simulator_default_remove_variable (OpensimSimulator *simulator, 
                                           gchar *var_name)
{
  return -1;
}



static void 
opensim_simulator_var_equation_changed (OpensimVariable *variable, 
                                        gchar *old_equation,
                                        gpointer sim)
{
  OpensimSimulator *simulator = OPENSIM_SIMULATOR (sim);
  OpensimSimulatorPrivate *self = simulator->priv;
  
  // in the future, we will probably want to do more here
  opensim_generator_rebase (self->generator, self->var_hash);
}

