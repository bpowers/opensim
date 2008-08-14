AM_CXXFLAGS            = $(OPENSIM_XML2_CXXFLAGS)
AM_LDFLAGS             = $(OPENSIM_XML2_LDFLAGS)

INCLUDES               = -I$(top_builddir) -I$(top_srcdir)

MAINTAINERCLEANFILES   = Makefile.in

EXTRA_DIST             = Simulator.h Variable.h globals.h opensim.h
pkginclude_HEADERS     = opensim.h

SUBDIRS                = AST CodeGen IO

noinst_LTLIBRARIES     = libsimobjs.la
libsimobjs_la_SOURCES  = Simulator.cpp Variable.cpp
libsimobjs_la_LIBADD   = AST/libAST.la CodeGen/libCodeGen.la \
                         IO/libIO.la
libsimobjs_la_LDFLAGS  = -no-undefined

lib_LTLIBRARIES        = libsim.la
libsim_la_SOURCES      = libsim.cpp
libsim_la_LIBADD       = libsimobjs.la
