if COVERAGE
  COVERAGE_CFLAG = -ftest-coverage -fprofile-arcs
else
  COVERAGE_CFLAG = 
endif

if DEBUG
  AM_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -g -O3 $(COVERAGE_CFLAG)
else
  AM_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -O3 $(COVERAGE_CFLAG)
endif


bin_PROGRAMS = csvviewer

csvviewer_SOURCES = main.c
csvviewer_LDFLAGS = $(COVERAGE_LDFLAG)
csvviewer_LDADD = $(top_builddir)/src/lib/libcsvviewer-base.a $(AM_LDFLAGS) @CURSES_LIBS@
