ifeq ($(DEBUG), 1)
    CFLAGS=-ggdb
else
    CFLAGS=
endif

CC=gcc
CFLAGS:=$(CFLAGS) -std=c99
LDFLAGS=

BUILDDIR=build
SOURCEDIR=src
HEADERDIR=includes

SOURCES = $(wildcard $(SOURCEDIR)/*.c)
OBJECTS = $(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

$(BUILDDIR):
	[ -d $(BUILDDIR) ] || mkdir $(BUILDDIR)

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c $(BUILDDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(HEADERDIR) -I$(SOURCEDIR) -c -o $@ $< 

csv-viewer: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(HEADERDIR) -I$(SOURCEDIR) $(OBJECTS) -o $@

all: csv-viewer

clean:
	@rm -rf $(BUILDDIR) csv-viewer.exe

.DEFAULT_GOAL := all
