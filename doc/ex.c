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


// the sim structure represents an instance of a simulation
struct _sim
{
  data_t *curr;
  data_t *next;

  uint32_t count;
  real_t *constants;
};
typedef struct _sim sim_t;


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
opensim_data_new (size_t count)
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


//==--- generated sim-specific code --------------------------------------==//

/*
    sim variables:
      0: time_start
      1: time_end
      2: time_step
      3: time_savestep
      4: number_of_contacts_per_day
      5: prob_of_infection
      6: total_population
      7: susceptible
      8: infected

    data variables:
      0: time
      1: daily_contacts_per_infected
      2: proportion_susceptible
      3: susceptibles_contacted_daily
      4: infection_rate
      5: susceptible
      6: infected
 */

// these are the constants specified by the rabbit fox model. you can
// change a constant without having to recompile or regenerate any
// bitcode.
static const real_t infection_constants[] = {1.0,  // time_start
                                             20.0, // time_end
                                             0.5,  // time_step
                                             1.0,  // time_savestep
                                             1.0,  // number_of_contacts_per_d
                                             0.5,  // prob_of_infection
                                             23.0, // total_population
                                             21.0, // susceptible
                                             2.0,  // infected
                                            };

static const data_t infection_defaults = {
  .values = (real_t *)infection_constants,
  .count  = sizeof (infection_constants) / sizeof (real_t)
};

static const size_t infection_num_vars = 7;



// forward-declatations
int infection_init (sim_t *);
data_t *opensim_data_new (size_t size);
int opensim_data_print (FILE *, data_t *);


sim_t *
infection_new ()
{
  sim_t *self = malloc (sizeof (sim_t));
  if (!self)
  {
    fprintf (stderr, "couldn't allocate memory for instance.\n");
    return NULL;
  }

  self->count = infection_defaults.count;
  self->constants = malloc (infection_defaults.count * sizeof (real_t));
  if (!self->constants)
  {
    fprintf (stderr, "couldn't allocate memory for constants.\n");
    return NULL;
  }

  // initialize our instances constant values from the defaults
  for (size_t i=0; i<infection_defaults.count; ++i)
    self->constants[i] = infection_defaults.values[i];

  return self;
}


int
infection_init (sim_t *self)
{
  if (self->curr)
    opensim_data_free (self->curr);
  if (self->next)
    opensim_data_free (self->next);

  self->curr = opensim_data_new (infection_num_vars);
  self->next = opensim_data_new (infection_num_vars);
  if (!self->curr || !self->next)
  {
    fprintf (stderr, "couldn't allocate memory for 'curr' or 'next'.\n");
  }

  // initialize all the initial values of the stocks
  real_t *susceptible = &self->curr->values[5];
  real_t *infected = &self->curr->values[6];

  *susceptible = self->constants[7];
  *infected = self->constants[8];

  return 0;
}


int
infection_simulate(sim_t *self)
{
  real_t start     = self->constants[0];
  real_t end       = self->constants[1];
  real_t step      = self->constants[2];
  real_t save_step = self->constants[3];
  bool do_save = true;
  uint32_t save_count = 0;
  uint32_t save_iterations = save_step / step;

  real_t *number_of_contacts_per_day = &self->constants[4];
  real_t *prob_of_infection          = &self->constants[5];
  real_t *total_population           = &self->constants[6];

  for (double time = start; time <= end; time = time + step)
  { 
    // make some aliases to have things easier to read
    real_t *stime                        = &self->curr->values[0];
    real_t *daily_contacts_per_infected  = &self->curr->values[1];
    real_t *proportion_susceptible       = &self->curr->values[2];
    real_t *susceptibles_contacted_daily = &self->curr->values[3];
    real_t *infection_rate               = &self->curr->values[4];
    real_t *susceptible                  = &self->curr->values[5];
    real_t *infected                     = &self->curr->values[6];
    real_t *susceptible_next             = &self->next->values[5];
    real_t *infected_next                = &self->next->values[6];
    real_t susceptible_net_flow;
    real_t infected_net_flow;

    *stime = time;

    // calculate flows
    *daily_contacts_per_infected = (*infected * *number_of_contacts_per_day);
    *proportion_susceptible = (*susceptible / 21.0);
    *susceptibles_contacted_daily = (*proportion_susceptible * 
                                     *daily_contacts_per_infected);
    *infection_rate = (*prob_of_infection * *susceptibles_contacted_daily);
    susceptible_net_flow = -(*infection_rate);
    infected_net_flow = *infection_rate;

    if (do_save)
      opensim_data_print (stdout, self->curr);

    *susceptible_next = *susceptible + (susceptible_net_flow * step);
    *infected_next = *infected + (infected_net_flow * step);

    ++save_count;
    if (save_count >= save_iterations || time + step > end)
    {
      do_save = true;
      save_count = 0;
    }
    else
      do_save = false;

    data_t *tmp = self->curr;
    self->curr = self->next;
    self->next = tmp;
  }
  return 0;
}


//==--- driver -----------------------------------------------------------==//

int
main (int argc, char *argv[])
{
  sim_t *run;
  int ret;

  // try to allocate a new structure;
  run = infection_new ();
  if (!run)
    return -ERRNEW;

  // if you want to change a constant for this run, you can do it here

  ret = infection_init (run);
  if (ret != 0)
    return ret;

  ret = infection_simulate (run);
  if (ret != 0)
    return ret;

  opensim_sim_free (run);

  return 0;
}

