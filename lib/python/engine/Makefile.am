AM_CXXFLAGS = $(PYTHON_CPPFLAGS) $(GLIBMM_CFLAGS)
AM_LDFLAGS             = $(GLIBMM_LIBS)

INCLUDES = $(PYTHON_INCLUDES) -I$(top_srcdir) -I$(top_srcdir)/lib

simulator_wrap.cxx: simulator.i
	$(SWIG) $(SWIG_PYTHON_OPT) -I$(top_srcdir)/lib $(PYTHON_INCLUDES) simulator.i

EXTRA_DIST = simulator.i

CLEANFILES = simulator.py simulator.pyc \
	simulator_wrap.cxx _simulator.so

if HAVE_PYTHON
opensimdir = $(pkgpythondir)/engine
opensim_PYTHON = __init__.py simulator.py
opensim_LTLIBRARIES = _simulator.la

activitydir = $(top_srcdir)/activity/site-packages/opensim/engine
activity_PYTHON = $(opensim_PYTHON)
activity_SCRIPTS = libcopy

libcopy: 
	mkdir -p $(activitydir)
	cp .libs/_simulator.so.0.0.0 $(activitydir)/_simulator.so
endif

_simulator_la_SOURCES = simulator_wrap.cxx
_simulator_la_LDFLAGS = $(PYTHON_LDFLAGS) $(GLIBMM_LIBS) -module
_simulator_la_LIBADD  = $(top_builddir)/lib/libsimobjs.la


