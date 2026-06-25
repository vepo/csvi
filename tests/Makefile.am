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
        csv_writer_tests \
        cell_commands_tests \
        matrix_tests \
        navigation_tests \
        viewport_cache_tests \
        buffer_reader_tests \
        viewer_tests \
        search_tests

check_PROGRAMS = csv_parser_tests \
                 csv_writer_tests \
                 cell_commands_tests \
                 matrix_tests \
                 navigation_tests \
                 viewport_cache_tests \
                 buffer_reader_tests \
                 viewer_tests \
                 search_tests

test_cflags = -Wall -O3 @CHECK_CFLAGS@ $(COVERAGE_CFLAG)
test_ldflags = $(COVERAGE_LDFLAG)
test_ldadd = $(top_builddir)/src/lib/libcsvviewer-core.a libmock.a $(AM_LDFLAGS) @CURSES_LIBS@ @CHECK_LIBS@

CLEANFILES = output-csv-parser.log \
             navigation-output.log \
             matrix-test-output.log \
             viewport-cache-output.log

if DEBUG
  csv_parser_tests_CFLAGS = $(test_cflags) -g
  csv_writer_tests_CFLAGS = $(test_cflags) -g
  cell_commands_tests_CFLAGS = $(test_cflags) -g
  matrix_tests_CFLAGS = $(test_cflags) -g
  navigation_tests_CFLAGS = $(test_cflags) -g
  viewport_cache_tests_CFLAGS = $(test_cflags) -g
  buffer_reader_tests_CFLAGS = $(test_cflags) -g
  search_tests_CFLAGS = $(test_cflags) -g
  viewer_tests_CFLAGS = $(test_cflags) -g -DCSVI_TEST
else
  csv_parser_tests_CFLAGS = $(test_cflags)
  csv_writer_tests_CFLAGS = $(test_cflags)
  cell_commands_tests_CFLAGS = $(test_cflags)
  matrix_tests_CFLAGS = $(test_cflags)
  navigation_tests_CFLAGS = $(test_cflags)
  viewport_cache_tests_CFLAGS = $(test_cflags)
  buffer_reader_tests_CFLAGS = $(test_cflags)
  search_tests_CFLAGS = $(test_cflags)
  viewer_tests_CFLAGS = $(test_cflags) -DCSVI_TEST
endif

csv_parser_tests_LDFLAGS = $(test_ldflags)
csv_parser_tests_SOURCES = csv-parser-tests.c
csv_parser_tests_LDADD = $(test_ldadd)

csv_writer_tests_LDFLAGS = $(test_ldflags)
csv_writer_tests_SOURCES = csv-writer-tests.c
csv_writer_tests_LDADD = $(test_ldadd)

cell_commands_tests_LDFLAGS = $(test_ldflags)
cell_commands_tests_SOURCES = cell-commands-test.c
cell_commands_tests_LDADD = $(test_ldadd)

matrix_tests_LDFLAGS = $(test_ldflags)
matrix_tests_SOURCES = matrix-tests.c
matrix_tests_LDADD = $(test_ldadd)

navigation_tests_LDFLAGS = $(test_ldflags)
navigation_tests_SOURCES = navigation-tests.c
navigation_tests_LDADD = $(test_ldadd)

viewport_cache_tests_LDFLAGS = $(test_ldflags)
viewport_cache_tests_SOURCES = viewport-cache-tests.c
viewport_cache_tests_LDADD = $(test_ldadd)

buffer_reader_tests_LDFLAGS = $(test_ldflags)
buffer_reader_tests_SOURCES = buffer-reader-tests.c
buffer_reader_tests_LDADD = $(test_ldadd)

viewer_tests_LDFLAGS = $(test_ldflags)
viewer_tests_SOURCES = viewer-tests.c \
                       $(top_srcdir)/src/lib/app/viewer.c
viewer_tests_LDADD = $(test_ldadd)

search_tests_LDFLAGS = $(test_ldflags)
search_tests_SOURCES = search-tests.c
search_tests_LDADD = $(test_ldadd)
