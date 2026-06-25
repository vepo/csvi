AUTOMAKE_OPTIONS = subdir-objects

if COVERAGE
  COVERAGE_CFLAG = -ftest-coverage -fprofile-arcs
else
  COVERAGE_CFLAG =
endif

AM_CPPFLAGS = -I$(top_srcdir)/src/lib

if DEBUG
  AM_CFLAGS = -Wall -g -O3 $(COVERAGE_CFLAG)
else
  AM_CFLAGS = -Wall $(COVERAGE_CFLAG)
endif

noinst_LIBRARIES = libcsvviewer-core.a libcsvviewer-base.a

noinst_HEADERS = \
  common/errors.h \
  common/helper.h \
  common/log.h \
  io/buffer-reader.h \
  io/csv-reader.h \
  layout/types.h \
  layout/matrix-config.h \
  ui/actions.h \
  ui/matrix-presentation.h \
  nav/navigation.h \
  cmd/commands.h \
  app/viewer.h

libcsvviewer_core_a_SOURCES = \
  common/log.c \
  io/buffer-reader.c \
  io/csv-reader.c \
  layout/matrix-config.c \
  ui/matrix-presentation.c \
  nav/navigation.c \
  cmd/commands.c

libcsvviewer_base_a_SOURCES = app/viewer.c
libcsvviewer_base_a_LIBADD = libcsvviewer-core.a
