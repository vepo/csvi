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

void csv_reader_set_separator(char separator);
csv_contents *csv_reader_read_file(char *path);
csv_token *csv_reader_get_token(size_t x, size_t y, const csv_contents *contents);
void csv_contents_dispose(csv_contents *);
void csv_token_dispose(csv_token *);

#endif
