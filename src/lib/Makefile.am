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
  io/csv-writer.h \
  layout/types.h \
  layout/matrix-config.h \
  layout/viewport-cache.h \
  ui/actions.h \
  ui/paint.h \
  ui/input-modes.h \
  ui/status-bar.h \
  ui/matrix-presentation.h \
  nav/navigation.h \
  cmd/commands.h \
  cmd/cell-commands.h \
  app/viewer.h \
  app/search.h

libcsvviewer_core_a_SOURCES = \
  common/log.c \
  io/buffer-reader.c \
  io/csv-reader.c \
  io/csv-writer.c \
  layout/matrix-config.c \
  layout/viewport-cache.c \
  ui/matrix-presentation.c \
  ui/input-modes.c \
  ui/status-bar.c \
  nav/navigation.c \
  cmd/commands.c \
  cmd/cell-commands.c \
  app/search.c

libcsvviewer_base_a_SOURCES = app/viewer.c
libcsvviewer_base_a_LIBADD = libcsvviewer-core.a
