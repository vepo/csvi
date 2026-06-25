#ifndef CSVI_BUFFER_READER_H_
#define CSVI_BUFFER_READER_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct buffer
{
    char *contents;
    size_t checkpoint;
    size_t length;
    struct buffer *next_buffer;
} buffer_t;

typedef struct buffer_reader
{
    void *handler;
    buffer_t *current_buffer;
    bool end_reached;
} buffer_reader_t;

buffer_reader_t *buffer_reader_open(const char *path, char *errbuf, size_t errbuf_len);
bool buffer_reader_acquire(buffer_reader_t *reader);
char buffer_reader_current_char(buffer_reader_t *reader, size_t offset);
void buffer_reader_commit(buffer_reader_t *reader, size_t offset);
void buffer_reader_current_copy(buffer_reader_t *reader, size_t size, char *dest);
void buffer_reader_release(buffer_reader_t *reader);
bool buffer_reader_has_data(buffer_reader_t *reader);
size_t buffer_reader_available(buffer_reader_t *reader);

#endif
