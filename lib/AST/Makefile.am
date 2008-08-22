AM_CXXFLAGS            = $(GOBJ_CFLAGS)
AM_LDFLAGS             = $(GOBJ_LIBS)

EXTRA_DIST              = ASTConsumer.h EulerAST.h General.h \
                          SimAST.h VariableAST.h LookupAST.h

noinst_LTLIBRARIES      = libAST.la
libAST_la_SOURCES       = EulerAST.cpp General.cpp SimAST.cpp \
                          VariableAST.cpp LookupAST.cpp
libAST_la_LDFLAGS       = -no-undefined
