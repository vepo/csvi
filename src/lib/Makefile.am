if DEBUG
  AM_CFLAGS =-I$(top_srcdir)/src/lib -Wall -g -O3
else
  AM_CFLAGS =-I$(top_srcdir)/src/lib -Wall
endif

noinst_LIBRARIES = libcsvviewer-base.a
noinst_HEADERS = csv-reader.h \
		 buffer-reader.h

libcsvviewer_base_a_SOURCES = \
                              $(libcsvviewer_base_a_HEADERS) \
                              csv-reader.c \
                              buffer-reader.c
