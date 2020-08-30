if COVERAGE
  COVERAGE_CFLAG = -ftest-coverage -fprofile-arcs
  COVERAGE_LDFLAG = -lgcov --coverage
else
  COVERAGE_CFLAG =
  COVERAGE_LDFLAG = 
endif

if DEBUG
  AM_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -g -O3 $(COVERAGE_CFLAG)
else
  AM_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -O3 $(COVERAGE_CFLAG)
endif


bin_PROGRAMS = csvi

csvi_SOURCES = main.c
csvi_LDFLAGS = $(COVERAGE_LDFLAG)
csvi_LDADD = $(top_builddir)/src/lib/libcsvviewer-base.a $(AM_LDFLAGS) @CURSES_LIBS@
