
AM_CXXFLAGS             = $(OPENSIM_LLVM_CXXFLAGS)
AM_LDFLAGS              = $(OPENSIM_LLVM_LDFLAGS)

INCLUDES                = -I$(top_srcdir)/include

lib_LTLIBRARIES        = libsim.la
libsim_la_SOURCES      = c_runtime.c driver.cc lex.cc parse.cc runtime.cc token.cc
libsim_la_LIBADD       = $(OPENSIM_LLVM_LDFLAGS)

