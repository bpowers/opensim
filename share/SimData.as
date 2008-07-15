package
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
        for each (entry in args[0])
        {
          index.push(entry[0]) 
          data.push(entry[1])
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
