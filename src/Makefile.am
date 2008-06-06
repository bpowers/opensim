INCLUDES                = -I$(top_builddir) -I$(top_builddir)/lib \
                          -I$(top_srcdir) -I$(top_srcdir)/lib

MAINTAINERCLEANFILES    = Makefile.in

bin_PROGRAMS            = opensim
bin_SCRIPTS             = opensim-py
opensim_SOURCES         = main.c
opensim_LDADD           = @LIBLTDL@ @LIBADD_DL@
