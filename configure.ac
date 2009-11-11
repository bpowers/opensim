#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([OpenSim], [0.7.0], [bobby@laptop.org])
AC_COPYRIGHT([Copyright 2008-2009 Bobby Powers.])
AC_PREREQ(2.61)
AC_CONFIG_SRCDIR([lib/runtime.cc])
AC_CONFIG_HEADERS([include/opensim/config.h])
AC_CONFIG_AUX_DIR([autoconf])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([dist-bzip2 dist-zip])


# Checks for programs.
AC_PROG_CC_C99
AC_PROG_CXX

# Check if we're on the mac.
AC_CANONICAL_HOST
case $host_os in 
*darwin*) on_mac=true;; 
*)        on_mac=false;; 
esac

AM_CONDITIONAL([DARWIN], $on_mac)

if test x$on_mac == xtrue; then
  AM_CONDITIONAL([am__fastdepOBJC], true)
  AC_PROG_OBJC(['gcc -std=gnu99' gcc objcc objc])
else
  AM_CONDITIONAL([am__fastdepOBJC], false)
fi


AC_DISABLE_FAST_INSTALL
AC_DISABLE_SHARED
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
DOLT
SHAVE_INIT([autoconf], [enable])


AC_ARG_ENABLE([python],
              AC_HELP_STRING([--enable-python],
                             [python bindings and frontend]),,
              [enable_python=yes])



if test "x$enable_python" = "xyes"; then
  AM_PATH_PYTHON([2.5])

  AM_CHECK_PYTHON_HEADERS(,[AC_MSG_ERROR(could not find python headers)])

  have_python=yes
fi

AM_CONDITIONAL(HAVE_PYTHON, test x$have_python = xyes)

# Checks for programs.
AC_PROG_INSTALL

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

ACX_PTHREAD([])
OPENSIM_LLVM_SETFLAGS([])
GTEST_LIB_CHECK([1.3])


AC_CONFIG_FILES([
  autoconf/shave
  autoconf/shave-libtool
  opensim/engine/Makefile
  opensim/engine/io/Makefile
  opensim/engine/passes/Makefile
  opensim/visuals/widgets/Makefile
  opensim/visuals/Makefile
  opensim/Makefile
  icons/Makefile
  share/Makefile
  lib/Makefile
  src/Makefile
  tests/python/Makefile
  Makefile
])

AC_OUTPUT

# clean up silly leftover files
#`rm -f include/opensim/config.h.in*`
`rm -f include/opensim/stamp-h1`
`rm -rf autom4te.cache`
