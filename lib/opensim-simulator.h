/*===--- opensim-simulator.h - Base class for interacting with opensim ---===
 *
 * Copyright 2008 Bobby Powers
 *
 * This file is part of OpenSim.
 * 
 * OpenSim is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * OpenSim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with OpenSim.  If not, see <http: *www.gnu.org/licenses/>.
 *
 *===---------------------------------------------------------------------===
 *
 * This class represents opensims at a high level, suitible for inclusion
 * in projects as part of a library.
 * TODO: implement features for dynamically changing opensims.
 *
 *===---------------------------------------------------------------------===
 */

#ifndef __OPENSIM_SIMULATOR_H__
#define __OPENSIM_SIMULATOR_H__

#include <glib.h>
#include <glib-object.h>
#include <glib/gprintf.h>

#include "opensim-variable.h"

G_BEGIN_DECLS

#ifndef __cplusplus
typedef enum
{
  sim_emit_IR      = 1, /* not supported                       */
  sim_emit_Python  = 2, /* full Python implementation of opensim */
  sim_emit_Fortran = 3, /* not implemented yet                 */
  sim_emit_Output  = 4, /* results of interpreting opensim       */
  sim_emit_AS3     = 5, /* full AS3 implementation of opensim    */
} OpensimOutput;
#endif
#ifdef __cplusplus
enum sim_output
{
  sim_emit_IR      = 1, /* not supported                       */
  sim_emit_Python  = 2, /* full Python implementation of opensim */
  sim_emit_Fortran = 3, /* not implemented yet                 */
  sim_emit_Output  = 4, /* results of interpreting opensim       */
  sim_emit_AS3     = 5, /* full AS3 implementation of opensim    */
};
#endif

GType opensim_output_get_type (void) G_GNUC_CONST;
#define OPENSIM_TYPE_OUTPUT (opensim_output_get_type ())

/*
 * Type macros.
 */
#define OPENSIM_TYPE_SIMULATOR            (opensim_simulator_get_type())
#define OPENSIM_SIMULATOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), \
                                           OPENSIM_TYPE_SIMULATOR, \
                                           OpensimSimulator))
#define OPENSIM_SIMULATOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), \
                                           OPENSIM_TYPE_SIMULATOR, \
                                           OpensimSimulatorClass))
#define OPENSIM_IS_SIMULATOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), \
                                           OPENSIM_TYPE_SIMULATOR))
#define OPENSIM_IS_SIMULATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), \
                                           OPENSIM_TYPE_SIMULATOR))
#define OPENSIM_SIMULATOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), \
                                           OPENSIM_TYPE_SIMULATOR, \
                                           OpensimSimulatorClass))


typedef struct _OpensimSimulator        OpensimSimulator;
typedef struct _OpensimSimulatorClass   OpensimSimulatorClass;
typedef struct _OpensimSimulatorPrivate OpensimSimulatorPrivate;


struct _OpensimSimulator
{
  GObject parent_instance;
  
  /*
   * Properties:
   *   - model_name       (gchar *)    get/set
   *   - file_name        (gchar *)    get/set
   *   - output_type      (sim_output) get/set
   *   - output_file_name (gchar *)    get/set
   *   - valid_model      (gboolean)   get
   */
   
  /* instance members */
  OpensimSimulatorPrivate *priv;
};

struct _OpensimSimulatorClass
{
  GObjectClass parent_class;
  
  int               (* output_debug_info) (OpensimSimulator *simulator);
  int               (* run)               (OpensimSimulator *simulator);
  int               (* load)              (OpensimSimulator *simulator,
                                           gchar            *model_path);
  int               (* save)              (OpensimSimulator *simulator);
  int               (* new_variable)      (OpensimSimulator *simulator,
                                           gchar            *var_name,
                                           gchar            *var_eqn);
  OpensimVariable * (* get_variable)      (OpensimSimulator *simulator,
                                           gchar            *var_name);
  int               (* remove_variable)   (OpensimSimulator *simulator,
                                           gchar            *var_name);
};

/* used by OPENSIM_TYPE_SIMULATOR */
GType opensim_simulator_get_type ();

/*
 * Method definitions.
 */

/* public */
int 
opensim_simulator_run (OpensimSimulator *simulator);

int 
opensim_simulator_load (OpensimSimulator *simulator, 
                        gchar            *model_path);

int 
opensim_simulator_save (OpensimSimulator *simulator);

int
opensim_simulator_new_variable (OpensimSimulator *simulator, 
                                gchar            *var_name, 
                                gchar            *var_eqn);

OpensimVariable *
opensim_simulator_get_variable (OpensimSimulator *simulator, 
                                gchar            *var_name);

int 
opensim_simulator_remove_variable (OpensimSimulator *simulator, 
                                   gchar            *var_name);

int 
opensim_simulator_output_debug_info (OpensimSimulator *simulator);


G_END_DECLS

#endif /* __OPENSIM_SIMULATOR_H__ */

