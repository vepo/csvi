#ifndef CSV_READER_H_
#define CSV_READER_H_

typedef struct csv_token
{
    char *data;
    struct csv_token *next;
} csv_token;

typedef struct csv_contents
{
    int columns;
    int lines;
    csv_token *first;
} csv_contents;

csv_contents *csv_reader_read_file(char *path);

#endif
