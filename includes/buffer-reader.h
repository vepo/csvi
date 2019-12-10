#ifndef BUFFER_READER_H_
#define BUFFER_READER_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct buffer
{
    char *contents;
    size_t checkpoint;
    size_t length;
    struct buffer *nextBuffer;
} buffer;

typedef struct buffer_reader
{
    void *handler;
    buffer *currentBuffer;
} buffer_reader;

buffer_reader *buffer_reader_open(char *path);

bool buffer_reader_acquire(buffer_reader *reader);

char buffer_reader_current_char(buffer_reader *reader, size_t offset);

void buffer_reader_commit(buffer_reader *reader, size_t offset);

void buffer_reader_current_copy(buffer_reader *reader, size_t start, size_t end, char *dest);

void buffer_reader_release(buffer_reader *reader);

bool buffer_reader_has_data(buffer_reader *reader);

#endif