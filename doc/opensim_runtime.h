/*===--- opensim-runtime.h - API for OpenSim's LLVM runtime -------------===//
 *
 * Copyright 2009 Bobby Powers
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
 * along with OpenSim.  If not, see <http://www.gnu.org/licenses/>.
 *
 *===--------------------------------------------------------------------===//
 *
 * This header defines the public API for interacting with OpenSim's runtime.
 *
 *===--------------------------------------------------------------------===//
 */


#ifndef __OPENSIM_RUNTIME_H__
#define __OPENSIM_RUNTIME_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>


// different runtime errors we might be faced with
#define ERRNEW  1
#define ERRINIT 2
#define ERRSIM  3


// easily switch between double and float
typedef double real_t;


// the data structure is for each 'frame' of our simulation
struct _data
{
  uint32_t count;
  real_t *values;
};
typedef struct _data data_t;


struct _sim;
typedef struct _sim sim_t;

// each different simulation will have different functions for these
// operations, so by storing the function pointers here, we can abstract
// most of the simulate funcion into the runtime, making the simulation
// code independent of the integration type (I think) (rk4, euler, etc)
struct _sim_ops
{
  int32_t (*simulate_flows) (sim_t *);
  int32_t (*update_stocks) (sim_t *);
};
typedef struct _sim_ops sim_ops;

// control keeps track of the start, end,step and save_step constants.
// I'm putting these in their own structure because they aren't really
// appropriate to expose as constants to the rest of the model.
struct _control
{
  real_t start;
  real_t end;
  real_t step;
  real_t save_step;
};
typedef struct _control control_t;


struct _class
{
  uint32_t num_vars;
  char **var_names;

  uint32_t num_constants;
  char **constant_names;
};
typedef struct _class class_info;


// the sim structure represents an instance of a simulation
struct _sim
{
  sim_ops *ops;
  class_info *info;

  data_t *curr;
  data_t *next;

  control_t time;

  uint32_t count;
  real_t *constants;
};

/* functions to free memory created during the course of simulating */
void opensim_data_free (data_t *sim);
void opensim_sim_free (sim_t *data);

/* create a new data_t structure to hold 'count' number of variables */
data_t *opensim_data_new (uint32_t count);

/* create a new sim_t structure, usually wrapped by a sim-specific
 * sim_new function */
sim_t *opensim_sim_new (sim_ops *ops,
                        class_info *info,
                        control_t *control,
                        data_t *defaults);

/* perform the sim-independent initialization of a sim_t */
int32_t opensim_sim_init (sim_t *sim);

/* functions to print out data */
int32_t opensim_data_print (FILE *file, data_t *data);
int32_t opensim_header_print (FILE *file, char *names[], uint32_t count);

/* run a simulation using euler integration */
int32_t opensim_simulate_euler (sim_t *sim);


#endif /* __OPENSIM_RUNTIME_H__ */

