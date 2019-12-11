#include "buffer-reader.h"

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

buffer_reader *buffer_reader_open(char *path)
{
    buffer_reader *reader = (buffer_reader *)malloc(sizeof(buffer_reader));
    FILE *fp = fopen(path, "r");
    reader->handler = fp;
    reader->currentBuffer = NULL;
    return reader;
}

bool buffer_reader_acquire(buffer_reader *reader)
{
    char *data = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    size_t readData = fread(data, sizeof(char), BUFFER_SIZE, reader->handler);
    if (readData)
    {
        buffer *nextBuffer = NULL;
        if (NULL == reader->currentBuffer)
        {
            nextBuffer = reader->currentBuffer = (buffer *)malloc(sizeof(buffer));
        }
        else
        {
            buffer *currBuffer = reader->currentBuffer;
            while (NULL != currBuffer->nextBuffer)
            {
                currBuffer = currBuffer->nextBuffer;
            }
            nextBuffer = currBuffer->nextBuffer = (buffer *)malloc(sizeof(buffer));
        }

        nextBuffer->contents = data;
        nextBuffer->checkpoint = 0;
        nextBuffer->nextBuffer = NULL;
        nextBuffer->length = readData;
        return true;
    }
    else
    {
        free(data);
        return false;
    }
}

void buffer_release(buffer *buffer)
{
    if (NULL != buffer)
    {
        if (NULL != buffer->contents)
        {
            free(buffer->contents);
        }
        free(buffer);
    }
}

void buffer_reader_release(buffer_reader *reader)
{
    buffer *nextBuffer = NULL;
    buffer *buffer = reader->currentBuffer;
    while (NULL != buffer)
    {
        nextBuffer = buffer->nextBuffer;
        buffer_release(buffer);
        buffer = nextBuffer;
    }
    fclose(reader->handler);
}

bool buffer_reader_proceed(buffer_reader *reader)
{
    if (reader->currentBuffer->checkpoint == reader->currentBuffer->length)
    {
        buffer_release(reader->currentBuffer);
        reader->currentBuffer = NULL;
    }
    return buffer_reader_acquire(reader);
}

bool buffer_reader_has_data(buffer_reader *reader)
{
    return (NULL != reader->currentBuffer || buffer_reader_acquire(reader)) &&
           (reader->currentBuffer->checkpoint < reader->currentBuffer->length || buffer_reader_proceed(reader));
}

size_t buffer_reader_available(buffer_reader *reader)
{
    size_t available = 0;
    buffer *curr = reader->currentBuffer;
    while (curr)
    {
        available += curr->length - curr->checkpoint;
        curr = curr->nextBuffer;
    }
    return available;
}

char buffer_reader_current_char(buffer_reader *reader, size_t offset)
{
    buffer *buffer = reader->currentBuffer;
    while (buffer->checkpoint + offset >= buffer->length)
    {
        offset -= buffer->length - buffer->checkpoint;
        if (!buffer->nextBuffer)
        {
            if (!buffer_reader_acquire(reader))
            {
                return EOF;
            }
        }
        buffer = buffer->nextBuffer;
    }
    return buffer->contents[buffer->checkpoint + offset];
}

void buffer_reader_commit(buffer_reader *reader, size_t offset)
{
    buffer *buffer = reader->currentBuffer;
    while (buffer->checkpoint + offset > buffer->length)
    {
        offset -= buffer->length - buffer->checkpoint;
        buffer->checkpoint = buffer->length;
        buffer = buffer->nextBuffer;
    }
    buffer->checkpoint += offset;
    buffer_reader_proceed(reader);
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void buffer_reader_current_copy(buffer_reader *reader, size_t start, size_t end, char *dest)
{
    buffer *buffer = reader->currentBuffer;
    if (buffer->checkpoint + end < buffer->length)
    {
        memcpy(dest, &(buffer->contents[buffer->checkpoint + start]), end - start);
    }
    else
    {
        size_t remaining = end - start - (buffer->length - buffer->checkpoint);
        memcpy(dest, &(buffer->contents[buffer->checkpoint + start]), buffer->length - start);
        dest = &(dest[buffer->length - start + 1]);
        while ((NULL != buffer->nextBuffer || buffer_reader_acquire(reader)) && remaining > 0)
        {
            memcpy(dest, buffer->contents, MIN(remaining, buffer->length));
            remaining -= MIN(remaining, buffer->length);
        }
    }
    dest[end - start] = '\0';
}