CC=gcc
CFLAGS=-I.
DEPS = 

build/%.o: src/%.c $(DEPS)
	mkdir -p build
	$(CC) -c -o $@ $< $(CFLAGS)

csv-viewer: build/csv-viewer.o
	$(CC) -o csv-viewer $^
all: csv-viewer

.DEFAULT_GOAL := all