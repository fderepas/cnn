
AC_INIT([cnn],[0.1])
dnl override site config, not standard practice
: ${CFLAGS=""}

AC_DEFINE_UNQUOTED([CFG_DEFAULT_PREFIX],["$HOME/cnn"],[default prefix])
dnl AC_PREFIX_DEFAULT(~/cnn)
AM_INIT_AUTOMAKE([subdir-objects])
dnl AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h:config.in])
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR
AC_DEFINE_UNQUOTED([CFG_PREFIX],["$prefix"],[prefix path])
AC_DEFINE_UNQUOTED([CFG_CC],["$CC"],[compiler name])
AC_DEFINE_UNQUOTED([CFG_CFLAGS],[" -Wall -g "],[c compilation flag])
AC_DEFINE_UNQUOTED([CFG_DATAROOTDIR],["$prefix/share"],['share' data])
AC_DEFINE_UNQUOTED([CFG_BINDIR],["${bindir}"],[install dir for binary])
AC_DEFINE_UNQUOTED([CFG_UNAME_IS_LINUX],[`cat uname_is_linux`],[unix name of current platform])
AC_DEFINE_UNQUOTED([CFG_GIT_REPO],["`cat .git/config | grep url | sed -e 's/^\turl = //'`"],[git repo])
AC_DEFINE_UNQUOTED([CFG_UNAME],["`uname`"],[unix name of current platform])
AC_DEFINE_UNQUOTED([CFG_UNAME_A],["`uname -a | tr '"' .`"],[uname with '-a' flag for all options])
AC_DEFINE_UNQUOTED([CFG_CFGDATE],["`date`"],[configuration date])
AC_DEFINE_UNQUOTED([CFG_GIT_HASH],["`git rev-parse --short HEAD`"],[git shortened hash])
AC_DEFINE_UNQUOTED([CFG_GIT_FHASH],["`git rev-parse HEAD`"],[git full hash])
git log --pretty=format:'%h' -n 1
AC_CONFIG_FILES([
Makefile
src/Makefile
src/inverter/Makefile
src/edge/Makefile
src/digits/Makefile
src/cnn/Makefile
])
AC_OUTPUT
