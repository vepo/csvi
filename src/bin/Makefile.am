if DEBUG
  AM_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -g -O3
else
  AM_CFLAGS =-I$(top_srcdir)/src/bin -I$(top_srcdir)/src/lib -Wall -O3
endif

bin_PROGRAMS = csvviewer
csvviewer_SOURCES = main.c
csvviewer_LDADD = $(top_builddir)/src/lib/libcsvviewer-base.a $(AM_LDFLAGS)
