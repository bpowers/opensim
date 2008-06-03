#!/usr/bin/env python

import simulator

filename = "new_rabbit.osm"

print "creating a simulator object"
sim = simulator.Simulator(filename)
sim.SetOutputFile("test.csv")
sim.Simulate()
