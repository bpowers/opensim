#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([OpenSim], [0.4.7], [bugs@opensimproject.org])
AC_CONFIG_SRCDIR([src/opensim])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([config])

AM_INIT_AUTOMAKE([dist-bzip2 dist-zip])


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

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

  PKG_CHECK_MODULES(PYGOBJ, [pygobject-2.0])
  AC_SUBST(PYGOBJ_CFLAGS)
  AC_SUBST(PYGOBJ_LIBS)

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
AC_LIB_LTDL
AC_FUNC_STRTOD
AC_FUNC_ALLOCA
AC_CHECK_FUNCS([isascii])


PKG_PROG_PKG_CONFIG()

pkg_modules="glib-2.0 >= 2.10.0 gobject-2.0 >= 2.12.0"
PKG_CHECK_MODULES(GOBJ, [$pkg_modules])
AC_SUBST(GOBJ_CFLAGS)
AC_SUBST(GOBJ_LIBS)

# llvm is disabled for now.
#OPENSIM_LLVM_SETFLAGS
OPENSIM_XML2_SETFLAGS
AC_SUBST(LIBLTDL)


AM_GNU_GETTEXT_VERSION([0.16])
AM_GNU_GETTEXT([external])


AC_CONFIG_FILES([Makefile src/Makefile lib/Makefile lib/IO/Makefile \
                 lib/CodeGen/Makefile lib/AST/Makefile lib/python/Makefile \
                 lib/python/visuals/Makefile icons/Makefile  \
                 lib/python/visuals/widgets/Makefile po/Makefile.in \
                 lib/python/engine/Makefile share/Makefile activity/Makefile \
                 activity/activity/Makefile activity/icons/Makefile \
                 activity/lib/Makefile activity/site-packages/Makefile],
	[test -z "$CONFIG_HEADERS" || echo timestamp > stamp-h.in])


AC_OUTPUT
