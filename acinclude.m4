AC_DEFUN(
	[OPENSIM_LLVM_SETFLAGS],
	[
		AC_CHECK_PROGS([LLVM], ['llvm-config'], [$MISSING llvm-config])

        	AC_MSG_NOTICE([Getting LLVM compiler and linker flags.])
		AC_SUBST(OPENSIM_LLVM_CXXFLAGS, ['esyscmd(llvm-config --cppflags)'])
                AC_SUBST(OPENSIM_LLVM_LDFLAGS,  ['esyscmd(llvm-config --ldflags)'])
		AC_SUBST(OPEMSIM_LLVM_LIBS, ['esyscmd(llvm-config --libs core jit native)'])
        ]
)

AC_DEFUN(
	[OPENSIM_XML2_SETFLAGS],
	[
		AC_CHECK_PROGS([XML2], ['xml2-config'], [$MISSING xml2-config])

        	AC_MSG_NOTICE([Getting libXML2 compiler and linker flags.])
		AC_SUBST(OPENSIM_XML2_CXXFLAGS, ['esyscmd(xml2-config --cflags)'])
                AC_SUBST(OPENSIM_XML2_LDFLAGS,  ['esyscmd(xml2-config --libs)'])
        ]
)



# AC_LIBLTDL_INSTALLABLE([DIRECTORY])
# -----------------------------------
# sets LIBLTDL to the link flags for the libltdl installable library and
# LTDLINCL to the include flags for the libltdl header and adds
# --enable-ltdl-install to the configure arguments.  Note that
# AC_CONFIG_SUBDIRS is not called here.  If DIRECTORY is not provided,
# and an installed libltdl is not found, it is assumed to be `libltdl'.
# LIBLTDL will be prefixed with '${top_builddir}/'# and LTDLINCL with
# '${top_srcdir}/' (note the single quotes!).  If your package is not
# flat and you're not using automake, define top_builddir and top_srcdir
# appropriately in the Makefiles.
# In the future, this macro may have to be called after AC_PROG_LIBTOOL.
AC_DEFUN([AC_LIBLTDL_INSTALLABLE],
[AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
  AC_CHECK_LIB(ltdl, lt_dlinit,
  [test x"$enable_ltdl_install" != xyes && enable_ltdl_install=no],
  [if test x"$enable_ltdl_install" = xno; then
     AC_MSG_WARN([libltdl not installed, but installation disabled])
   else
     enable_ltdl_install=yes
   fi
  ])
  if test x"$enable_ltdl_install" = x"yes"; then
    ac_configure_args="$ac_configure_args --enable-ltdl-install"
    LIBLTDL='${top_builddir}/'ifelse($#,1,[$1],['libltdl'])/libltdl.la
    LTDLINCL='-I${top_srcdir}/'ifelse($#,1,[$1],['libltdl'])
  else
    ac_configure_args="$ac_configure_args --enable-ltdl-install=no"
    LIBLTDL="-lltdl"
    LTDLINCL=
  fi
  # For backwards non-gettext consistent compatibility...
  INCLTDL="$LTDLINCL"
])# AC_LIBLTDL_INSTALLABLE

