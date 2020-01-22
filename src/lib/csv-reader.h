#ifndef CSV_READER_H_
#define CSV_READER_H_
#include <stdlib.h>

typedef struct csv_token
{
    size_t x;
    size_t y;
    char *data;
    struct csv_token *next;
} csv_token;

typedef struct csv_contents
{
    size_t columns;
    size_t lines;
    csv_token *first;
} csv_contents;

csv_contents *csv_reader_read_file(char *path);
csv_token *csv_reader_get_token(size_t x, size_t y, csv_contents *contents);

#endif
