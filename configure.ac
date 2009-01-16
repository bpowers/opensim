#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([OpenSim], [0.5], [bobby@laptop.org])
AC_CONFIG_SRCDIR([src/opensim])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([dist-bzip2 dist-zip])
AM_MAINTAINER_MODE

# we really only need gcc if we're on the mac

# Check if we're on the mac.
AC_CANONICAL_HOST
case $host_os in 
*darwin*) on_mac=true;; 
*)        on_mac=false;; 
esac

AM_CONDITIONAL([DARWIN], $on_mac) 
AM_CONDITIONAL([am__fastdepOBJC], false)
mac_make

#if test x$on_mac == xtrue; then
#  mac_make=packaging/mac/Makefile
#  AM_CONDITIONAL([am__fastdepOBJC], true)

#  AC_PROG_CC_C99
#  AC_PROG_OBJC(['gcc -std=gnu99' gcc objcc objc])
#  AC_DISABLE_FAST_INSTALL
#  AC_DISABLE_STATIC
#  AC_PROG_LIBTOOL
#  DOLT
#fi




# Checks for programs.
AC_PROG_INSTALL

# automake macros
AM_PATH_PYTHON


AC_SUBST(ABS_TOP, ['esyscmd(pwd)'])


AC_CONFIG_FILES([
opensim/ng/Makefile
opensim/visuals/widgets/Makefile
opensim/visuals/Makefile
opensim/Makefile
icons/Makefile
share/Makefile
src/Makefile
$mac_make
Makefile
])


AC_OUTPUT
