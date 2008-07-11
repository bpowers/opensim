package opensim
{
  public class OpenSim
  {
      data["OS_end"] = [50.000000]
      data["OS_savestep"] = [1.000000]
      data["OS_start"] = [0.000000]
      data["OS_timestep"] = [0.015625]
      data["average_fox_life"] = [4.000000]
      data["average_rabbit_life"] = [2.000000]
      data["carrying_capacity"] = [500.000000]
      data["effect_of_crowding_on_deaths_lookup"] = [[[0.000000, 0.750000], [3.000000, 2.500000], [6.000000, 6.000000], [8.000000, 11.000000], [10.000000, 20.000000]]]
      data["fox_birth_rate"] = [0.250000]
      data["fox_food_requirements"] = [25.000000]
      data["fox_mortality_lookup"] = [[[0.000000, 20.000000], [0.300000, 5.000000], [0.500000, 2.000000], [1.000000, 1.000000], [2.000000, 0.500000]]]
      data["fox_population"] = [initial_fox_population]
      data["fox_rabbit_consumption_lookup"] = [[[0.000000, 0.000000], [1.000000, 1.000000], [2.000000, 2.000000], [6.000000, 2.000000]]]
      data["initial_fox_population"] = [30.000000]
      data["initial_rabbit_population"] = [500.000000]
      data["rabbit_birth_rate"] = [2.000000]
      data["rabbit_population"] = [initial_rabbit_population]
#using euler integration
for time in frange(OS_start, OS_end, OS_timestep):
      rabbit_births = (rabbit_population * rabbit_birth_rate)
      rabbit_crowding = (rabbit_population / carrying_capacity)
      fox_consumption_of_rabbits = ((fox_population * fox_food_requirements) * lookup(fox_rabbit_consumption_lookup, rabbit_crowding))
      rabbit_deaths = Math.max(((rabbit_population / average_rabbit_life) * lookup(effect_of_crowding_on_deaths_lookup, rabbit_crowding)),fox_consumption_of_rabbits)
      fox_births = (fox_population * fox_birth_rate)
      fox_food_availability = ((fox_consumption_of_rabbits / fox_population) / fox_food_requirements)
      fox_deaths = ((fox_population / average_fox_life) * lookup(fox_mortality_lookup, fox_food_availability))

      #generally put print statements here
      print('%f,%f,%f,%f,%f,%f,%f,%f,%f,%f' % (time, rabbit_births, rabbit_crowding, fox_consumption_of_rabbits, rabbit_deaths, rabbit_population, fox_births, fox_food_availability, fox_deaths, fox_population))

      #updating stocks
      rabbit_population = (rabbit_population + ((rabbit_births - rabbit_deaths) * OS_timestep))
      fox_population = (fox_population + ((fox_births - fox_deaths) * OS_timestep))
  }
}

