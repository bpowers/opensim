/*===--- rabbit.c - a sample of what rabbit would codegen to be ---------===//
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
 * This is the C version of what our rabbit model would codegen to
 * for our JIT.
 *
 *===--------------------------------------------------------------------===//
 */

#include "opensim/c_runtime.h"


//==--- generated sim-specific code --------------------------------------==//

/*
    constant variables:
      0: initial_rabbit_population
      1: average_rabbit_life
      2: rabbit_birth_rate
      3: initial_fox_population
      4: average_fox_life
      5: fox_birth_rate
      6: fox_food_requirements
      7: carrying_capacity

    data variables:
      0: time
      1: rabbit_crowding
      2: fox_consumption_of_rabbits
      3: rabbit_deaths
      4: rabbit_births
      5: fox_births
      6: fox_food_availability
      7: fox_deaths
      8: fox_population
      9: rabbit_population
 */

// forward declarations of some functions, so we can point to them in
// our rabbit_ops structure.
int32_t rabbit_simulate_flows (sim_t *);
int32_t rabbit_update_stocks (sim_t *);

// these are the constants specified by the rabbit fox model. you can
// change a constant without having to recompile or regenerate any
// bitcode.
static sim_ops rabbit_ops = {
  .simulate_flows = rabbit_simulate_flows,
  .update_stocks  = rabbit_update_stocks
};

static char *rabbit_var_names[] = {"time",
                                   "rabbit_crowding",
                                   "fox_consumption_of_rabbits",
                                   "rabbit_deaths",
                                   "rabbit_births",
                                   "fox_births",
                                   "fox_food_availability",
                                   "fox_deaths",
                                   "fox_population",
                                   "rabbit_population"
                                  };

static char *rabbit_const_names[] = {"initial_rabbit_population",
                                     "average_rabbit_life",
                                     "rabbit_birth_rate",
                                     "initial_fox_population",
                                     "average_fox_life",
                                     "fox_birth_rate",
                                     "fox_food_requirements",
                                     "carrying_capacity"
                                    };

static class_info rabbit_info = {
  .var_names = rabbit_var_names,
  .num_vars = sizeof (rabbit_var_names) / sizeof (char *),

  .constant_names = rabbit_const_names,
  .num_constants = sizeof (rabbit_const_names) / sizeof (char *)
};

static control_t rabbit_def_control = {
  .start     = 0.0,
  .end       = 50.0,
  .step      = 0.015625,
  .save_step = 1.0
};

static real_t rabbit_constants[] = {500.0, // initial_rabbit_population
                                    2.0,   // average_rabbit_life
                                    2.0,   // rabbit_birth_rate
                                    30.0,  // initial_fox_population
                                    4.0,   // average_fox_life
                                    0.25,  // fox_birth_rate
                                    25.0,  // fox_food_requirements
                                    500.0, // carrying_capacity
                                   };

static data_t rabbit_defaults = {
  .values = (real_t *)rabbit_constants,
  .count  = sizeof (rabbit_constants) / sizeof (real_t)
};

static real_t lookup1_x[] = {0.0, 1.0, 2.0, 6.0};
static real_t lookup1_y[] = {0.0, 1.0, 2.0, 2.0};
static table_t lookup1 = {
  .x = (real_t *)lookup1_x,
  .y = (real_t *)lookup1_y,
  .size = sizeof (lookup1_x) / sizeof (real_t)
};

static real_t lookup2_x[] = {0.0,  0.3, 0.5, 1.0, 2.0};
static real_t lookup2_y[] = {20.0, 5.0, 2.0, 1.0, 0.5};
static table_t lookup2 = {
  .x = (real_t *)lookup2_x,
  .y = (real_t *)lookup2_y,
  .size = sizeof (lookup2_x) / sizeof (real_t)
};

static real_t lookup3_x[] = {0.0,  3.0, 6.0, 8.0,  10.0};
static real_t lookup3_y[] = {0.75, 2.5, 6.0, 11.0, 20.0};
static table_t lookup3 = {
  .x = (real_t *)lookup3_x,
  .y = (real_t *)lookup3_y,
  .size = sizeof (lookup3_x) / sizeof (real_t)
};

static table_t *tables[] = {&lookup1, &lookup2, &lookup3};
static tables_t rabbit_lookups = {
  .tables = tables,
  .count = sizeof (tables) / sizeof (table_t *)
};


sim_t *
rabbit_new ()
{
  // basically all we have to do here is call the runtime sim_new function
  // with pointers to our infection specific structures
  return opensim_sim_new (&rabbit_ops,
                          &rabbit_info,
                          &rabbit_def_control,
                          &rabbit_defaults,
                          &rabbit_lookups);
}


