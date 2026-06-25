AUTOMAKE_OPTIONS = subdir-objects

## Common rules

if COVERAGE
  COVERAGE_CFLAG = -ftest-coverage -fprofile-arcs
  COVERAGE_LDFLAG = -lgcov --coverage
else
  COVERAGE_CFLAG =
  COVERAGE_LDFLAG =
endif

AM_CPPFLAGS = -I$(top_srcdir)/src/lib

if DEBUG
  AM_CFLAGS = -Wall -g -O3 $(COVERAGE_CFLAG)
else
  AM_CFLAGS = -Wall -O3 $(COVERAGE_CFLAG)
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
        navigation_tests \
        buffer_reader_tests \
        viewer_tests

check_PROGRAMS = csv_parser_tests \
                 commands_tests \
                 matrix_tests \
                 navigation_tests \
                 buffer_reader_tests \
                 viewer_tests

TEST_CFLAGS = -Wall -O3 @CHECK_CFLAGS@ $(COVERAGE_CFLAG)
TEST_LDFLAGS = $(COVERAGE_LDFLAG)
TEST_LDADD = $(top_builddir)/src/lib/libcsvviewer-core.a libmock.a $(AM_LDFLAGS) @CURSES_LIBS@ @CHECK_LIBS@

if DEBUG
  csv_parser_tests_CFLAGS = $(TEST_CFLAGS) -g
  commands_tests_CFLAGS = $(TEST_CFLAGS) -g
  matrix_tests_CFLAGS = $(TEST_CFLAGS) -g
  navigation_tests_CFLAGS = $(TEST_CFLAGS) -g
  buffer_reader_tests_CFLAGS = $(TEST_CFLAGS) -g
  viewer_tests_CFLAGS = $(TEST_CFLAGS) -g -DCSVI_TEST
else
  csv_parser_tests_CFLAGS = $(TEST_CFLAGS)
  commands_tests_CFLAGS = $(TEST_CFLAGS)
  matrix_tests_CFLAGS = $(TEST_CFLAGS)
  navigation_tests_CFLAGS = $(TEST_CFLAGS)
  buffer_reader_tests_CFLAGS = $(TEST_CFLAGS)
  viewer_tests_CFLAGS = $(TEST_CFLAGS) -DCSVI_TEST
endif

csv_parser_tests_LDFLAGS = $(TEST_LDFLAGS)
csv_parser_tests_SOURCES = csv-parser-tests.c
csv_parser_tests_LDADD = $(TEST_LDADD)

commands_tests_LDFLAGS = $(TEST_LDFLAGS)
commands_tests_SOURCES = commands-test.c
commands_tests_LDADD = $(TEST_LDADD)

matrix_tests_LDFLAGS = $(TEST_LDFLAGS)
matrix_tests_SOURCES = matrix-tests.c
matrix_tests_LDADD = $(TEST_LDADD)

navigation_tests_LDFLAGS = $(TEST_LDFLAGS)
navigation_tests_SOURCES = navigation-tests.c
navigation_tests_LDADD = $(TEST_LDADD)

buffer_reader_tests_LDFLAGS = $(TEST_LDFLAGS)
buffer_reader_tests_SOURCES = buffer-reader-tests.c
buffer_reader_tests_LDADD = $(TEST_LDADD)

viewer_tests_LDFLAGS = $(TEST_LDFLAGS)
viewer_tests_SOURCES = viewer-tests.c \
                       $(top_srcdir)/src/lib/app/viewer.c
viewer_tests_LDADD = $(TEST_LDADD)
