#include "io/csv-reader.h"
#include "io/buffer-reader.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static char csv_reader_separator = ';';

static void commit_while_isspace(buffer_reader_t *reader)
{
    while (buffer_reader_available(reader) > 0 && isspace(buffer_reader_current_char(reader, 0)))
    {
        buffer_reader_commit(reader, 1);
    }
}

static int count_spaces_previous(buffer_reader_t *reader, size_t start_pos)
{
    int spaces = 0;
    while (isspace(buffer_reader_current_char(reader, start_pos - spaces - 1)))
    {
        ++spaces;
    }
    return spaces;
}

static void proceed_token(csv_contents *contents, csv_token *current_token, csv_token *last_token)
{
    if (last_token)
    {
        last_token->next = current_token;
    }
    else
    {
        contents->first = current_token;
    }
}

static size_t process_end_of_token(buffer_reader_t *reader, size_t current_pos, size_t curr_column, csv_contents *contents)
{
    size_t column = curr_column;
    if (buffer_reader_current_char(reader, current_pos) == '\n')
    {
        contents->lines++;
        contents->columns = MAX(contents->columns, column);
        column = 0;
    }
    else
    {
        ++column;
    }

    buffer_reader_commit(reader, current_pos + 1);
    return column;
}

static size_t proceed_escaped_token(buffer_reader_t *reader, size_t *token_end)
{
    size_t escaped_counter = 0;
    while ((buffer_reader_available(reader) > *token_end + 1))
    {
        if (buffer_reader_current_char(reader, *token_end) == '"' && buffer_reader_current_char(reader, *token_end + 1) == '"')
        {
            *token_end += 2;
            ++escaped_counter;
            continue;
        }
        else if (buffer_reader_current_char(reader, *token_end) == '"')
        {
            ++*token_end;
            break;
        }
        else
        {
            ++*token_end;
        }
    }
    return *token_end - escaped_counter - 2;
}

static char *read_escaped_token(buffer_reader_t *reader, size_t token_end, size_t token_size)
{
    char *token = (char *)malloc(sizeof(char) * token_size + 1);
    size_t escaped_chars = 0;
    for (int buffer_pos = 0; buffer_pos < (int)token_size; buffer_pos++)
    {
        if (buffer_reader_current_char(reader, buffer_pos + 1 + escaped_chars) == '"' && buffer_reader_current_char(reader, buffer_pos + 2 + escaped_chars) == '"')
        {
            ++escaped_chars;
        }
        token[buffer_pos] = buffer_reader_current_char(reader, buffer_pos + 1 + escaped_chars);
    }
    token[token_size] = '\0';
    return token;
}

void csv_reader_set_separator(char separator)
{
    csv_reader_separator = separator;
}

