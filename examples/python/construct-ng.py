#!/usr/bin/env python

import logging
import opensim.ng as engine

LOG_FORMAT = '%(levelname)s: %(message)s'


# callback for when an equation changes
def equation_checker(var, old_equation):
  print("\nthe equation for '%s' changed." % var.props.name)
  print("  old equation: '%s'" % old_equation)
  print("  new equation: '%s'\n" % var.props.equation)


# callback function that is executed when sim.save () is called
def save_extra(sim, save_file):

  save_file.write("\n<more_info>\n")
  save_file.write("  oh so sweet metadata for %s\n" % sim.model_name)
  save_file.write("</more_info>\n")


def run():
  sim = engine.Simulator('sweet example')
  
  # set control variables
  save_step = sim.get_var('time_savestep')
  if save_step:
    save_step.props.equation = '4'
  
  # set sim-specific variables
  sim.new_var('weird', '3.14*time')
  sim.new_var('lookup', '[(0,2),(50,40),(80,56)]')
  sim.new_var('test', 'lookup[time]')
  sim.new_var('another factor', 'weird*2')
  test = sim.new_var('other test')
  # testing both case insensitivity and lack of spaces
  sim.new_var('q', 'inTEg(test,3)')

  if test is not None:

    test.connect('equation_changed', equation_checker)
    test.props.equation = 'lookup[time] * another_factor'

    print("%s's influences:" % test.props.name)
    for v in test.get_influences():
      print('  %s' % v.props.name)

  variables = sim.get_vars()
  if variables:
    print("\nmodel's variables:")

    print('')
    for v in variables:
      print("  %s (%s)" % (v.props.name, engine.name_for_type(v.props.type)))


  # skip a line
  print('')
  # set our output to be python, and call run.  since we haven't specified
  # an output file-name, it defaults to standard output.
  #sim.props.output_type =  engine.EMIT_PYTHON
  sim.run()
  #sim.output_debug_info()

  # connect our save_extra function to the 'saving' signal, set a 
  # file name for the model (which is different from an output file-name)
  # and save our model
  sim.connect("saving", save_extra)
  sim.props.file_name = "test_sim.osm"
  #sim.save()


# run if we're the main module
if __name__ == '__main__':
  logging.basicConfig(level=logging.DEBUG, format=LOG_FORMAT)
  run()

