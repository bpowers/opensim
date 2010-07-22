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
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif 

#define uint unsigned int

// help the compiler do better branch prediction
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

// different runtime errors we might be faced with
#define ERRNEW  1
#define ERRINIT 2
#define ERRSIM  3


// easily switch between double and float
typedef double real_t;


// the data structure is for each 'frame' of our simulation
struct data
{
	uint32_t count;
	real_t *values;
};


// the data structure representing a lookup table
struct table
{
	uint16_t size;
	real_t *x;
	real_t *y;
};


// the data structure representing a lookup table
struct tables
{
	uint32_t count;
	struct table **tables;
};


// forward declaration so we can use it in the function prototypes for
// sim_ops
struct sim;

// each different simulation will have different functions for these
// operations, so by storing the function pointers here, we can abstract
// most of the simulate funcion into the runtime, making the simulation
// code independent of the integration type (I think) (rk4, euler, etc)
struct sim_ops
{
	int32_t (*simulate_flows) (struct sim *);
	int32_t (*update_stocks) (struct sim *);
	int32_t (*integ_method) (struct sim *);
};


// control keeps track of the start, end,step and save_step constants.
// I'm putting these in their own structure because they aren't really
// appropriate to expose as constants to the rest of the model.
struct control
{
	real_t start;
	real_t end;
	real_t step;
	real_t save_step;
};


struct _class
{
	uint32_t num_vars;
	const char **var_names;

	uint32_t num_constants;
	const char **constant_names;

	struct sim_ops *ops;

	// the class provides defaults for the creation of new instances.
	struct control def_control;
	struct data *defaults;
	struct tables *def_lookups;
};
typedef struct _class_t class_t;


struct thread_info
{
	pthread_t threads[2];
};


// the sim structure represents an instance of a simulation
struct sim
{
	struct _class *_class;

	struct data *curr;
	struct data *next;

	struct control time;

	struct data *constants;

	struct tables *lookups;
};


/* initialize and exit from the opensim runtime */
int opensim_init();
void opensim_exit();

/* functions to free memory created during the course of simulating */
void opensim_data_free(struct data *sim);
void opensim_sim_free(struct sim *data);
void opensim_table_free(struct table *table);

/* create a new data_t structure to hold 'count' number of variables */
struct data *opensim_data_new(size_t count);

/* create a new sim_t structure, usually wrapped by a sim-specific
 * sim_new function */
struct sim *opensim_sim_new(struct class *_class);

/* create a new table to store lookup data */
struct table *opensim_table_new(size_t size);

/* perform the sim-independent initialization of a sim_t */
int opensim_sim_init(struct sim *sim);

/* functions to print out data */
int opensim_data_print(FILE *file, data_t *data);
int opensim_header_print(FILE *file, const char *names[], size_t count);

/* run a simulation using euler integration */
int opensim_simulate_euler(struct sim *sim);

/* simulate an instantiated model */
int opensim_simulate(struct sim *sim);


real_t max(real_t a, real_t b);
real_t lookup(struct table *table, real_t index);

#ifdef __cplusplus
}
#endif

#endif /* __OPENSIM_RUNTIME_H__ */