csv_contents *csv_reader_read_file(const char *path, char *errbuf, size_t errbuf_len)
{
    csv_contents *contents = (csv_contents *)malloc(sizeof(csv_contents));
    if (!contents)
    {
        if (errbuf && errbuf_len > 0)
        {
            snprintf(errbuf, errbuf_len, "out of memory");
        }
        return NULL;
    }

    contents->lines = 0;
    contents->columns = 0;
    contents->first = NULL;
    csv_token *last_token = NULL;
    buffer_reader_t *reader = buffer_reader_open(path, errbuf, errbuf_len);
    if (!reader)
    {
        free(contents);
        return NULL;
    }

    size_t column = 0;

    while (buffer_reader_acquire(reader))
    {
        while (buffer_reader_has_data(reader))
        {
            commit_while_isspace(reader);
            if (buffer_reader_has_data(reader))
            {
                if (buffer_reader_current_char(reader, 0) == '"')
                {
                    size_t token_end = 2;
                    char *token = read_escaped_token(reader, token_end, proceed_escaped_token(reader, &token_end));

                    csv_token *current_token = (csv_token *)malloc(sizeof(csv_token));
                    current_token->data = token;
                    current_token->x = column;
                    current_token->y = contents->lines;
                    current_token->next = NULL;

                    if (buffer_reader_current_char(reader, token_end) != '\n' && buffer_reader_current_char(reader, token_end) != csv_reader_separator)
                    {
                        token_end++;
                    }
                    while (buffer_reader_available(reader) > token_end && isspace(buffer_reader_current_char(reader, token_end)) && buffer_reader_current_char(reader, token_end) != '\n')
                    {
                        ++token_end;
                    }
                    while (buffer_reader_available(reader) > token_end && buffer_reader_current_char(reader, token_end) != csv_reader_separator && buffer_reader_current_char(reader, token_end) != '\n' && buffer_reader_current_char(reader, token_end) != EOF)
                    {
                        ++token_end;
                    }
                    column = process_end_of_token(reader, token_end, column, contents);
                    proceed_token(contents, current_token, last_token);
                    last_token = current_token;
                }
                else
                {
                    size_t token_end = 0;
                    while ((buffer_reader_available(reader) > token_end || !reader->end_reached) && buffer_reader_current_char(reader, token_end) != csv_reader_separator && buffer_reader_current_char(reader, token_end) != '\n' && buffer_reader_current_char(reader, token_end) != EOF)
                    {
                        ++token_end;
                    }

                    int space_pos = count_spaces_previous(reader, token_end);

                    char *token = (char *)malloc(sizeof(char) * (token_end + 1 - space_pos));
                    buffer_reader_current_copy(reader, token_end + 1 - space_pos, token);
                    csv_token *current_token = (csv_token *)malloc(sizeof(csv_token));
                    current_token->data = token;
                    current_token->x = column;
                    current_token->y = contents->lines;
                    current_token->next = NULL;
                    column = process_end_of_token(reader, token_end, column, contents);
                    proceed_token(contents, current_token, last_token);
                    last_token = current_token;
                }
            }
        }
    }

    contents->columns = 0;
    contents->lines = 0;
    contents->index = NULL;
    if (contents->first)
    {
        csv_token *token = contents->first;
        while (token->next)
        {
            contents->columns = MAX(contents->columns, token->x);
            contents->lines = MAX(contents->lines, token->y);
            token = token->next;
        }
        contents->columns++;
        contents->lines++;
    }

    if (contents->columns > 0 && contents->lines > 0)
    {
        size_t index_size = contents->columns * contents->lines;
        contents->index = calloc(index_size, sizeof(csv_token *));
        if (contents->index)
        {
            csv_token *curr = contents->first;
            while (curr)
            {
                if (curr->y < contents->lines && curr->x < contents->columns)
                {
                    contents->index[curr->y * contents->columns + curr->x] = curr;
                }
                curr = curr->next;
            }
        }
    }

    buffer_reader_release(reader);
    return contents;
}

static csv_token *csv_contents_last_token(csv_contents *contents)
{
    csv_token *last = contents->first;
    if (!last)
    {
        return NULL;
    }
    while (last->next)
    {
        last = last->next;
    }
    return last;
}

int csv_reader_set_cell(csv_contents *contents, size_t x, size_t y, const char *value)
{
    if (!contents || x >= contents->columns || y >= contents->lines)
    {
        return -1;
    }

    const char *new_value = value ? value : "";
    csv_token *token = csv_reader_get_token(x, y, contents);

    if (token)
    {
        char *copy = strdup(new_value);
        if (!copy)
        {
            return -1;
        }
        free(token->data);
        token->data = copy;
        return 0;
    }

    csv_token *new_token = calloc(1, sizeof(csv_token));
    if (!new_token)
    {
        return -1;
    }

    new_token->data = strdup(new_value);
    if (!new_token->data)
    {
        free(new_token);
        return -1;
    }
    new_token->x = x;
    new_token->y = y;
    new_token->next = NULL;

    if (contents->first)
    {
        csv_token *last = csv_contents_last_token(contents);
        last->next = new_token;
    }
    else
    {
        contents->first = new_token;
    }

    if (contents->index)
    {
        contents->index[y * contents->columns + x] = new_token;
    }

    return 0;
}

csv_token *csv_reader_get_token(size_t x, size_t y, const csv_contents *contents)
{
    if (!contents || x >= contents->columns || y >= contents->lines)
    {
        return NULL;
    }
    if (contents->index)
    {
        return contents->index[y * contents->columns + x];
    }

    csv_token *curr_token = contents->first;
    while (curr_token)
    {
        if (curr_token->x == x && curr_token->y == y)
        {
            return curr_token;
        }
        curr_token = curr_token->next;
    }
    return NULL;
}

void csv_contents_dispose(csv_contents *contents)
{
    if (contents)
    {
        free(contents->index);
        if (contents->first)
        {
            csv_token_dispose(contents->first);
        }
        free(contents);
    }
}

void csv_token_dispose(csv_token *token)
{
    if (token)
    {
        if (token->next)
        {
            csv_token_dispose(token->next);
        }

        if (token->data)
        {
            free(token->data);
        }
        free(token);
    }
}
