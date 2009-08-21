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

#include "opensim/c_runtime.h"


enum _flags
{
  SIM = 0x0,
  OUTPUT = 0x1,
  QUIT = 0x2
};

/**
 * opensim_data_free:
 * @data: the data_t object to be freed
 *
 * This function frees the given data_t object. It is undefined to call
 * this function with @data being NULL.
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
 * Free the given sim_t object.
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
    opensim_data_free (sim->constants);

  free (sim);
}


/**
 * opensim_table_free:
 * @table: the table who is to be freed.
 *
 * This function frees the given table_t object. It is undefined to call
 * this function with @table being NULL.
 */
void
opensim_table_free (table_t *table)
{
  if (table->x)
    free (table->x);
  if (table->y)
    free (table->y);

  free (table);
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
  data_t *new_data = (data_t *)malloc (sizeof (data_t));
  if (!new_data)
  {
    fprintf (stderr, "couldn't allocate space for new data_t.\n");
    return NULL;
  }

  new_data->count = count;
  new_data->values = (real_t *)malloc (count * sizeof (real_t));
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
 * @lookups: lookup tables
 *
 * This function does the common tasks associated with creating a
 * correctly sized new sim_t.  It requires a number of parameters and
 * creates a sim_t to their specifications.
 *
 * Returns: a pointer to a correctly formed sim_t on success, NULL on error.
 */
sim_t *
opensim_sim_new (class_t *_class)
{
  sim_t *sim = (sim_t *)malloc (sizeof (sim_t));
  if (!sim)
  {
    fprintf (stderr, "couldn't allocate memory for instance.\n");
    return NULL;
  }

  sim->_class = _class;

  sim->time.start     = _class->def_control.start;
  sim->time.end       = _class->def_control.end;
  sim->time.step      = _class->def_control.step;
  sim->time.save_step = _class->def_control.save_step;

  sim->lookups = _class->def_lookups;

  sim->constants = opensim_data_new (_class->defaults->count);
  if (!sim->constants)
  {
    fprintf (stderr, "couldn't allocate memory for constants.\n");
    return NULL;
  }

  // initialize our instances constant values from the defaults
  for (uint32_t i=0; i<sim->constants->count; ++i)
    sim->constants->values[i] = _class->defaults->values[i];

  return sim;
}


/**
 * opensim_table_new:
 * @size: the size (number of x,y pairs) of the table to allocate.
 *
 * This function allocates a new table_t with enough space to store
 * @size number of two-tuples.
 *
 * Returns: a pointer to the new table_t on success, NULL on failure.
 */
table_t *
opensim_table_new (uint32_t size)
{
  table_t *new_table = (table_t *)malloc (sizeof (table_t));
  if (!new_table)
  {
    fprintf (stderr, "couldn't allocate space for new table_t.\n");
    return NULL;
  }

  new_table->size = size;
  new_table->x = (real_t *)malloc (size * sizeof (real_t));
  new_table->y = (real_t *)malloc (size * sizeof (real_t));
  if (!new_table->x || !new_table->y)
  {
    fprintf (stderr, "couldn't allocate array for new table_t.\n");
    // make sure we cleanup anything we managed to allocate.
    if (new_table->x)
      free (new_table->x);
    if (new_table->y)
      free (new_table->y);
    free (new_table);
    return NULL;
  }

  return new_table;
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

  sim->curr = opensim_data_new (sim->_class->num_vars);
  sim->next = opensim_data_new (sim->_class->num_vars);
  if (!sim->curr || !sim->next)
  {
    fprintf (stderr, "couldn't allocate memory for 'curr' or 'next'.\n");
    return -ERRINIT;
  }

  return 0;
}


void *
opensim_output_thread (void *param)
{

  return NULL;
}



/**
 * opensim_init:
 *
 * This initializes the opensim runtime system.  Its used for getting
 * threading and any state necessary set up.
 *
 * Returns: 0 on success, a negative error code on error.
 */
int32_t
opensim_init ()
{
  pthread_t output_thread;
  pthread_attr_t attr;

  pthread_attr_init (&attr);
  pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);

  return 0;
}


/**
 * opensim_exit:
 *
 * This exits the opensim runtime system, cleaning up state and theading.
 */
void
opensim_exit ()
{

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
opensim_header_print (FILE *file, const char *names[], uint32_t count)
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
  real_t save_step = sim->time.save_step;

  bool do_save = true;
  uint32_t save_count = 0;
  uint32_t save_iterations = save_step / step;

  opensim_header_print (stdout, sim->_class->var_names,
                        sim->_class->num_vars);

  for (real_t time = start; time <= end; time = time + step)
  {
    // set the time variable in the data here, so we don't have
    // to pass time explicitly to simulate functions.
    real_t *stime = &sim->curr->values[0];
    *stime = time;

    // now simulate the flows. by definition this should only effect
    // the 'curr' member of sim.
    sim->_class->ops->simulate_flows (sim);

    // update our stocks in preperation for the next iteration. this
    // should only effect the 'next' member of sim.
    sim->_class->ops->update_stocks (sim);

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

    // for now we just shuffle curr and next back and forth.  if we get
    // around to having a seperate thread for writing out the data, this
    // part will change
    data_t *tmp = sim->curr;
    sim->curr = sim->next;
    sim->next = tmp;
  }
  return 0;
}


/**
 * opensim_simulate:
 * @sim: the initialized simulation to be run.
 *
 * This function simulates an instantiated model.
 *
 * It uses the simulation method specified in _class->ops->integ_method.
 *
 * Returns: 0 on success, a negative number on failure.
 */
int32_t
opensim_simulate (sim_t *sim)
{
  return sim->_class->ops->integ_method (sim);
}

inline real_t
max (real_t a, real_t b)
{
  return a > b ? a : b;
}


real_t
lookup (table_t *table, real_t index)
{
  uint32_t size = table->size;
  if (unlikely(table->size == 0)) return 0;

  real_t *x = table->x;
  real_t *y = table->y;

  // if the request is outside the min or max, then we return
  // the nearest element of the array
  if (unlikely(index < x[0]))
    return y[0];
  else if (unlikely(index > x[size-1]))
    return y[size-1];

  // binary search makes more sense here
  uint16_t low = 0;
  uint16_t high = size;
  uint16_t mid;
  while (low < high)
  {
    mid = low + ((high-low)/2);

    if (x[mid] < index)
      low = mid + 1;
    else
      high = mid;
  }

  // at this point low == high, so using 'i' seems more readable.
  uint32_t i = low;
  if (unlikely(x[i] == index))
  {
    return y[i];
  }
  else
  {
    // slope = deltaY/deltaX
    real_t slope = (y[i] - y[i-1]) / (x[i] - x[i-1]);

    return (index-x[i-1])*slope + y[i-1];
  }

  return 0;
}

