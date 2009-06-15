
AM_CXXFLAGS             = $(OPENSIM_LLVM_CXXFLAGS) $(PTHREAD_CFLAGS)
AM_LDFLAGS              = $(OPENSIM_LLVM_LDFLAGS) \
                          $(OPEMSIM_LLVM_LIBS) $(PTHREAD_LIBS)

INCLUDES                = -I$(top_srcdir)/include -I$(top_builddir)/include

MAINTAINERCLEANFILES    = Makefile.in

EXTRA_DIST              = opensim opensim-gtk

bin_SCRIPTS             = opensim opensim-gtk
bin_PROGRAMS            = opensim2

opensim2_SOURCES       = driver.cc
opensim2_LDADD         = $(top_builddir)/lib/libsim.la

