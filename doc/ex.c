#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>


//==--- runtime ----------------------------------------------------------==//

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
  int (*simulate_flows) (sim_t *);
  int (*update_stocks) (sim_t *);
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
  for (size_t i=0; i<defaults->count; ++i)
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
int
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
int
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
int
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
int
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


//==--- generated sim-specific code --------------------------------------==//

/*
    sim variables:
      0: number_of_contacts_per_day
      1: prob_of_infection
      2: total_population
      3: susceptible
      4: infected

    data variables:
      0: time
      1: daily_contacts_per_infected
      2: proportion_susceptible
      3: susceptibles_contacted_daily
      4: infection_rate
      5: susceptible
      6: infected
 */

// forward declarations of some functions, so we can point to them in
// our infection_ops structure.
int infection_simulate_flows (sim_t *);
int infection_update_stocks (sim_t *);

// these are the constants specified by the rabbit fox model. you can
// change a constant without having to recompile or regenerate any
// bitcode.
static sim_ops infection_ops = {
  .simulate_flows = infection_simulate_flows,
  .update_stocks  = infection_update_stocks
};

static char *infection_var_names[] = {"time",
                                      "daily_contacts_per_infected",
                                      "proportion_susceptible",
                                      "susceptibles_contacted_daily",
                                      "infection_rate",
                                      "susceptible",
                                      "infected"
                                     };

static char *infection_const_names[] = {"number_of_contacts_per_day",
                                        "prob_of_infection",
                                        "total_population",
                                        "susceptible",
                                        "infected"
                                       };

static class_info infection_info = {
  .var_names = infection_var_names,
  .num_vars = sizeof (infection_var_names) / sizeof (char *),

  .constant_names = infection_const_names,
  .num_constants = sizeof (infection_const_names) / sizeof (char *)
};

static control_t infection_def_control = {
  .start     = 0.0,
  .end       = 20.0,
  .step      = 0.5,
  .save_step = 1.0
};

static real_t infection_constants[] = {1.0,  // number_of_contacts_per_day
                                       0.5,  // prob_of_infection
                                       23.0, // total_population
                                       21.0, // susceptible
                                       2.0,  // infected
                                      };

static data_t infection_defaults = {
  .values = (real_t *)infection_constants,
  .count  = sizeof (infection_constants) / sizeof (real_t)
};


sim_t *
infection_new ()
{
  // basically all we have to do here is call the runtime sim_new function
  // with pointers to our infection specific structures
  return opensim_sim_new (&infection_ops,
                          &infection_info,
                          &infection_def_control,
                          &infection_defaults);
}


int
infection_init (sim_t *self)
{
  opensim_sim_init (self);

  real_t *susceptible = &self->curr->values[5];
  real_t *infected = &self->curr->values[6];

  // initialize all the initial values of the stocks. for other
  // simulations, there might be some maths in here, so that
  // the initial values of stocks can be based on an equation
  // involving constants.
  *susceptible = self->constants[3];
  *infected = self->constants[4];

  return 0;
}


int
infection_simulate_flows (sim_t *self)
{
  // these are convience aliases to make the equations readable.  they
  // get completely optimized away when compiled
  real_t *number_of_contacts_per_day   = &self->constants[0];
  real_t *prob_of_infection            = &self->constants[1];
  real_t *total_population             = &self->constants[2];
  real_t *time                         = &self->curr->values[0];
  real_t *daily_contacts_per_infected  = &self->curr->values[1];
  real_t *proportion_susceptible       = &self->curr->values[2];
  real_t *susceptibles_contacted_daily = &self->curr->values[3];
  real_t *infection_rate               = &self->curr->values[4];
  real_t *susceptible                  = &self->curr->values[5];
  real_t *infected                     = &self->curr->values[6];

  // calculate flows
  *daily_contacts_per_infected = (*infected * *number_of_contacts_per_day);
  *proportion_susceptible = (*susceptible / 21.0);
  *susceptibles_contacted_daily = (*proportion_susceptible *
                                   *daily_contacts_per_infected);
  *infection_rate = (*prob_of_infection * *susceptibles_contacted_daily);

  return 0;
}


int
infection_update_stocks (sim_t *self)
{
  // these are convience aliases to make the equations readable.  they
  // get completely optimized away when compiled
  real_t *step                         = &self->time.step;
  real_t *infection_rate               = &self->curr->values[4];
  real_t *susceptible                  = &self->curr->values[5];
  real_t *infected                     = &self->curr->values[6];
  real_t *susceptible_next             = &self->next->values[5];
  real_t *infected_next                = &self->next->values[6];
  real_t susceptible_net_flow;
  real_t infected_net_flow;

  susceptible_net_flow = -(*infection_rate);
  infected_net_flow = *infection_rate;
  *susceptible_next = *susceptible + (susceptible_net_flow * *step);
  *infected_next = *infected + (infected_net_flow * *step);

  return 0;
}


//==--- driver -----------------------------------------------------------==//

int
main (int argc, char *argv[])
{
  sim_t *sim;
  int ret;

  // allocate space for a new simulation.
  sim = infection_new ();
  if (!sim)
    return -ERRNEW;

  // if you want to change a constant for this run, you can do it here

  // before we simulate, we have to initialize the sim.  This means that
  // we allocate space to store the values for auxiliary and flow
  // variables, and set the initial values for the stocks.
  ret = infection_init (sim);
  if (ret != 0)
    return ret;

  // now that we have an initialized sim object, we can simulate it
  // from start to finish using this convenience function.
  ret = opensim_simulate_euler (sim);
  if (ret != 0)
    return ret;


  // finally, its good practice to free the simulation to get back its
  // memory.  Not so important here, but if this were embedded in a
  // larger program, you would begin to leak memory.
  opensim_sim_free (sim);

  return 0;
}

