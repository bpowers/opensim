/*===--- infection.c - a sample of what infection would codegen to be ---===//
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
 * This is the C version of what our infection model would codegen to
 * for our JIT.
 *
 *===--------------------------------------------------------------------===//
 */

#include "opensim/c_runtime.h"


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
int32_t infection_simulate_flows (sim_t *);
int32_t infection_update_stocks (sim_t *);

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
                          &infection_defaults,
                          NULL);
}


int32_t
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


int32_t
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

