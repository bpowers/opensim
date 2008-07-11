package opensim
{
  public class OpenSim
  {
OS_end = 20.000000
OS_savestep = 1.000000
OS_start = 0.000000
OS_timestep = 0.500000
infected = 2.000000
initial_infected = 23.000000
number_of_contacts_per_day = 1.000000
prob_of_infection = 0.500000
susceptible = 21.000000
total_population = 23.000000
#using euler integration
for time in frange(OS_start, OS_end, OS_timestep):
  proportion_susceptible = (susceptible / 21.000000)
  daily_contacts_per_infected = (infected * number_of_contacts_per_day)
  susceptibles_contacted_daily = (proportion_susceptible * daily_contacts_per_infected)
  infection_rate = (prob_of_infection * susceptibles_contacted_daily)

  #generally put print statements here
  print('%f,%f,%f,%f,%f,%f,%f' % (time, proportion_susceptible, daily_contacts_per_infected, susceptibles_contacted_daily, infection_rate, susceptible, infected))

  #updating stocks
  susceptible = (susceptible + (-infection_rate * OS_timestep))
  infected = (infected + (infection_rate * OS_timestep))
  }
}

