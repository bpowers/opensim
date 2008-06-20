AM_CXXFLAGS = $(PYTHON_CPPFLAGS)

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
endif

_simulator_la_SOURCES = simulator_wrap.cxx
_simulator_la_LDFLAGS = $(PYTHON_LDFLAGS) -module
_simulator_la_LIBADD  = $(top_builddir)/lib/libsimobjs.la

