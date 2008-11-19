AM_CFLAGS              = $(OPENSIM_XML2_CFLAGS) $(GOBJ_CFLAGS)
AM_CXXFLAGS            = $(OPENSIM_XML2_CFLAGS) $(GOBJ_CFLAGS)
AM_LDFLAGS             = $(OPENSIM_XML2_LDFLAGS) $(GOBJ_LIBS)

INCLUDES               = -I$(top_builddir) -I$(top_srcdir)

MAINTAINERCLEANFILES   = Makefile.in

EXTRA_DIST             = opensim-simulator.h opensim-variable.h globals.h opensim.h
pkginclude_HEADERS     = opensim.h opensim-simulator.h opensim-variable.h

SUBDIRS                = AST CodeGen IO

noinst_LTLIBRARIES     = libsimobjs.la
libsimobjs_la_SOURCES  = opensim-simulator.c opensim-variable.c
libsimobjs_la_LIBADD   = AST/libAST.la CodeGen/libCodeGen.la \
                         IO/libIO.la

libsimobjs_la_LDFLAGS  = -no-undefined @LIBINTL@

lib_LTLIBRARIES        = libsim.la
libsim_la_SOURCES      = libsim.cpp
libsim_la_LIBADD       = libsimobjs.la

