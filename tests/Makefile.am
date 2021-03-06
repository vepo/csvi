## Common rules

if COVERAGE
  COVERAGE_CFLAG = -ftest-coverage -fprofile-arcs
  COVERAGE_LDFLAG = -lgcov --coverage
else
  COVERAGE_CFLAG =
  COVERAGE_LDFLAG = 
endif

## MOCK Rules

if DEBUG
  AM_CFLAGS =-I$(top_srcdir)/src/lib -Wall -g -O3 $(COVERAGE_CFLAG)
else
  AM_CFLAGS =-I$(top_srcdir)/src/lib -Wall $(COVERAGE_CFLAG)
endif

noinst_LIBRARIES = libmock.a
noinst_HEADERS = mock-token.h \
                 test-resources.h

libmock_a_SOURCES = \
                    $(libmock_a_HEADERS) \
                    mock-token.c \
                    test-resources.c

## Tests Rules

TESTS = csv_parser_tests \
        commands_tests \
        matrix_tests \
        navigation_tests

check_PROGRAMS = csv_parser_tests \
                 commands_tests \
                 matrix_tests \
                 navigation_tests

if DEBUG
  csv_parser_tests_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -g -O3 @CHECK_CFLAGS@ $(COVERAGE_CFLAG)
  commands_tests_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -g -O3 @CHECK_CFLAGS@ $(COVERAGE_CFLAG)
  matrix_tests_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -g -O3 @CHECK_CFLAGS@ $(COVERAGE_CFLAG)
  navigation_tests_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -g -O3 @CHECK_CFLAGS@ $(COVERAGE_CFLAG)
else
  csv_parser_tests_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -O3 @CHECK_CFLAGS@ $(COVERAGE_CFLAG)
  commands_tests_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -O3 @CHECK_CFLAGS@ $(COVERAGE_CFLAG)
  matrix_tests_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -O3 @CHECK_CFLAGS@ $(COVERAGE_CFLAG)
  navigation_tests_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -O3 @CHECK_CFLAGS@ $(COVERAGE_CFLAG)
endif

csv_parser_tests_LDFLAGS = $(COVERAGE_LDFLAG)
csv_parser_tests_SOURCES = csv-parser-tests.c
csv_parser_tests_LDADD = $(top_builddir)/src/lib/*.o libmock.a $(AM_LDFLAGS) @CURSES_LIBS@  @CHECK_LIBS@

commands_tests_LDFLAGS = $(COVERAGE_LDFLAG)
commands_tests_SOURCES = commands-test.c
commands_tests_LDADD = $(top_builddir)/src/lib/*.o libmock.a $(AM_LDFLAGS) @CURSES_LIBS@  @CHECK_LIBS@

matrix_tests_LDFLAGS = $(COVERAGE_LDFLAG)
matrix_tests_SOURCES = matrix-tests.c
matrix_tests_LDADD = $(top_builddir)/src/lib/*.o libmock.a $(AM_LDFLAGS) @CURSES_LIBS@  @CHECK_LIBS@

navigation_tests_LDFLAGS = $(COVERAGE_LDFLAG)
navigation_tests_SOURCES = navigation-tests.c
navigation_tests_LDADD = $(top_builddir)/src/lib/*.o libmock.a $(AM_LDFLAGS) @CURSES_LIBS@  @CHECK_LIBS@
