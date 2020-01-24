TESTS = run_all_tests

check_PROGRAMS = run_all_tests

if DEBUG
  run_all_tests_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -g -O3 @CHECK_CFLAGS@
else
  run_all_tests_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -O3 @CHECK_CFLAGS@
endif

run_all_tests_SOURCES = run_all_tests.c
run_all_tests_LDADD = $(top_builddir)/src/lib/libcsvviewer-base.a $(AM_LDFLAGS) @CURSES_LIBS@  @CHECK_LIBS@