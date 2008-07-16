package model
{
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

    private var data:Object
    private var original_data:Object
    private var i:int
    private var do_save:Boolean
    private var save_count:int
    private var save_iterations:int
    private var timestep:Number

    // ** insert here **


        if (do_save)
        {
          i++
          data["time"][j] = time + timestep
        }

        // determining whether or not to save results next iteration
        save_count = save_count + 1
        if (save_count >= save_iterations || time + timestep >= data["OS_end"][0])
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


    public function getI():Number
    {
      return i
    }


    public function getSaveCount():Number
    {
      return save_count
    }


    public function getSaveIterations():Number
    {
      return save_iterations
    }


    public function getTimestep():Number
    {
      return timestep
    }
    

    // resets everything
    public function Start():Number
    {
      data = original_data

      timestep = data["OS_timestep"][0]
      do_save = true
      
      i = 0
      save_count = 0
      save_iterations = data["OS_savestep"][0] / data["OS_timestep"][0]

      // do one round of simulation, so that we fill in data
      simulate(data["OS_timestep"][0])
      return 0
    }


    // length of time to simulate, NOT # of iterations.  if it doesn't match
    // a multiple of time_step, it will be rounded down to the nearest one.  
    // If it is greater than the time left in the simulation, it will simulate
    // to the end and not beyond
    public function Continue(continue_time:Number):Number
    {
      simulate(continue_time)

      return 0
    }


    // it is safe to call this even if there is no simulation running. (to make
    // sure there is no active simulation, for example
    public function Finish():Number
    {
      simulate(data["OS_end"][0] - data["OS_start"][0])
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
      if (!data.hasOwnProperty(var_name))
        return -1

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

      return 0
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
      var ret_val:SimData =  new SimData()
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

      for (var i:int = 0; i < table.data.length; i++)
      {
        var x:Number = table.index[i]
        var y:Number = table.data[i]

        if (index == x) return y
        if (index < x)
        {
          // slope = deltaY/deltaX
          var slope:Number = (y - table.data[i-1])/(x - table.index[i-1])
          return (index-table.index[i-1])*slope + table.data[i-1]
        }
      }
      
      // should have returned by here.
      return -1
    }
  }
}

