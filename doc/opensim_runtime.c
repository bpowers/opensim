/*===--- opensim-runtime.c - OpenSim's LLVM runtime ---------------------===//
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
 * This implements OpenSim's runtime.
 *
 *===--------------------------------------------------------------------===//
 */

#include "opensim_runtime.h"


/**
 * opensim_data_free:
 * @data: the data_t object to be freed
 *
 * This function frees the given data_t object. It is undefined to call
 * this function with @sim being NULL.
 */
void
opensim_data_free (data_t *data)
{
  if (data->values)
    free (data->values);

  free (data);
}


/**
 * opensim_sim_free:
 * @sim: the sim_t object to be freed
 *
 * This function frees the given sim_t object and any associated data_t
 * objects.  It is undefined to call this function with @sim being NULL.
 */
void
opensim_sim_free (sim_t *sim)
{
  if (sim->curr)
    opensim_data_free (sim->curr);

  if (sim->next)
    opensim_data_free (sim->next);

  if (sim->constants)
    free (sim->constants);

  free (sim);
}


/**
 * opensim_data_new:
 * @count: the number of elements in the new data_t.
 *
 * This function allocates a new data_t with enough space to store
 * @count number of values.
 *
 * Returns: a pointer to the new data_t on success, NULL on failure.
 */
data_t *
opensim_data_new (uint32_t count)
{
  data_t *new_data = malloc (sizeof (data_t));
  if (!new_data)
  {
    fprintf (stderr, "couldn't allocate space for new data_t.\n");
    return NULL;
  }

  new_data->count = count;
  new_data->values = malloc (count * sizeof (real_t));
  if (!new_data->values)
  {
    fprintf (stderr, "couldn't allocate array for new data_t.\n");
    free (new_data);
    return NULL;
  }

  return new_data;
}


/**
 * opensim_sim_new:
 * @ops: class operations
 * @info: class information
 * @control: time variables
 * @defaults: default constants
 *
 * This function does the common tasks associated with creating a
 * correctly sized new sim_t.  It requires a number of parameters and
 * creates a sim_t to their specifications.
 *
 * Returns: a pointer to a correctly formed sim_t on success, NULL on error.
 */
sim_t *
opensim_sim_new (sim_ops *ops,
                 class_info *info,
                 control_t *control,
                 data_t *defaults)
{
  sim_t *sim = malloc (sizeof (sim_t));
  if (!sim)
  {
    fprintf (stderr, "couldn't allocate memory for instance.\n");
    return NULL;
  }

  sim->ops = ops;
  sim->info = info;

  sim->time.start     = control->start;
  sim->time.end       = control->end;
  sim->time.step      = control->step;
  sim->time.save_step = control->save_step;

  sim->count = defaults->count;
  sim->constants = malloc (defaults->count * sizeof (real_t));
  if (!sim->constants)
  {
    fprintf (stderr, "couldn't allocate memory for constants.\n");
    return NULL;
  }

  // initialize our instances constant values from the defaults
  for (uint32_t i=0; i<defaults->count; ++i)
    sim->constants[i] = defaults->values[i];

  return sim;
}


/**
 * opensim_sim_init:
 * @sim: the simulation to be initialized
 *
 * This function performs the common initialization for sim_t objects.
 * If the sim has any existing data it frees it, then allocates new
 * data for curr and next.
 *
 * Returns: 0 on success, a negative error code on error.
 */
int32_t
opensim_sim_init (sim_t *sim)
{
  if (sim->curr)
    opensim_data_free (sim->curr);
  if (sim->next)
    opensim_data_free (sim->next);

  sim->curr = opensim_data_new (sim->info->num_vars);
  sim->next = opensim_data_new (sim->info->num_vars);
  if (!sim->curr || !sim->next)
  {
    fprintf (stderr, "couldn't allocate memory for 'curr' or 'next'.\n");
    return -ERRINIT;
  }

  return 0;
}


/**
 * opensim_data_print:
 * @file: an open FILE handle to write our output to.
 * @data: a data_t* containing the data we want printed
 *
 * This takes the values stored in data and prints them out to the
 * specified file in the format:
 * val1,val2,val3
 * We put a comma between each value, but not a trailing comma.
 *
 * Returns: 0 on success, a negative number on failure.
 */
int32_t
opensim_data_print (FILE *file, data_t *data)
{
  // gracefully handle NULL and zero-length data.
  if (!data || data->count == 0)
    return -1;

  fprintf (file, "%f", data->values[0]);
  for (uint32_t i = 1; i < data->count; ++i)
    fprintf (file, ",%f", data->values[i]);
  fprintf (file, "\n");

  return 0;
}


/**
 * opensim_header_print:
 * @file: an open FILE handle to write our output to.
 * @names: a char** containing the names we want printed
 * @count: the number of names to print
 *
 * This takes the strings stored in names and prints them out to the
 * specified file in the format:
 * name1,name2,name3
 * We put a comma between each value, but not a trailing comma.
 *
 * Returns: 0 on success, a negative number on failure.
 */
int32_t
opensim_header_print (FILE *file, char *names[], uint32_t count)
{
  // gracefully handle NULL and zero-length data.
  if (!names || count == 0)
    return -1;

  fprintf (file, "%s", names[0]);
  for (uint32_t i = 1; i < count; ++i)
    fprintf (file, ",%s", names[i]);
  fprintf (file, "\n");

  return 0;
}


/**
 * opensim_simulate_euler:
 * @sim: the initialized simulation to be run.
 *
 * This function uses euler integration to simulate a function from
 * start to finish.
 *
 * Returns: 0 on success, a negative number on failure.
 */
int32_t
opensim_simulate_euler (sim_t *sim)
{
  real_t start     = sim->time.start;
  real_t end       = sim->time.end;
  real_t step      = sim->time.step;
  real_t save_step = sim->time.save_step
;
  bool do_save = true;
  uint32_t save_count = 0;
  uint32_t save_iterations = save_step / step;

  opensim_header_print (stdout, sim->info->var_names, sim->info->num_vars);

  for (double time = start; time <= end; time = time + step)
  {
    // set the time variable in the data here, so we don't have
    // to pass time explicitly to simulate functions.
    real_t *stime = &sim->curr->values[0];
    *stime = time;

    // now simulate the flows. by definition this should only effect
    // the 'curr' member of sim.
    sim->ops->simulate_flows (sim);

    // update our stocks in preperation for the next iteration. this
    // should only effect the 'next' member of sim.
    sim->ops->update_stocks (sim);

    // if we're suppose to 'save' or print the values from this
    // timestep, then we do it here.
    if (do_save)
      opensim_data_print (stdout, sim->curr);

    ++save_count;
    if (save_count >= save_iterations || time + step > end)
    {
      do_save = true;
      save_count = 0;
    }
    else
      do_save = false;

    data_t *tmp = sim->curr;
    sim->curr = sim->next;
    sim->next = tmp;
  }
  return 0;
}

