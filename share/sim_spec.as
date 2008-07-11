package opensim
{
  // basic class to pack time and data arrays together
  public class SimData
  {
    public var index:Array
    public var data:Array
  }

  public class OpenSim
  {
    private var data:Array
    private var curr_itr:Array
    private var next_itr:Array

    public function OpenSim():
    {
      data = new Array()
      curr_itr = new Array()
      next_itr = new Array()
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

    }


    // for all functions returning a Number (that isn't data you're 
    // interested in), it will return 0 for normal operation, and 
    // a negative number if it failed (corresponding to the error it
    // encountered)

    // 'built-in' variables:
    //  - time
    //  - start_time
    //  - end_time
    //  - time_step
    //  - save_step (must be multiple of time step)


    // simple lookup table implementation
    private function lookup(table:Number, index:Number):Number
    {
      if (table.length == 0) 
        return 0

      var end:Number = table.length - 1

      // if the request is outside the min or max, then we return
      // the nearest element of the array
      if   (index < table[0][0])   return table[0][1]
      elif (index > table[end][0]) return table[end][1]

      for (i:Number = 0; i < table.length; i++)
      {
        x = table[i][0]
        y = table[i][1]

        if (index == x) return y
        if (index < x)
        {
          // slope = deltaY/deltaX
          slope = (y - table[i-1][1])/(x - table[i-1][0])
          return (index-table[i-1][0])*slope + table[i-1][1]
        }
      }
    }


    private function simulate(time_span:Number)
    {
      // this is where the math will go, simulating from the current time 
      // to current time + time_span
    }


    public function Start():Number
    {

    }


    // length of time to simulate, NOT     // of iterations.  if it doesn't match
    // a multiple of time_step, it will be rounded down to the nearest one.  
    // If it is greater than the time left in the simulation, it will simulate
    // to the end and not beyond
    public function Continue(time:Number):Number
    {

    }


    // it is safe to call this even if there is no simulation running. (to make
    // sure there is no active simulation, for example
    public function Finish():Number
    {

    }


    // will stop any running simulations, return to the start_time, and reset all
    // constants and lookup values to their initial values
    public function Reset():Number
    {

    }

    // works on constants any time, but is only defined for other variables when 
    // a model is bring simulated.
    public function getValue(var_name:String):Number
    {

    }


    // allows you to set the value of a constant before a simulation, or a 
    // lookup during a simulation.  If you set the value of a lookup during a 
    // simulation, it will add (or replace) the value you set at the index of the 
    // current time
    public function setValue(var_name:String, var_value:Number):Number
    {

    }


    // if needed, set sparse to False to 'fill in' the data.  If sparse==False
    // and the variable is a lookup table, then the SimData returned
    // will have index and time arrays of length save_step*(end_time-start_time)
    // and any data not on a save_step will be removed, so as to keep 
    // time-indexed data of equal length.  It is inappropriate to specify a sparse 
    // value for a lookup table not indexed with respect to time.
    public function getData(var_name:String, sparse:Boolean=True):SimData
    {

    }


    // used to set lookup data only.
    public function setData(var_name:String, var_data:SimData):Number
    {

    }


    public function getUnits(var_name:String):String
    {
        return "";
    }


    public function getDescription(var_name:String):String
    {
      return "";
    }
  }
}

