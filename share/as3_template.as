package 
{
  // basic class to pack time and data arrays together
  public class SimData
  {
    public var index:Array
    public var data:Array

    // convenience constructor
    public function SimData(new_index:Array, new_data:Array)
    {
      index = new_index
      data = new_data
    }

    public function SimData(lookup:Array)
    {
      index = []
      data = []

      for each (entry in lookup)
      {
        index.push(entry[0]) 
        data.push(entry[1])
      }
    }
  }

  // **insert point**

  public class OpenSim
  {
    // for all functions returning a Number (that isn't data you're 
    // interested in), it will return 0 for normal operation, and 
    // a negative number if it failed (corresponding to the error it
    // encountered)

    // 'built-in' variables:
    //  - time
    //  - start_time (OS_start)
    //  - end_time (OS_end)
    //  - time_step (OS_timestep)
    //  - save_step (OS_savestep) (must be multiple of time step)

    private var data:Array
    private var curr_itr:Array
    private var next_itr:Array
    private var i:int
    private var do_save:Boolean
    private var save_count:int
    private var save_iterations:int
    private var timestep:Number

    public function OpenSim()
    {
      data = new Array()
      // initialization


      // store all the initialization data as the first item in 
      // the data 'dictionary'
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
      
      // keep track of the initialized data so that we can reset the 
      // simulation
      original_data = new Array(data)

      // initialize simulation
      Start()
    }


    private function simulate(time_span:Number)
    {
      // negative time span would just mess stuff up
      time_span = Math.max(0, time_span)

      // this is where the math will go, simulating from the current time 
      // to current time + time_span
      cur_time = data["time"]

      end_time = Math.min(data["OS_end"][0], cur_time + time_span);      

      for (time = cur_time; time < end_time; time = time + timestep)
      {
        data["rabbit_births"][i] = (data["rabbit_population"][i] * data["rabbit_birth_rate"][0])
        data["rabbit_crowding"][i] = (data["rabbit_population"][i] / data["carrying_capacity"][0])
        data["fox_consumption_of_rabbits"][i] = ((data["fox_population"][i] * data["fox_food_requirements"][0]) * lookup(data["fox_rabbit_consumption_lookup"], data["rabbit_crowding"][i]))
        data["rabbit_deaths"][i] = Math.max(((data["rabbit_population"][i] / data["average_rabbit_life"][0]) * lookup(data["effect_of_crowding_on_deaths_lookup"], data["rabbit_crowding"][i])),data["fox_consumption_of_rabbits"][i])
        data["fox_births"][i] = (data["fox_population"][i] * data["fox_birth_rate"][0])
        data["fox_food_availability"][i] = ((data["fox_consumption_of_rabbits"][i] / data["fox_population"][i]) / data["fox_food_requirements"][0])
        data["fox_deaths"][i] = ((data["fox_population"][i] / data["average_fox_life"][0]) * lookup(data["fox_mortality_lookup"], data["fox_food_availability"][i]))

        var j:int = i
        if (do_save)
        {
          j = i+1;
        }

        //updating stocks
        data["rabbit_population"][j] = (data["rabbit_population"][i] + ((data["rabbit_births"][i] - data["rabbit_deaths"][i]) * data["OS_timestep"][0]))
        data["fox_population"][j] = (data["fox_population"][i] + ((data["fox_births"][i] - data["fox_deaths"][i]) * data["OS_timestep"][0]))
        
        // determining whether or not to save results next iteration
        save_count = save_count + 1
        if (save_count >= save_iterations || time + timestep > data["OS_end"][0])
        {        
          do_save = true
          save_count = 0
        }
        else
        {
          do_save = false
        }
      }
    }


    // resets everything
    public function Start():Number
    {
      data = new Array(original_data)

      timestep = data["OS_timestep"][0]
      do_save = true
      
      i = 0
      save_count = 0
      save_iterations = data["OS_savestep"][0] / data["OS_timestep"][0]

      return 0
    }


    // length of time to simulate, NOT # of iterations.  if it doesn't match
    // a multiple of time_step, it will be rounded down to the nearest one.  
    // If it is greater than the time left in the simulation, it will simulate
    // to the end and not beyond
    public function Continue(time:Number):Number
    {
      simulate(time)

      return 0
    }


    // it is safe to call this even if there is no simulation running. (to make
    // sure there is no active simulation, for example
    public function Finish():Number
    {
      simulate(data["end_time"][0] - data["start_time"][0])
      return 0
    }


    // will stop any running simulations, return to the start_time, and reset all
    // constants and lookup values to their initial values
    public function Reset():Number
    {
      return Start()
    }

    // works on constants any time, but is only defined for other variables when 
    // a model is bring simulated, or when it is done simulating
    public function getValue(var_name:String):Number
    {
      // constant
      if (data[var_name].length == 1)
        return data[var_name][0]

      // not constant
      return data[var_name][i]
    }


    // allows you to set the value of a constant before a simulation, or a 
    // lookup during a simulation.  If you set the value of a lookup during a 
    // simulation, it will add (or replace) the value you set at the index of the 
    // current time
    public function setValue(var_name:String, var_value:Number):Number
    {
      data[var_name][0] = var_value
    }


    // if needed, set sparse to False to 'fill in' the data.  If sparse==False
    // and the variable is a lookup table, then the SimData returned
    // will have index and time arrays of length save_step*(end_time-start_time)
    // and any data not on a save_step will be removed, so as to keep 
    // time-indexed data of equal length.  It is inappropriate to specify a sparse 
    // value for a lookup table not indexed with respect to time.
    public function getData(var_name:String, sparse:Boolean=true):SimData
    {
      // this should work for regular data, but not lookups
      ret_val =  new SimData()
      ret_val.index = data["time"]
      ret_val.data = data[var_name]
      return ret_val
    }


    // used to set lookup data only.
    public function setData(var_name:String, var_data:SimData):Number
    {
      return -1
    }


    public function getUnits(var_name:String):String
    {
      // implement me please!
      return "";
    }


    public function getDescription(var_name:String):String
    {
      // implement me please!
      return "";
    }
    
    
    // simple lookup table implementation
    private function lookup(table:SimData, index:Number):Number
    {
      if (table.data.length == 0) 
        return 0

      var end:int = table.data.length - 1

      // if the request is outside the min or max, then we return
      // the nearest element of the array
      if      (index < table.index[0])   return table.data[0]
      else if (index > table.index[end]) return table.data[end]

      for (i:int = 0; i < table.data.length; i++)
      {
        x = table.index[i]
        y = table.data[i]

        if (index == x) return y
        if (index < x)
        {
          // slope = deltaY/deltaX
          slope = (y - table.data[i-1])/(x - table.index[i-1])
          return (index-table.index[i-1])*slope + table.data[i-1]
        }
      }
    }
  }
}

