TESTS = csv_parser_tests

check_PROGRAMS = csv_parser_tests

if COVERAGE
  COVERAGE_CFLAG = -ftest-coverage -fprofile-arcs
else
  COVERAGE_CFLAG =
endif

if DEBUG
  csv_parser_tests_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -g -O3 @CHECK_CFLAGS@ $(COVERAGE_CFLAG)
else
  csv_parser_tests_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -O3 @CHECK_CFLAGS@ $(COVERAGE_CFLAG)
endif

csv_parser_tests_LDFLAGS = $(TC_LDFLAGS)
csv_parser_tests_SOURCES = csv-parser-tests.c 
csv_parser_tests_LDADD = $(top_builddir)/src/lib/*.o $(AM_LDFLAGS) @CURSES_LIBS@  @CHECK_LIBS@