#include "io/buffer-reader.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

buffer_reader_t *buffer_reader_open(const char *path, char *errbuf, size_t errbuf_len)
{
    buffer_reader_t *reader = (buffer_reader_t *)malloc(sizeof(buffer_reader_t));
    if (!reader)
    {
        if (errbuf && errbuf_len > 0)
        {
            snprintf(errbuf, errbuf_len, "out of memory");
        }
        return NULL;
    }

    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        if (errbuf && errbuf_len > 0)
        {
            snprintf(errbuf, errbuf_len, "could not open '%s': %s", path, strerror(errno));
        }
        free(reader);
        return NULL;
    }

    reader->handler = fp;
    reader->current_buffer = NULL;
    reader->end_reached = false;
    return reader;
}

bool buffer_reader_acquire(buffer_reader_t *reader)
{
    if (!reader->end_reached)
    {
        char *data = (char *)malloc(sizeof(char) * BUFFER_SIZE);
        size_t read_data = fread(data, sizeof(char), BUFFER_SIZE, reader->handler);
        if (read_data)
        {
            buffer_t *next_buffer = NULL;
            if (!reader->current_buffer)
            {
                next_buffer = reader->current_buffer = (buffer_t *)malloc(sizeof(buffer_t));
            }
            else
            {
                buffer_t *curr_buffer = reader->current_buffer;
                while (curr_buffer->next_buffer)
                {
                    curr_buffer = curr_buffer->next_buffer;
                }
                next_buffer = curr_buffer->next_buffer = (buffer_t *)malloc(sizeof(buffer_t));
            }
            next_buffer->contents = data;
            next_buffer->checkpoint = 0;
            next_buffer->next_buffer = NULL;
            next_buffer->length = read_data;
            return true;
        }

        free(data);
        reader->end_reached = true;
    }
    return false;
}

static void buffer_release(buffer_t *buf)
{
    if (buf)
    {
        if (buf->contents)
        {
            free(buf->contents);
            buf->contents = NULL;
        }
        free(buf);
    }
}

void buffer_reader_release(buffer_reader_t *reader)
{
    if (!reader)
    {
        return;
    }

    buffer_t *next_buffer = NULL;
    buffer_t *buffer = reader->current_buffer;
    while (buffer)
    {
        reader->current_buffer = next_buffer = buffer->next_buffer;
        buffer_release(buffer);
        buffer = next_buffer;
    }
    if (!reader->end_reached)
    {
        fclose(reader->handler);
    }
    free(reader);
}

static bool buffer_reader_proceed(buffer_reader_t *reader)
{
    while (reader->current_buffer && reader->current_buffer->checkpoint == reader->current_buffer->length)
    {
        buffer_t *next = reader->current_buffer->next_buffer;
        buffer_release(reader->current_buffer);
        if (next)
        {
            reader->current_buffer = next;
        }
        else
        {
            reader->current_buffer = NULL;
            buffer_reader_acquire(reader);
        }
    }
    return buffer_reader_available(reader) > 0;
}

bool buffer_reader_has_data(buffer_reader_t *reader)
{
    return (reader->current_buffer || buffer_reader_acquire(reader)) &&
           (reader->current_buffer->checkpoint < reader->current_buffer->length || buffer_reader_proceed(reader));
}

size_t buffer_reader_available(buffer_reader_t *reader)
{
    size_t available = 0;
    buffer_t *curr = reader->current_buffer;
    while (curr)
    {
        available += curr->length - curr->checkpoint;
        if (!curr->next_buffer && !reader->end_reached)
        {
            buffer_reader_acquire(reader);
        }
        curr = curr->next_buffer;
    }
    return available;
}

char buffer_reader_current_char(buffer_reader_t *reader, size_t offset)
{
    size_t remaining_offset = offset;
    buffer_t *buffer = reader->current_buffer;
    while (buffer->checkpoint + remaining_offset >= buffer->length)
    {
        remaining_offset -= buffer->length - buffer->checkpoint;
        if (!buffer->next_buffer)
        {
            buffer_reader_acquire(reader);
            if (!buffer->next_buffer)
            {
                return EOF;
            }
        }
        buffer = buffer->next_buffer;
    }
    return buffer->contents[buffer->checkpoint + remaining_offset];
}

void buffer_reader_commit(buffer_reader_t *reader, size_t offset)
{
    size_t remaining_offset = offset;
    buffer_t *buffer = reader->current_buffer;
    while (buffer && buffer->checkpoint + remaining_offset > buffer->length)
    {
        remaining_offset -= buffer->length - buffer->checkpoint;
        buffer->checkpoint = buffer->length;
        if (buffer->next_buffer)
        {
            buffer = buffer->next_buffer;
        }
        else
        {
            buffer_reader_acquire(reader);
            buffer = buffer->next_buffer;
        }
    }
    if (buffer)
    {
        buffer->checkpoint += remaining_offset;
        buffer_reader_proceed(reader);
    }
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void buffer_reader_current_copy(buffer_reader_t *reader, size_t size, char *dest)
{
    buffer_t *buffer = reader->current_buffer;
    if (buffer->checkpoint + size < buffer->length)
    {
        memcpy(dest, &(buffer->contents[buffer->checkpoint]), size);
    }
    else
    {
        dest[size - 1] = '\0';
        size_t remaining = size - (buffer->length - buffer->checkpoint);
        memcpy(dest, &(buffer->contents[buffer->checkpoint]), buffer->length - buffer->checkpoint);
        char *curr_dest = &(dest[buffer->length - buffer->checkpoint]);
        while (remaining > 0)
        {
            if (!buffer->next_buffer)
            {
                buffer_reader_acquire(reader);
                buffer = buffer->next_buffer;
                if (!buffer)
                {
                    remaining = 0;
                    continue;
                }
            }
            else
            {
                buffer = buffer->next_buffer;
            }

            size_t curr_length = MIN(remaining, buffer->length);
            memcpy(curr_dest, buffer->contents, curr_length);
            curr_dest = &(curr_dest[curr_length]);
            remaining -= curr_length;
        }
    }
    dest[size - 1] = '\0';
}
