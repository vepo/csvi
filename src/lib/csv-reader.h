#ifndef CSV_READER_H_
#define CSV_READER_H_
#include <stdbool.h>
#include <stdio.h>

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

enum CSV_STATUS
{
    CSV_STATUS_READ = 1,
    CSV_STATUS_NO_READ_PERMISSION = 97,
    CSV_STATUS_ALREADY_OPEN = 98,
    CSV_STATUS_FILE_NOT_FOUND = 99
};

typedef enum CSV_STATUS CsvStatus;

typedef struct csv_line_metadata
{
    int index;
    long int start_position;
    long int size;
} csv_line_metadata_t;

/**
 * 
 **/
typedef struct csv_metadata
{
    CsvStatus status;
    char *directory;
    char *filename;
    FILE *file_handler;
    char separator;
    bool can_read;
    bool can_write;
    int columns;
    int lines;
    csv_line_metadata_t *lines_metadata;
} csv_metadata_t;

csv_metadata_t *csv_reader_open(char *path);

#endif
