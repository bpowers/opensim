AM_LDFLAGS             = $(OPENSIM_XML2_LDFLAGS) $(GOBJ_LIBS)


if HAVE_PYTHON

CLEANFILES = model-simulator.defs model-simulator.c

model-simulator.defs:
	python2.5 $(top_srcdir)/codegen/h2def.py $(top_srcdir)/lib/model-simulator.h > model-simulator.defs

model-simulator.c: model-simulator.defs model-simulator.override
	python2.5 -d $(top_srcdir)/codegen/codegen.py \
	--prefix engine \
	--override model-simulator.override \
	model-simulator.defs > $@

opensimdir = $(pkgpythondir)
opensim_LTLIBRARIES = _engine.la

activitydir = $(top_srcdir)/activity/site-packages/opensim
activity_PYTHON = $(opensim_PYTHON)

_engine_la_SOURCES = model-simulator.c
_engine_la_CFLAGS  = $(PYGOBJ_CFLAGS) $(PYTHON_INCLUDES) $(GOBJ_CFLAGS)
_engine_la_LDFLAGS = $(PYTHON_LDFLAGS) $(PYGOBJ_LIBS) -module
_engine_la_LIBADD  = $(top_builddir)/lib/libsim.la

endif

