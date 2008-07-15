package model
{
  // basic class to pack time and data arrays together
  public class SimData
  {
    public var index:Array
    public var data:Array

    // convenience constructor
    // can either give 0, 1, or 2 arguments.  0: basic initialization.
    // 1: assumes list of tuples, and unpacks them.  2: assumes is 
    // (index, data)
    public function SimData(... args)
    {
      
      index = []
      data = []

      if (args.length == 1)
      {
        for (var i:int = 0; i< args[0].length; i++)
        {
          index.push(args[0][i][0]) 
          data.push(args[0][i][1])
        }
      }
      else if (args.length == 2)
      {
        index = args[0]
        data = args[1]
      }
    }
  }
}
