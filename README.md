OpenSim - system dynamics for sugar and linux
=============================================

this is OpenSim, a System Dynamics modeling framework released under
the GPLv3.  it is being (in)actively developed by Bobby Powers.

to run, install pygoocanvas and sugar.  On Fedora 16 simply:

    $ sudo yum install pygoocanvas sugar

and then a simple:

    $ ./configure && make && sudo make install

should work, after which you can run the model editor:

    $ opensim-gtk

and you should get a visual system dynamics model editor.

beautiful screenshot
--------------------

![screenshot](/bpowers/opensim/raw/master/doc/screenshot.png)

current status
--------------

Frankly, I haven't worked on this in a while, but I wanted to get a
mirror of the code from
[dev.laptop.org](http://dev.laptop.org/git/activities/model/) up.

The model editor allows you create diagrams and edit equations (by
right clicking on elements).  There appears to be some bugginess in
there; after a while the diagram appeared to get 'stuck' for me.
Also, moving around stocks after loading the goats example seems to be
buggy.  I haven't worked on this in about 2 years, so I'm not sure
exactly what the issue is right now.

To run a model, save it in the editor, and then from the command line use opensim:

    $ opensim examples/models/goats.osm

Eventually you should be able to run the model through the UI, but
that isn't implemented yet.

more info
---------

for more information (although out of date), please visit
http://www.opensimproject.org

Share and enjoy!
