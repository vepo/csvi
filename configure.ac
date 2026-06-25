#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([csv-viewer], [0.0.1], [@vepo])
AC_CONFIG_SRCDIR([src/bin/main.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign subdir-objects dist-bzip2 silent-rules])
PKG_PROG_PKG_CONFIG

# Preserve externally set CFLAGS across AC_PROG_CC.
saved_CFLAGS="$CFLAGS"

AC_PROG_CC
LT_INIT

CFLAGS="$saved_CFLAGS"
AC_LANG([C])

PKG_CHECK_MODULES([CHECK], [check])
PKG_CHECK_MODULES([CURSES], [ncurses])

AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h])
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug], [enable debug symbols (default: no)]),
  [AS_CASE([$enableval],
    [yes], [debug=true],
    [no],  [debug=false],
    [AC_MSG_ERROR([invalid value: $enableval])])],
  [debug=false])
AM_CONDITIONAL([DEBUG], [test x"$debug" = x"true"])
AS_IF([test x"$debug" = x"true"],
  [AC_DEFINE([DEBUG], [1], [Define to 1 for debug builds])],
  [AC_DEFINE([DEBUG], [0], [Define to 0 for release builds])])

AC_ARG_ENABLE([coverage],
  AS_HELP_STRING([--enable-coverage], [enable gcov coverage (default: no)]),
  [AS_CASE([$enableval],
    [yes], [coverage=true],
    [no],  [coverage=false],
    [AC_MSG_ERROR([invalid value: $enableval])])],
  [coverage=false])
AM_CONDITIONAL([COVERAGE], [test x"$coverage" = x"true"])
AS_IF([test x"$coverage" = x"true"],
  [AC_DEFINE([COVERAGE], [1], [Define to 1 for coverage builds])],
  [AC_DEFINE([COVERAGE], [0], [Define to 0 when coverage is disabled])])

AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
                 src/lib/Makefile
                 src/bin/Makefile
                 tests/Makefile])
AC_OUTPUT
