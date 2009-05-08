# sets compiler and linker flags necessary for LLVM JIT
AC_DEFUN([OPENSIM_LLVM_SETFLAGS],
[
    AC_CHECK_PROGS([LLVM], ['llvm-config'], [$MISSING llvm-config])

    AC_MSG_NOTICE([Getting LLVM compiler and linker flags.])
    AC_SUBST(OPENSIM_LLVM_CXXFLAGS, ['esyscmd(llvm-config --cppflags)'])
    AC_SUBST(OPENSIM_LLVM_LDFLAGS,  ['esyscmd(llvm-config --ldflags)'])
    AC_SUBST(OPEMSIM_LLVM_LIBS,     ['esyscmd(llvm-config --libs jit)'])
]
)
