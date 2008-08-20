INCLUDES                = -I$(top_builddir) -I$(top_builddir)/lib \
                          -I$(top_srcdir) -I$(top_srcdir)/lib

MAINTAINERCLEANFILES    = Makefile.in

EXTRA_DIST              = opensim opensim-gtk opensim-main.glade

bin_SCRIPTS             = opensim opensim-gtk
pkgdata_DATA            = opensim-main.glade

bin_PROGRAMS            = opensim-c opensim-d
opensim_c_SOURCES       = main.c
opensim_c_LDADD         = @LIBLTDL@ @LIBADD_DL@

opensim_d_SOURCES       = main-nonltdl.c
opensim_d_LDADD         = -lsim