int32_t
rabbit_init (sim_t *self)
{
  opensim_sim_init (self);

  real_t *initial_rabbit_population = &self->constants[0];
  real_t *initial_fox_population = &self->constants[3];
  real_t *fox_population = &self->curr->values[8];
  real_t *rabbit_population = &self->curr->values[9];

  // initialize all the initial values of the stocks. for other
  // simulations, there might be some maths in here, so that
  // the initial values of stocks can be based on an equation
  // involving constants.
  *rabbit_population = *initial_rabbit_population;
  *fox_population = *initial_fox_population;

  return 0;
}


int32_t
rabbit_simulate_flows (sim_t *self)
{
  // these are convience aliases to make the equations readable.  they
  // get completely optimized away when compiled
  real_t *initial_rabbit_population  = &self->constants[0];
  real_t *average_rabbit_life        = &self->constants[1];
  real_t *rabbit_birth_rate          = &self->constants[2];
  real_t *initial_fox_population     = &self->constants[3];
  real_t *average_fox_life           = &self->constants[4];
  real_t *fox_birth_rate             = &self->constants[5];
  real_t *fox_food_requirements      = &self->constants[6];
  real_t *carrying_capacity          = &self->constants[7];
  real_t *time                       = &self->curr->values[0];
  real_t *rabbit_crowding            = &self->curr->values[1];
  real_t *fox_consumption_of_rabbits = &self->curr->values[2];
  real_t *rabbit_deaths              = &self->curr->values[3];
  real_t *rabbit_births              = &self->curr->values[4];
  real_t *fox_births                 = &self->curr->values[5];
  real_t *fox_food_availability      = &self->curr->values[6];
  real_t *fox_deaths                 = &self->curr->values[7];
  real_t *fox_population             = &self->curr->values[8];
  real_t *rabbit_population          = &self->curr->values[9];
  table_t *fox_rabbit_consumption_lookup       = self->lookups->tables[0];
  table_t *fox_mortality_lookup                = self->lookups->tables[1];
  table_t *effect_of_crowding_on_deaths_lookup = self->lookups->tables[2];

  // calculate flows
  
  *fox_consumption_of_rabbits = *fox_population * *fox_food_requirements *
                                lookup (fox_rabbit_consumption_lookup,
                                       *rabbit_crowding);

  *fox_births = *fox_population * *fox_birth_rate;
  *fox_deaths = *fox_population / *average_fox_life *
                lookup (fox_mortality_lookup, *fox_food_availability);

  *fox_food_availability = *fox_consumption_of_rabbits / *fox_population /
                           *fox_food_requirements;

  *rabbit_crowding = *rabbit_population / *carrying_capacity;

  *rabbit_births = *rabbit_population * *rabbit_birth_rate;
  *rabbit_deaths = max (*rabbit_population / *average_rabbit_life *
                       lookup (effect_of_crowding_on_deaths_lookup,
                               *rabbit_crowding),
                       *fox_consumption_of_rabbits);

  return 0;
}


int
rabbit_update_stocks (sim_t *self)
{
  // these are convience aliases to make the equations readable.  they
  // get completely optimized away when compiled
  real_t *step                       = &self->time.step;
  real_t *rabbit_deaths              = &self->curr->values[3];
  real_t *rabbit_births              = &self->curr->values[4];
  real_t *fox_births                 = &self->curr->values[5];
  real_t *fox_deaths                 = &self->curr->values[7];
  real_t *fox_population             = &self->curr->values[8];
  real_t *rabbit_population          = &self->curr->values[9];
  real_t *fox_population_next        = &self->next->values[8];
  real_t *rabbit_population_next     = &self->next->values[9];
  real_t rabbit_population_net_flow;
  real_t fox_population_net_flow;

  rabbit_population_net_flow = *rabbit_births - *rabbit_deaths;
  fox_population_net_flow = *fox_births - *fox_deaths;
  *rabbit_population_next = *rabbit_population +
                            (rabbit_population_net_flow * *step);
  *fox_population_next = *fox_population +
                         (fox_population_net_flow * *step);

  return 0;
}


//==--- driver -----------------------------------------------------------==//

int
main (int argc, char *argv[])
{
  sim_t *sim;
  int ret;

  // allocate space for a new simulation.
  sim = rabbit_new ();
  if (!sim)
    return -ERRNEW;

  // if you want to change a constant for this run, you can do it here

  // before we simulate, we have to initialize the sim.  This means that
  // we allocate space to store the values for auxiliary and flow
  // variables, and set the initial values for the stocks.
  ret = rabbit_init (sim);
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

