AM_LDFLAGS = $(OPENSIM_XML2_LDFLAGS) $(GOBJ_LIBS)
EXTRA_DIST = opensim-simulator.override

if HAVE_PYTHON

CLEANFILES          = opensim-simulator.defs opensim-sim.c \
                      opensim-simulator.loT \
                      opensim-variable.defs opensim-var.c

opensim-simulator.defs:
	python2.5 $(top_srcdir)/codegen/h2def.py \
                $(top_srcdir)/lib/opensim-variable.h \
                > opensim-simulator.defs

	python2.5 $(top_srcdir)/codegen/h2def.py \
                $(top_srcdir)/lib/opensim-simulator.h \
                >> opensim-simulator.defs

opensim-sim.c: opensim-simulator.defs opensim-simulator.override
	python2.5 -d $(top_srcdir)/codegen/codegen.py \
	--prefix engine_simulator \
	--override \
            $(top_srcdir)/lib/python/engine/opensim-simulator.override \
	opensim-simulator.defs > $@


opensimdir          = $(pkgpythondir)
opensim_LTLIBRARIES = engine.la

activitydir         = $(top_srcdir)/activity/site-packages/opensim
activity_PYTHON     = $(opensim_PYTHON)

engine_la_SOURCES   = opensim-sim.c opensim-enginemodule.c
engine_la_CFLAGS    = $(PYGOBJ_CFLAGS) $(PYTHON_INCLUDES) $(GOBJ_CFLAGS)
engine_la_LDFLAGS   = $(PYTHON_LDFLAGS) $(PYGOBJ_LIBS) -module \
                      -headerpad_max_install_names
engine_la_LIBADD    = $(top_builddir)/lib/libsim.la

endif
