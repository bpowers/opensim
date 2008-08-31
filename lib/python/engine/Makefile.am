AM_LDFLAGS             = $(OPENSIM_XML2_LDFLAGS) $(GOBJ_LIBS)


if HAVE_PYTHON

CLEANFILES = opensim-simulator.defs opensim-wrap.c opensim-simulator.loT \
             opensim-variable.defs

opensim-simulator.defs:
	python2.5 $(top_srcdir)/codegen/h2def.py $(top_srcdir)/lib/opensim-simulator.h > opensim-simulator.defs

opensim-variable.defs:
	python2.5 $(top_srcdir)/codegen/h2def.py $(top_srcdir)/lib/opensim-variable.h > opensim-variable.defs

opensim-wrap.c: opensim-simulator.defs opensim-variable.defs opensim-simulator.override
	python2.5 -d $(top_srcdir)/codegen/codegen.py \
	--prefix engine \
	--override opensim-variable.override \
	--override opensim-simulator.override \
	opensim-simulator.defs \
	opensim-variable.defs > $@

opensimdir = $(pkgpythondir)
opensim_LTLIBRARIES = engine.la

activitydir = $(top_srcdir)/activity/site-packages/opensim
activity_PYTHON = $(opensim_PYTHON)

engine_la_SOURCES = opensim-wrap.c opensim-enginemodule.c
engine_la_CFLAGS  = $(PYGOBJ_CFLAGS) $(PYTHON_INCLUDES) $(GOBJ_CFLAGS)
engine_la_LDFLAGS = $(PYTHON_LDFLAGS) $(PYGOBJ_LIBS) -module
engine_la_LIBADD  = $(top_builddir)/lib/libsim.la

endif

