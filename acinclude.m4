AC_DEFUN(
  [OPENSIM_LLVM_SETFLAGS],
  [
    AC_CHECK_PROGS([LLVM], ['llvm-config'], [$MISSING llvm-config])

    AC_MSG_NOTICE([Getting LLVM compiler and linker flags.])
    AC_SUBST(OPENSIM_LLVM_CXXFLAGS, ['esyscmd(llvm-config --cppflags)'])
    AC_SUBST(OPENSIM_LLVM_LDFLAGS,  ['esyscmd(llvm-config --ldflags)'])
    AC_SUBST(OPEMSIM_LLVM_LIBS, 
             ['esyscmd(llvm-config --libs core jit native)'])
  ]
)

AC_DEFUN(
  [OPENSIM_XML2_SETFLAGS],
  [
    AC_CHECK_PROGS([XML2], ['xml2-config'], [$MISSING xml2-config])

    AC_MSG_NOTICE([Getting libXML2 compiler and linker flags.])
    AC_SUBST(OPENSIM_XML2_CFLAGS,  ['esyscmd(xml2-config --cflags)'])
    AC_SUBST(OPENSIM_XML2_LDFLAGS, ['esyscmd(xml2-config --libs)'])
  ]
)

dnl a macro to check for ability to create python extensions
dnl  AM_CHECK_PYTHON_HEADERS([ACTION-IF-POSSIBLE], [ACTION-IF-NOT-POSSIBLE])
dnl function also defines PYTHON_INCLUDES
AC_DEFUN([AM_CHECK_PYTHON_HEADERS],
[AC_REQUIRE([AM_PATH_PYTHON])
AC_MSG_CHECKING(for headers required to compile python extensions)
dnl deduce PYTHON_INCLUDES
py_prefix=`$PYTHON -c "import sys; print sys.prefix"`
py_exec_prefix=`$PYTHON -c "import sys; print sys.exec_prefix"`
PYTHON_INCLUDES="-I${py_prefix}/include/python${PYTHON_VERSION}"
if test "$py_prefix" != "$py_exec_prefix"; then
  PYTHON_INCLUDES="$PYTHON_INCLUDES -I${py_exec_prefix}/include/python${PYTHON_VERSION}"
fi
AC_SUBST(PYTHON_INCLUDES)
dnl check if the headers exist:
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $PYTHON_INCLUDES"
AC_TRY_CPP([#include <Python.h>],dnl
[AC_MSG_RESULT(found)
$1],dnl
[AC_MSG_RESULT(not found)
$2])
CPPFLAGS="$save_CPPFLAGS"
])


AC_DEFUN([AC_PYTHON_DEVEL],[
        #
        # Allow the use of a (user set) custom python version
        #
        AC_ARG_VAR([PYTHON_VERSION],[The installed Python
                version to use, for example '2.3'. This string
                will be appended to the Python interpreter
                canonical name.])

        AC_PATH_PROG([PYTHON],[python[$PYTHON_VERSION]])
        if test -z "$PYTHON"; then
           AC_MSG_ERROR([Cannot find python$PYTHON_VERSION in your system path])
           PYTHON_VERSION=""
        fi

        #
        # Check for a version of Python >= 2.1.0
        #
        AC_MSG_CHECKING([for a version of Python >= '2.1.0'])
        ac_supports_python_ver=`$PYTHON -c "import sys, string; \
                ver = string.split(sys.version)[[0]]; \
                print ver >= '2.1.0'"`
        if test "$ac_supports_python_ver" != "True"; then
                if test -z "$PYTHON_NOVERSIONCHECK"; then
                        AC_MSG_RESULT([no])
                        AC_MSG_FAILURE([
This version of the AC@&t@_PYTHON_DEVEL macro
doesn't work properly with versions of Python before
2.1.0. You may need to re-run configure, setting the
variables PYTHON_CPPFLAGS, PYTHON_LDFLAGS, PYTHON_SITE_PKG,
PYTHON_EXTRA_LIBS and PYTHON_EXTRA_LDFLAGS by hand.
Moreover, to disable this check, set PYTHON_NOVERSIONCHECK
to something else than an empty string.
])
                else
                        AC_MSG_RESULT([skip at user request])
                fi
        else
                AC_MSG_RESULT([yes])
        fi

        #
        # if the macro parameter ``version'' is set, honour it
        #
        if test -n "$1"; then
                AC_MSG_CHECKING([for a version of Python $1])
                ac_supports_python_ver=`$PYTHON -c "import sys, string; \
                        ver = string.split(sys.version)[[0]]; \
                        print ver $1"`
                if test "$ac_supports_python_ver" = "True"; then
                   AC_MSG_RESULT([yes])
                else
                        AC_MSG_RESULT([no])
                        AC_MSG_ERROR([this package requires Python $1.
If you have it installed, but it isn't the default Python
interpreter in your system path, please pass the PYTHON_VERSION
variable to configure. See ``configure --help'' for reference.
])
                        PYTHON_VERSION=""
                fi
        fi

        #
        # Check if you have distutils, else fail
        #
        AC_MSG_CHECKING([for the distutils Python package])
        ac_distutils_result=`$PYTHON -c "import distutils" 2>&1`
        if test -z "$ac_distutils_result"; then
                AC_MSG_RESULT([yes])
        else
                AC_MSG_RESULT([no])
                AC_MSG_ERROR([cannot import Python module "distutils".
Please check your Python installation. The error was:
$ac_distutils_result])
                PYTHON_VERSION=""
        fi

        #
        # Check for Python include path
        #
        AC_MSG_CHECKING([for Python include path])
        if test -z "$PYTHON_CPPFLAGS"; then
                python_path=`$PYTHON -c "import distutils.sysconfig; \
                        print distutils.sysconfig.get_python_inc();"`
                if test -n "${python_path}"; then
                        python_path="-I$python_path"
                fi
                PYTHON_CPPFLAGS=$python_path
        fi
        AC_MSG_RESULT([$PYTHON_CPPFLAGS])
        AC_SUBST([PYTHON_CPPFLAGS])

        #
        # Check for Python library path
        #
        AC_MSG_CHECKING([for Python library path])
        if test -z "$PYTHON_LDFLAGS"; then
                # (makes two attempts to ensure we've got a version number
                # from the interpreter)
                py_version=`$PYTHON -c "from distutils.sysconfig import *; \
                        from string import join; \
                        print join(get_config_vars('VERSION'))"`
                if test "$py_version" == "[None]"; then
                        if test -n "$PYTHON_VERSION"; then
                                py_version=$PYTHON_VERSION
                        else
                                py_version=`$PYTHON -c "import sys; \
                                        print sys.version[[:3]]"`
                        fi
                fi

                PYTHON_LDFLAGS=`$PYTHON -c "from distutils.sysconfig import *; \
                        from string import join; \
                        print '-L' + get_python_lib(0,1), \
                        '-lpython';"`$py_version
        fi
        AC_MSG_RESULT([$PYTHON_LDFLAGS])
        AC_SUBST([PYTHON_LDFLAGS])

        #
        # Check for site packages
        #
        AC_MSG_CHECKING([for Python site-packages path])
        if test -z "$PYTHON_SITE_PKG"; then
                PYTHON_SITE_PKG=`$PYTHON -c "import distutils.sysconfig; \
                        print distutils.sysconfig.get_python_lib(0,0);"`
        fi
        AC_MSG_RESULT([$PYTHON_SITE_PKG])
        AC_SUBST([PYTHON_SITE_PKG])

        #
        # libraries which must be linked in when embedding
        #
        AC_MSG_CHECKING(python extra libraries)
        if test -z "$PYTHON_EXTRA_LIBS"; then
           PYTHON_EXTRA_LIBS=`$PYTHON -c "import distutils.sysconfig; \
                conf = distutils.sysconfig.get_config_var; \
                print conf('LOCALMODLIBS'), conf('LIBS')"`
        fi
        AC_MSG_RESULT([$PYTHON_EXTRA_LIBS])
        AC_SUBST(PYTHON_EXTRA_LIBS)

        #
        # linking flags needed when embedding
        #
        AC_MSG_CHECKING(python extra linking flags)
        if test -z "$PYTHON_EXTRA_LDFLAGS"; then
                PYTHON_EXTRA_LDFLAGS=`$PYTHON -c "import distutils.sysconfig; \
                        conf = distutils.sysconfig.get_config_var; \
                        print conf('LINKFORSHARED')"`
        fi
        AC_MSG_RESULT([$PYTHON_EXTRA_LDFLAGS])
        AC_SUBST(PYTHON_EXTRA_LDFLAGS)

        #
        # final check to see if everything compiles alright
        #
        AC_MSG_CHECKING([consistency of all components of python development environment])
        AC_LANG_PUSH([C])
        # save current global flags
        LIBS="$ac_save_LIBS $PYTHON_LDFLAGS"
        CPPFLAGS="$ac_save_CPPFLAGS $PYTHON_CPPFLAGS"
        AC_TRY_LINK([
                #include <Python.h>
        ],[
                Py_Initialize();
        ],[pythonexists=yes],[pythonexists=no])

        AC_MSG_RESULT([$pythonexists])

        if test ! "$pythonexists" = "yes"; then
           AC_MSG_ERROR([
  Could not link test program to Python. Maybe the main Python library has been
  installed in some non-standard library path. If so, pass it to configure,
  via the LDFLAGS environment variable.
  Example: ./configure LDFLAGS="-L/usr/non-standard-path/python/lib"
  ============================================================================
   ERROR!
   You probably have to install the development version of the Python package
   for your distribution.  The exact name of this package varies among them.
  ============================================================================
           ])
          PYTHON_VERSION=""
        fi
        AC_LANG_POP
        # turn back to default flags
        CPPFLAGS="$ac_save_CPPFLAGS"
        LIBS="$ac_save_LIBS"

        #
        # all done!
        #
])




# pkg.m4 - Macros to locate and utilise pkg-config.            -*- Autoconf -*-
# 
# Copyright Â© 2004 Scott James Remnant <scott@netsplit.com>.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# As a special exception to the GNU General Public License, if you
# distribute this file as part of a program that contains a
# configuration script generated by Autoconf, you may include it under
# the same distribution terms that you use for the rest of that program.

# PKG_PROG_PKG_CONFIG([MIN-VERSION])
# ----------------------------------
AC_DEFUN([PKG_PROG_PKG_CONFIG],
[m4_pattern_forbid([^_?PKG_[A-Z_]+$])
m4_pattern_allow([^PKG_CONFIG(_PATH)?$])
AC_ARG_VAR([PKG_CONFIG], [path to pkg-config utility])dnl
if test "x$ac_cv_env_PKG_CONFIG_set" != "xset"; then
	AC_PATH_TOOL([PKG_CONFIG], [pkg-config])
fi
if test -n "$PKG_CONFIG"; then
	_pkg_min_version=m4_default([$1], [0.9.0])
	AC_MSG_CHECKING([pkg-config is at least version $_pkg_min_version])
	if $PKG_CONFIG --atleast-pkgconfig-version $_pkg_min_version; then
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
		PKG_CONFIG=""
	fi
		
fi[]dnl
])# PKG_PROG_PKG_CONFIG

# PKG_CHECK_EXISTS(MODULES, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# Check to see whether a particular set of modules exists.  Similar
# to PKG_CHECK_MODULES(), but does not set variables or print errors.
#
#
# Similar to PKG_CHECK_MODULES, make sure that the first instance of
# this or PKG_CHECK_MODULES is called, or make sure to call
# PKG_CHECK_EXISTS manually
# --------------------------------------------------------------
AC_DEFUN([PKG_CHECK_EXISTS],
[AC_REQUIRE([PKG_PROG_PKG_CONFIG])dnl
if test -n "$PKG_CONFIG" && \
    AC_RUN_LOG([$PKG_CONFIG --exists --print-errors "$1"]); then
  m4_ifval([$2], [$2], [:])
m4_ifvaln([$3], [else
  $3])dnl
fi])


# _PKG_CONFIG([VARIABLE], [COMMAND], [MODULES])
# ---------------------------------------------
m4_define([_PKG_CONFIG],
[if test -n "$$1"; then
    pkg_cv_[]$1="$$1"
 elif test -n "$PKG_CONFIG"; then
    PKG_CHECK_EXISTS([$3],
                     [pkg_cv_[]$1=`$PKG_CONFIG --[]$2 "$3" 2>/dev/null`],
		     [pkg_failed=yes])
 else
    pkg_failed=untried
fi[]dnl
])# _PKG_CONFIG

# _PKG_SHORT_ERRORS_SUPPORTED
# -----------------------------
AC_DEFUN([_PKG_SHORT_ERRORS_SUPPORTED],
[AC_REQUIRE([PKG_PROG_PKG_CONFIG])
if $PKG_CONFIG --atleast-pkgconfig-version 0.20; then
        _pkg_short_errors_supported=yes
else
        _pkg_short_errors_supported=no
fi[]dnl
])# _PKG_SHORT_ERRORS_SUPPORTED


# PKG_CHECK_MODULES(VARIABLE-PREFIX, MODULES, [ACTION-IF-FOUND],
# [ACTION-IF-NOT-FOUND])
#
#
# Note that if there is a possibility the first call to
# PKG_CHECK_MODULES might not happen, you should be sure to include an
# explicit call to PKG_PROG_PKG_CONFIG in your configure.ac
#
#
# --------------------------------------------------------------
AC_DEFUN([PKG_CHECK_MODULES],
[AC_REQUIRE([PKG_PROG_PKG_CONFIG])dnl
AC_ARG_VAR([$1][_CFLAGS], [C compiler flags for $1, overriding pkg-config])dnl
AC_ARG_VAR([$1][_LIBS], [linker flags for $1, overriding pkg-config])dnl

pkg_failed=no
AC_MSG_CHECKING([for $1])

_PKG_CONFIG([$1][_CFLAGS], [cflags], [$2])
_PKG_CONFIG([$1][_LIBS], [libs], [$2])

m4_define([_PKG_TEXT], [Alternatively, you may set the environment variables $1[]_CFLAGS
and $1[]_LIBS to avoid the need to call pkg-config.
See the pkg-config man page for more details.])

if test $pkg_failed = yes; then
        _PKG_SHORT_ERRORS_SUPPORTED
        if test $_pkg_short_errors_supported = yes; then
	        $1[]_PKG_ERRORS=`$PKG_CONFIG --short-errors --print-errors "$2" 2>&1`
        else 
	        $1[]_PKG_ERRORS=`$PKG_CONFIG --print-errors "$2" 2>&1`
        fi
	# Put the nasty error message in config.log where it belongs
	echo "$$1[]_PKG_ERRORS" >&AS_MESSAGE_LOG_FD

	ifelse([$4], , [AC_MSG_ERROR(dnl
[Package requirements ($2) were not met:

$$1_PKG_ERRORS

Consider adjusting the PKG_CONFIG_PATH environment variable if you
installed software in a non-standard prefix.

_PKG_TEXT
])],
		[AC_MSG_RESULT([no])
                $4])
elif test $pkg_failed = untried; then
	ifelse([$4], , [AC_MSG_FAILURE(dnl
[The pkg-config script could not be found or is too old.  Make sure it
is in your PATH or set the PKG_CONFIG environment variable to the full
path to pkg-config.

_PKG_TEXT

To get pkg-config, see <http://pkg-config.freedesktop.org/>.])],
		[$4])
else
	$1[]_CFLAGS=$pkg_cv_[]$1[]_CFLAGS
	$1[]_LIBS=$pkg_cv_[]$1[]_LIBS
        AC_MSG_RESULT([yes])
	ifelse([$3], , :, [$3])
fi[]dnl
])# PKG_CHECK_MODULES



dnl dolt, a replacement for libtool
dnl Copyright å© 2007-2008 Josh Triplett <josh@freedesktop.org>
dnl Copying and distribution of this file, with or without modification,
dnl are permitted in any medium without royalty provided the copyright
dnl notice and this notice are preserved.
dnl
dnl To use dolt, invoke the DOLT macro immediately after the libtool macros.
dnl Optionally, copy this file into acinclude.m4, to avoid the need to have it
dnl installed when running autoconf on your project.

AC_DEFUN([DOLT], [
AC_REQUIRE([AC_CANONICAL_HOST])
# dolt, a replacement for libtool
# Josh Triplett <josh@freedesktop.org>
AC_PATH_PROG(DOLT_BASH, bash)
AC_MSG_CHECKING([if dolt supports this host])
dolt_supported=yes
if test x$DOLT_BASH = x; then
    dolt_supported=no
fi
if test x$GCC != xyes; then
    dolt_supported=no
fi
case $host in
i?86-*-linux*|x86_64-*-linux*|powerpc-*-linux* \
|amd64-*-freebsd*|i?86-*-freebsd*|ia64-*-freebsd*)
    pic_options='-fPIC'
    ;;
i?86-apple-darwin*)
    pic_options='-fno-common'
    ;;
*)
    dolt_supported=no
    ;;
esac
if test x$dolt_supported = xno ; then
    AC_MSG_RESULT([no, falling back to libtool])
    LTCOMPILE='$(LIBTOOL) --tag=CC $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=compile $(COMPILE)'
    LTCXXCOMPILE='$(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=compile $(CXXCOMPILE)'
else
    AC_MSG_RESULT([yes, replacing libtool])

dnl Start writing out doltcompile.
    cat <<__DOLTCOMPILE__EOF__ >doltcompile
#!$DOLT_BASH
__DOLTCOMPILE__EOF__
    cat <<'__DOLTCOMPILE__EOF__' >>doltcompile
args=("$[]@")
for ((arg=0; arg<${#args@<:@@@:>@}; arg++)) ; do
    if test x"${args@<:@$arg@:>@}" = x-o ; then
        objarg=$((arg+1))
        break
    fi
done
if test x$objarg = x ; then
    echo 'Error: no -o on compiler command line' 1>&2
    exit 1
fi
lo="${args@<:@$objarg@:>@}"
obj="${lo%.lo}"
if test x"$lo" = x"$obj" ; then
    echo "Error: libtool object file name \"$lo\" does not end in .lo" 1>&2
    exit 1
fi
objbase="${obj##*/}"
__DOLTCOMPILE__EOF__

dnl Write out shared compilation code.
    if test x$enable_shared = xyes; then
        cat <<'__DOLTCOMPILE__EOF__' >>doltcompile
libobjdir="${obj%$objbase}.libs"
if test ! -d "$libobjdir" ; then
    mkdir_out="$(mkdir "$libobjdir" 2>&1)"
    mkdir_ret=$?
    if test "$mkdir_ret" -ne 0 && test ! -d "$libobjdir" ; then
	echo "$mkdir_out" 1>&2
        exit $mkdir_ret
    fi
fi
pic_object="$libobjdir/$objbase.o"
args@<:@$objarg@:>@="$pic_object"
__DOLTCOMPILE__EOF__
    cat <<__DOLTCOMPILE__EOF__ >>doltcompile
"\${args@<:@@@:>@}" $pic_options -DPIC || exit \$?
__DOLTCOMPILE__EOF__
    fi

dnl Write out static compilation code.
dnl Avoid duplicate compiler output if also building shared objects.
    if test x$enable_static = xyes; then
        cat <<'__DOLTCOMPILE__EOF__' >>doltcompile
non_pic_object="$obj.o"
args@<:@$objarg@:>@="$non_pic_object"
__DOLTCOMPILE__EOF__
        if test x$enable_shared = xyes; then
            cat <<'__DOLTCOMPILE__EOF__' >>doltcompile
"${args@<:@@@:>@}" >/dev/null 2>&1 || exit $?
__DOLTCOMPILE__EOF__
        else
            cat <<'__DOLTCOMPILE__EOF__' >>doltcompile
"${args@<:@@@:>@}" || exit $?
__DOLTCOMPILE__EOF__
        fi
    fi

dnl Write out the code to write the .lo file.
dnl The second line of the .lo file must match "^# Generated by .*libtool"
    cat <<'__DOLTCOMPILE__EOF__' >>doltcompile
{
echo "# $lo - a libtool object file"
echo "# Generated by doltcompile, not libtool"
__DOLTCOMPILE__EOF__

    if test x$enable_shared = xyes; then
        cat <<'__DOLTCOMPILE__EOF__' >>doltcompile
echo "pic_object='.libs/${objbase}.o'"
__DOLTCOMPILE__EOF__
    else
        cat <<'__DOLTCOMPILE__EOF__' >>doltcompile
echo pic_object=none
__DOLTCOMPILE__EOF__
    fi

    if test x$enable_static = xyes; then
        cat <<'__DOLTCOMPILE__EOF__' >>doltcompile
echo "non_pic_object='${objbase}.o'"
__DOLTCOMPILE__EOF__
    else
        cat <<'__DOLTCOMPILE__EOF__' >>doltcompile
echo non_pic_object=none
__DOLTCOMPILE__EOF__
    fi

    cat <<'__DOLTCOMPILE__EOF__' >>doltcompile
} > "$lo"
__DOLTCOMPILE__EOF__

dnl Done writing out doltcompile; substitute it for libtool compilation.
    chmod +x doltcompile
    LTCOMPILE='$(top_builddir)/doltcompile $(COMPILE)'
    LTCXXCOMPILE='$(top_builddir)/doltcompile $(CXXCOMPILE)'

dnl automake ignores LTCOMPILE and LTCXXCOMPILE when it has separate CFLAGS for
dnl a target, so write out a libtool wrapper to handle that case.
dnl Note that doltlibtool does not handle inferred tags or option arguments
dnl without '=', because automake does not use them.
    cat <<__DOLTLIBTOOL__EOF__ > doltlibtool
#!$DOLT_BASH
__DOLTLIBTOOL__EOF__
    cat <<'__DOLTLIBTOOL__EOF__' >>doltlibtool
top_builddir_slash="${0%%doltlibtool}"
: ${top_builddir_slash:=./}
args=()
modeok=false
tagok=false
for arg in "$[]@"; do
    case "$arg" in
        --mode=compile) modeok=true ;;
        --tag=CC|--tag=CXX) tagok=true ;;
        *) args+=("$arg")
    esac
done
if $modeok && $tagok ; then
    . ${top_builddir_slash}doltcompile "${args@<:@@@:>@}"
else
    exec ${top_builddir_slash}libtool "$[]@"
fi
__DOLTLIBTOOL__EOF__

dnl Done writing out doltlibtool; substitute it for libtool.
    chmod +x doltlibtool
    LIBTOOL='$(top_builddir)/doltlibtool'
fi
AC_SUBST(LTCOMPILE)
AC_SUBST(LTCXXCOMPILE)
# end dolt
])
