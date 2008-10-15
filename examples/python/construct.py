#!/usr/bin/env python

from opensim import engine


# callback for when an equation changes
def equation_checker (var, old_equation):
  print "\nthe equation for '%s' changed." % var.props.name
  print "  old equation: '%s'" % old_equation
  print "  new equation: '%s'\n" % var.props.equation

# callback function that is executed when sim.save () is called
def save_extra (sim, save_file_pointer):
  save_file = engine.get_file (sim, save_file_pointer)
  
  save_file.write ("\n<more_info>\n")
  save_file.write ("  oh so sweet metadata for %s\n" % sim.props.model_name)
  save_file.write ("</more_info>\n")


def run ():
  sim = engine.Simulator ()
  
  # set control variables
  sim.new_variable ('OS_start', '0')
  sim.new_variable ('OS_end', '100')
  sim.new_variable ('OS_timestep', '.25')
  sim.new_variable ('OS_savestep', '4')

  # set sim-specific variables  
  sim.new_variable ('weird', '3.14*time')
  sim.new_variable ('lookup', '[(0,2),(50,40),(80,56)]')
  sim.new_variable ('test', 'lookup[time]')

  test = sim.get_variable ('test')
  if test is not None:
    test.connect ("equation_changed", equation_checker)
    test.props.equation = 'lookup[time] * weird'

  # set our output to be python, and call run.  since we haven't specified
  # an output file-name, it defaults to standard output.
  sim.props.output_type =  engine.emit_Python
  #sim.run()
  
  # connect our save_extra function to the 'saving' signal, set a 
  # file name for the model (which is different from an output file-name)
  # and save our model
  sim.connect ("saving", save_extra)
  sim.props.file_name = "test_sim.osm"
  #sim.save ()


# run if we're the main module
if __name__ == '__main__':
  run ()
