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

bin_PROGRAMS = csvi

csvi_SOURCES = main.c
csvi_LDFLAGS = $(COVERAGE_LDFLAG)
csvi_LDADD = $(top_builddir)/src/lib/libcsvviewer-base.a \
             $(top_builddir)/src/lib/libcsvviewer-core.a \
             $(AM_LDFLAGS) @CURSES_LIBS@
