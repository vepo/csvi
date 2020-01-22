#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [victor.perticarrari@gmail.com])
AC_CONFIG_SRCDIR([src/bin/main.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
PKG_PROG_PKG_CONFIG

# Remember externally set CFLAGS
EXTERNAL_CFLAGS="$CFLAGS"

# Checks for programs.
AC_PROG_CC_C99
AC_PROG_CPP
AC_PROG_RANLIB

# Reset the externally set CFLAGS after calling AC_PROG*
CFLAGS="$EXTERNAL_CFLAGS"

# Use the C language and compiler for the following checks
AC_LANG([C])

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Add debug support
AC_ARG_ENABLE(debug,
  AS_HELP_STRING(
    [--enable-debug],
    [enable debugging, default: no]),
    [case "${enableval}" in
      yes) debug=true ;;
      no)  debug=false ;;
      *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
    esac],
    [debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")
AM_COND_IF(DEBUG,
    AC_DEFINE(DEBUG, 1, [Define to 0 if this is a release build]),
    AC_DEFINE(DEBUG, 0, [Define to 1 or higher if this is a debug build]))

AC_ARG_WITH([curses],
  [AS_HELP_STRING([--with-curses],
    [use curses for controlling terminal])],
  [],
  [with_curses=auto])

AS_CASE(["$with_curses"],
  [yes], [PKG_CHECK_MODULES([CURSES],
            [ncurses],
            [HAVE_CURSES=1])],
  [no], [HAVE_CURSES=0],
  [PKG_CHECK_MODULES([CURSES],
    [ncurses],
    [HAVE_CURSES=1],
    [HAVE_CURSES=0])])

AM_CONDITIONAL([USE_CURSES], [test "$HAVE_CURSES" -eq 1])
AS_IF([test "$HAVE_CURSES" -eq 1],
      [AC_DEFINE([HAVE_CURSES],
      [1],
      [Use curses])])

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
		 src/lib/Makefile
                 src/bin/Makefile])
AC_OUTPUT
