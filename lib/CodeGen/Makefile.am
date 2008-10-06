AM_CFLAGS               = $(GOBJ_CFLAGS) 
AM_CXXFLAGS             = $(AM_CFLAGS)
AM_LDFLAGS              = $(GOBJ_LIBS)

EXTRA_DIST              = opensim-generator.h SimBuilder.h \
                          PythonPrintModule.h InterpreterModule.h \
                          AS3PrintModule.h

noinst_LTLIBRARIES      = libCodeGen.la
libCodeGen_la_SOURCES   = opensim-generator.c SimBuilder.cpp \
                          PythonPrintModule.cpp InterpreterModule.cpp
libCodeGen_la_LDFLAGS   = -no-undefined
