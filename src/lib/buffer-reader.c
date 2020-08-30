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
    if (!fp)
    {
        fprintf(stderr, "Could not open file: %s\n", path);
        exit(-1);
    }

    reader->handler = fp;
    reader->currentBuffer = NULL;
    reader->endReached = false;
    return reader;
}

bool buffer_reader_acquire(buffer_reader *reader)
{
    if (!reader->endReached)
    {
        char *data = (char *)malloc(sizeof(char) * BUFFER_SIZE);
        size_t readData = fread(data, sizeof(char), BUFFER_SIZE, reader->handler);
        if (readData)
        {
            buffer *nextBuffer = NULL;
            if (!reader->currentBuffer)
            {
                nextBuffer = reader->currentBuffer = (buffer *)malloc(sizeof(buffer));
            }
            else
            {
                buffer *currBuffer = reader->currentBuffer;
                while (currBuffer->nextBuffer)
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
            reader->endReached = true;
        }
    }
    return false;
}

void buffer_release(buffer *buffer)
{
    if (buffer)
    {
        if (buffer->contents)
        {
            free(buffer->contents);
            buffer->contents = NULL;
        }
        free(buffer);
    }
}

void buffer_reader_release(buffer_reader *reader)
{
    buffer *nextBuffer = NULL;
    buffer *buffer = reader->currentBuffer;
    while (buffer)
    {
        reader->currentBuffer = nextBuffer = buffer->nextBuffer;
        buffer_release(buffer);
        buffer = nextBuffer;
    }
    if (!reader->endReached)
    {
        fclose(reader->handler);
    }
}

bool buffer_reader_proceed(buffer_reader *reader)
{
    while (reader->currentBuffer && reader->currentBuffer->checkpoint == reader->currentBuffer->length)
    {
        buffer *next = reader->currentBuffer->nextBuffer;
        buffer_release(reader->currentBuffer);
        if (next)
        {
            reader->currentBuffer = next;
        }
        else
        {
            reader->currentBuffer = NULL;
            buffer_reader_acquire(reader);
        }
    }
    return buffer_reader_available(reader) > 0;
}

bool buffer_reader_has_data(buffer_reader *reader)
{
    return (reader->currentBuffer || buffer_reader_acquire(reader)) &&
           (reader->currentBuffer->checkpoint < reader->currentBuffer->length || buffer_reader_proceed(reader));
}

size_t buffer_reader_available(buffer_reader *reader)
{
    size_t available = 0;
    buffer *curr = reader->currentBuffer;
    while (curr)
    {
        available += curr->length - curr->checkpoint;
        if (!curr->nextBuffer && !reader->endReached)
        {
            buffer_reader_acquire(reader);
        }
        curr = curr->nextBuffer;
    }
    return available;
}

char buffer_reader_current_char(buffer_reader *reader, size_t offset)
{
    size_t remaingOffset = offset;
    buffer *buffer = reader->currentBuffer;
    while (buffer->checkpoint + remaingOffset >= buffer->length)
    {
        remaingOffset -= buffer->length - buffer->checkpoint;
        if (!buffer->nextBuffer)
        {
            buffer_reader_acquire(reader);
            if (!buffer->nextBuffer)
            {
                return EOF;
            }
        }
        buffer = buffer->nextBuffer;
    }
    return buffer->contents[buffer->checkpoint + remaingOffset];
}

void buffer_reader_commit(buffer_reader *reader, size_t offset)
{
    size_t remaingOffset = offset;
    buffer *buffer = reader->currentBuffer;
    while (buffer && buffer->checkpoint + remaingOffset > buffer->length)
    {
        remaingOffset -= buffer->length - buffer->checkpoint;
        buffer->checkpoint = buffer->length;
        if (buffer->nextBuffer)
        {
            buffer = buffer->nextBuffer;
        }
        else
        {
            buffer_reader_acquire(reader);
            buffer = buffer->nextBuffer;
        }
    }
    if (buffer)
    {
        buffer->checkpoint += remaingOffset;
        buffer_reader_proceed(reader);
    }
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void buffer_reader_current_copy(buffer_reader *reader, size_t size, char *dest)
{
    buffer *buffer = reader->currentBuffer;
    if (buffer->checkpoint + size < buffer->length)
    {
        memcpy(dest, &(buffer->contents[buffer->checkpoint]), size);
    }
    else
    {
        dest[size - 1] = '\0';
        size_t remaining = size - (buffer->length - buffer->checkpoint);
        memcpy(dest, &(buffer->contents[buffer->checkpoint]), buffer->length - buffer->checkpoint);
        char *currDest = &(dest[buffer->length - buffer->checkpoint]);
        while (remaining > 0)
        {
            if (!buffer->nextBuffer)
            {
                buffer_reader_acquire(reader);
                buffer = buffer->nextBuffer;
                if (!buffer)
                {
                    remaining = 0;
                    continue;
                }
            }
            else
            {
                buffer = buffer->nextBuffer;
            }

            size_t currLength = MIN(remaining, buffer->length);
            memcpy(currDest, buffer->contents, currLength);
            currDest = &(currDest[currLength]);
            remaining -= currLength;
        }
    }
    dest[size - 1] = '\0';
}
