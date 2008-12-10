#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([OpenSim], [0.5], [bugs@opensimproject.org])
AC_CONFIG_SRCDIR([src/opensim])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([dist-bzip2 dist-zip])


# Checks for programs.
AC_PROG_CC_C99

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

AC_LIBTOOL_WIN32_DLL
#AC_DISABLE_STATIC
AC_PROG_LIBTOOL
DOLT

have_python=no
AM_PATH_PYTHON([2.5])

if test -z "$PYTHON" ; then
  AC_MSG_WARN([Python not found])
else
  AM_CHECK_PYTHON_HEADERS(,[AC_MSG_ERROR(could not find Python headers)])

  #PKG_CHECK_MODULES(PYGOBJ, [pygobject-2.0])
  #AC_SUBST(PYGOBJ_CFLAGS)
  #AC_SUBST(PYGOBJ_LIBS)

  have_python=yes
fi



AM_CONDITIONAL(HAVE_PYTHON, test x$have_python = xyes) 

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_STRTOD
AC_FUNC_ALLOCA
AC_CHECK_FUNCS([isascii])


AC_SUBST(ABS_TOP, ['esyscmd(pwd)'])


# AM_GNU_GETTEXT_VERSION([0.16])
# AM_GNU_GETTEXT([external])


AC_CONFIG_FILES([
po/Makefile.in
opensim/ng/Makefile
opensim/visuals/widgets/Makefile
opensim/visuals/Makefile
opensim/Makefile
icons/Makefile
share/Makefile
src/Makefile
packaging/mac/Makefile
Makefile
])


AC_OUTPUT
