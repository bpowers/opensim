#!/usr/bin/env python

from opensim import simulator

filename = "../examples/new_rabbit.osm"

print "creating a simulator object"
sim = simulator.Simulator(filename)
sim.SetOutputFile("test.csv")
sim.Simulate()
