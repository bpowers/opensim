#!/usr/bin/env python

from opensim import engine

def save_extra(sim):
  print "\n<construct>"
  print "  oh so sweet metadata for %s" % sim.props.model_name
  print "</construct>"

def run():
  sim = engine.Simulator()
  
  sim.new_variable('OS_start', '0')
  sim.new_variable('OS_end', '100')
  sim.new_variable('OS_timestep', '.25')
  sim.new_variable('OS_savestep', '4')
  
  sim.new_variable('weird', '3.14*time')
  sim.new_variable('lookup', '[(0,2),(50,40),(80,56)]')
  sim.new_variable('test', 'lookup[time]')

  #print 'sim membs: ', list(engine.Simulator.__dict__)
  #print 'sim props: ', list(engine.Simulator.props)
  #print 'currently: ', sim.get_property('output-type')
  sim.props.output_type =  engine.emit_Python
  #sim.run()

  sim.connect("saving", save_extra)


  #sim.props.file_name = "test_sim.osm"
  sim.save()

if __name__ == '__main__':
  run()
