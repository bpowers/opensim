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
  save_step = sim.get_variable ('time_savestep')
  save_step.props.equation = '4'
  
  # set sim-specific variables  
  sim.new_variable ('weird', '3.14*time')
  sim.new_variable ('lookup', '[(0,2),(50,40),(80,56)]')
  sim.new_variable ('test', 'lookup[time]')
  sim.new_variable ('another factor', 'weird*2')
  test = sim.new_variable ('blank', '')
  
  if test is not None:
    test.connect ('equation_changed', equation_checker)
    test.props.equation = 'lookup[time] * another_factor'

  print "%s's influences" % test.props.name
  for v in test.get_influences ():
    print "  %s" % v.props.name
  
  # set our output to be python, and call run.  since we haven't specified
  # an output file-name, it defaults to standard output.
  sim.props.output_type =  engine.emit_Python
  #sim.run()
  sim.output_debug_info()
  
  # connect our save_extra function to the 'saving' signal, set a 
  # file name for the model (which is different from an output file-name)
  # and save our model
  sim.connect ("saving", save_extra)
  sim.props.file_name = "test_sim.osm"
  sim.save ()


# run if we're the main module
if __name__ == '__main__':
  run ()
