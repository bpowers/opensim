
AM_CXXFLAGS             = $(OPENSIM_LLVM_CXXFLAGS) $(PTHREAD_CFLAGS)
AM_LDFLAGS              = $(OPENSIM_LLVM_LDFLAGS) \
                          $(OPEMSIM_LLVM_LIBS) $(PTHREAD_LIBS)

INCLUDES                = -I$(top_srcdir)/include -I$(top_builddir)

lib_LTLIBRARIES        = libsim.la
libsim_la_SOURCES      = c_runtime.c lex.cc parse.cc runtime.cc token.cc types.cc

