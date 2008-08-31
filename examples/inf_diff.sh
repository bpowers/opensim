#!/bin/sh

opensim ../examples/infection.osm -t python > infection_c.py
opensim-d ../examples/infection.osm -t python > infection_g.py
diff -u infection_c.py infection_g.py > infection.diff
rm infection_*.py
#gedit infection.diff &
