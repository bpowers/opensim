#!/usr/bin/env python

from opensim import engine

def run():
  sim = engine.Simulator()
  
  sim.new_variable('OS_start', '0')
  sim.new_variable('OS_end', '100')
  sim.new_variable('OS_timestep', '.25')
  sim.new_variable('OS_savestep', '4')
  
  sim.new_variable('weird', '3.14*time')
  sim.new_variable('lookup', '[(0,2),(50,40),(80,56)]')
  sim.new_variable('test', 'lookup[time]')
  
  sim.run()


if __name__ == '__main__':
  run()
