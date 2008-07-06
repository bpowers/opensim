
# basic class to pack time and data arrays together
public class SimData
{
  public var index:Array
  public var data:Array
}

# for all functions returning a Number (that isn't data you're 
# interested in), it will return 0 for normal operation, and 
# a negative number if it failed (corresponding to the error it
# encountered)

# 'built-in' variables:
#  - time
#  - start_time
#  - end_time
#  - time_step
#  - save_step (must be multiple of time step)


public function simStart():Number

public function simContinue(time:Number):Number
# length of time to simulate, NOT # of iterations.  if it doesn't match
# a multiple of time_step, it will be rounded down to the nearest one.  
# If it is greater than the time left in the simulation, it will simulate
# to the end and not beyond

public function simFinish():Number
# it is safe to call this even if there is no simulation running. (to make
# sure there is no active simulation, for example

public function simReset():Number
# will stop any running simulations, return to the start_time, and reset all
# constants and lookup values to their initial values

public function getValue(var_name:String):Number
# works on constants any time, but is only defined for other variables when 
# a model is bring simulated.

public function setValue(var_name:String, var_value:Number):Number
# allows you to set the value of a constant before a simulation, or a 
# lookup during a simulation.  If you set the value of a lookup during a 
# simulation, it will add (or replace) the value you set at the index of the 
# current time


public function getData(var_name:String, sparse:Boolean=True):SimData
# if needed, set sparse to False to 'fill in' the data.  If sparse==False
# and the variable is a lookup table, then the SimData returned
# will have index and time arrays of length save_step*(end_time-start_time)
# and any data not on a save_step will be removed, so as to keep 
# time-indexed data of equal length.  It is inappropriate to specify a sparse 
# value for a lookup table not indexed with respect to time.


public function setData(var_name:String, var_data:SimData):Number
# used to set lookup data only.

public function getUnits(var_name:String):String
public function getDescription(var_name:String):String
