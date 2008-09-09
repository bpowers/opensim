AM_CXXFLAGS            = $(GOBJ_CFLAGS)
AM_LDFLAGS             = $(GOBJ_LIBS)

EXTRA_DIST              = SimBuilder.h \
                          PythonPrintModule.h InterpreterModule.h \
                          AS3PrintModule.h

noinst_LTLIBRARIES      = libCodeGen.la
libCodeGen_la_SOURCES   = SimBuilder.cpp \
                          PythonPrintModule.cpp InterpreterModule.cpp
libCodeGen_la_LDFLAGS   = -no-undefined
