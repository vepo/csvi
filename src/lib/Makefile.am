if DEBUG
  AM_CFLAGS =-I$(top_srcdir)/src/lib -lncurses -Wall -g -O3
else
  AM_CFLAGS =-I$(top_srcdir)/src/lib -lncurses -Wall
endif

noinst_LIBRARIES = libcsvviewer-base.a
noinst_HEADERS = csv-reader.h \
		 buffer-reader.h \
     matrix-presentation.h

libcsvviewer_base_a_SOURCES = \
                              $(libcsvviewer_base_a_HEADERS) \
                              csv-reader.c \
                              buffer-reader.c \
                              matrix-presentation.c
