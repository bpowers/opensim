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
      data["effect_of_crowding_on_deaths_lookup"] = new SimData([[0.000000, 0.750000], [3.000000, 2.500000], [6.000000, 6.000000], [8.000000, 11.000000], [10.000000, 20.000000]])
      data["fox_birth_rate"] = [0.250000]
      data["fox_food_requirements"] = [25.000000]
      data["fox_mortality_lookup"] = new SimData([[0.000000, 20.000000], [0.300000, 5.000000], [0.500000, 2.000000], [1.000000, 1.000000], [2.000000, 0.500000]])
      data["fox_population"] = [data["initial_fox_population"][0]]
      data["fox_rabbit_consumption_lookup"] = new SimData([[0.000000, 0.000000], [1.000000, 1.000000], [2.000000, 2.000000], [6.000000, 2.000000]])
      data["initial_fox_population"] = [30.000000]
      data["initial_rabbit_population"] = [500.000000]
      data["rabbit_birth_rate"] = [2.000000]
      data["rabbit_population"] = [data["initial_rabbit_population"][0]]
      data["time"] = [data["OS_start"][0]]
#using euler integration
for time in frange(OS_start, OS_end, OS_timestep):
        data["rabbit_births"][i] = (data["rabbit_population"][i] * data["rabbit_birth_rate"][0])
        data["rabbit_crowding"][i] = (data["rabbit_population"][i] / data["carrying_capacity"][0])
        data["fox_consumption_of_rabbits"][i] = ((data["fox_population"][i] * data["fox_food_requirements"][0]) * lookup(data["fox_rabbit_consumption_lookup", data["rabbit_crowding"][i]))
        data["rabbit_deaths"][i] = Math.max(((data["rabbit_population"][i] / data["average_rabbit_life"][0]) * lookup(data["effect_of_crowding_on_deaths_lookup", data["rabbit_crowding"][i])),data["fox_consumption_of_rabbits"][i])
        data["fox_births"][i] = (data["fox_population"][i] * data["fox_birth_rate"][0])
        data["fox_food_availability"][i] = ((data["fox_consumption_of_rabbits"][i] / data["fox_population"][i]) / data["fox_food_requirements"][0])
        data["fox_deaths"][i] = ((data["fox_population"][i] / data["average_fox_life"][0]) * lookup(data["fox_mortality_lookup", data["fox_food_availability"][i]))

        //updating stocks
        data["rabbit_population"][j] = (data["rabbit_population"][i] + ((data["rabbit_births"][i] - data["rabbit_deaths"][i]) * data["OS_timestep"][0]))
        data["fox_population"][j] = (data["fox_population"][i] + ((data["fox_births"][i] - data["fox_deaths"][i]) * data["OS_timestep"][0]))
  }
}

