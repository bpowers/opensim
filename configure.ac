#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([OpenSim], [0.2.8], [bugs@opensimproject.org])
AC_CONFIG_SRCDIR([src/opensim])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([config])

AM_INIT_AUTOMAKE([dist-bzip2 dist-zip])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AC_GNU_SOURCE
AM_GNU_GETTEXT

AC_DISABLE_FAST_INSTALL

AC_LIBTOOL_DLOPEN
AC_LIBLTDL_INSTALLABLE
AC_LIBTOOL_WIN32_DLL
AC_DISABLE_STATIC
AC_PROG_LIBTOOL


dnl pkg-config
CS="C#"
have_csharp=no

dnl C# compiler
AC_PATH_PROG(CSC, csc, no)
AC_PATH_PROG(MCS, gmcs, no)
AC_PATH_PROG(MONO, mono, no)

if test "x$CSC" = "xno" -a "x$MCS" = "xno"  ; then
	AC_MSG_WARN([No $CS compiler found, skipping $CS SWIG bindings])
else
	if test "x$MCS" = "xno" ; then
		MCS=$CSC
	fi

	if test "x$MONO" = "xno"; then
		AC_MSG_WARN([No mono runtime found, skipping $CS SWIG bindings])
	else
		AC_SUBST(MCS)
		have_csharp=yes
    AC_MSG_NOTICE([Found mono, enabling $CS bindings])
	fi
fi

AM_CONDITIONAL(HAVE_CSHARP, test x$have_csharp = xyes) 

have_python=no
AC_PYTHON_DEVEL([>='2.5'])

if test -z "$PYTHON" ; then
  AC_MSG_WARN([Python not found])
else
  AM_PATH_PYTHON([2.5])
  AC_PROG_SWIG(1.3.31)
  if test -z "$SWIG_LIB" ; then
    AC_MSG_WARN([SWIG not found, Python wrappers won't be built])
  else 
    SWIG_ENABLE_CXX
    SWIG_MULTI_MODULE_SUPPORT
    SWIG_PYTHON
    have_python=yes
  fi
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


OPENSIM_GTHREADS
# llvm is disabled for now.
#OPENSIM_LLVM_SETFLAGS
OPENSIM_XML2_SETFLAGS
AC_SUBST(LIBLTDL)

AC_ARG_ENABLE(debug,
[  --enable-debug    Turn on debugging],
[case "${enableval}" in
  yes) debug=true ;;
  no)  debug=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
esac],[debug=false])
AM_CONDITIONAL(DEBUG, test x$debug = xtrue)


AC_CONFIG_FILES([Makefile src/Makefile lib/Makefile lib/IO/Makefile  intl/Makefile po/Makefile.in\
                 lib/CodeGen/Makefile lib/AST/Makefile lib/python/Makefile \
                 lib/python/visuals/Makefile icons/Makefile  \
                 lib/python/visuals/widgets/Makefile \
                 lib/python/engine/Makefile share/Makefile activity/Makefile \
                 activity/activity/Makefile activity/icons/Makefile \
                 activity/lib/Makefile activity/site-packages/Makefile],
	[test -z "$CONFIG_HEADERS" || echo timestamp > stamp-h.in])


AC_OUTPUT
