#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([OpenSim], [0.2.3], [bugs@opensimproject.org])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([config])

AM_INIT_AUTOMAKE([dist-bzip2 dist-zip])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AC_DISABLE_FAST_INSTALL

AC_LIBTOOL_DLOPEN
AC_LIBLTDL_INSTALLABLE
AC_LIBTOOL_WIN32_DLL
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

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


# llvm is disabled for now.
#OPENSIM_LLVM_SETFLAGS
OPENSIM_XML2_SETFLAGS
AC_SUBST(LIBLTDL)



AC_CONFIG_FILES([Makefile src/Makefile lib/Makefile lib/IO/Makefile lib/CodeGen/Makefile lib/AST/Makefile],
	[test -z "$CONFIG_HEADERS" || echo timestamp > stamp-h.in])


AC_OUTPUT
