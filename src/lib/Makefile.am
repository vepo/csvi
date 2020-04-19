if COVERAGE
  COVERAGE_CFLAG = -ftest-coverage -fprofile-arcs
else
  COVERAGE_CFLAG =
endif

if DEBUG
  AM_CFLAGS =-I$(top_srcdir)/src/lib -Wall -g -O3 $(COVERAGE_CFLAG)
else
  AM_CFLAGS =-I$(top_srcdir)/src/lib -Wall $(COVERAGE_CFLAG)
endif

noinst_LIBRARIES = libcsvviewer-base.a
noinst_HEADERS = actions.h \
                 logger.h \
                 helper.h \
                 csv-reader.h \
                 buffer-reader.h \
                 matrix-config.h \
                 matrix-presentation.h

libcsvviewer_base_a_SOURCES = \
                              $(libcsvviewer_base_a_HEADERS) \
                              logger.c \
                              csv-reader.c \
                              buffer-reader.c \
                              matrix-config.c \
                              matrix-presentation.c
