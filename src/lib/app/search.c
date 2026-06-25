#include "app/search.h"

#include <stdlib.h>
#include <string.h>

struct csvi_search
{
    const csv_contents *file;
    char *pattern;
    size_t *match_rows;
    size_t *match_cols;
    size_t match_count;
    size_t match_index;
};

csvi_search_t *csvi_search_create(const csv_contents *file)
{
    csvi_search_t *search = calloc(1, sizeof(csvi_search_t));
    if (search)
    {
        search->file = file;
        search->match_index = 0;
    }
    return search;
}

void csvi_search_dispose(csvi_search_t *search)
{
    if (!search)
    {
        return;
    }
    free(search->pattern);
    free(search->match_rows);
    free(search->match_cols);
    free(search);
}

static void search_clear_matches(csvi_search_t *search)
{
    free(search->match_rows);
    free(search->match_cols);
    search->match_rows = NULL;
    search->match_cols = NULL;
    search->match_count = 0;
    search->match_index = 0;
}

bool csvi_search_find(csvi_search_t *search, const char *pattern)
{
    if (!search || !pattern)
    {
        return false;
    }

    free(search->pattern);
    search->pattern = strdup(pattern);
    search_clear_matches(search);

    if (!search->pattern || search->pattern[0] == '\0' || !search->file)
    {
        return false;
    }

    size_t capacity = 16;
    search->match_rows = malloc(capacity * sizeof(size_t));
    search->match_cols = malloc(capacity * sizeof(size_t));
    if (!search->match_rows || !search->match_cols)
    {
        search_clear_matches(search);
        return false;
    }

    for (size_t y = 0; y < search->file->lines; ++y)
    {
        for (size_t x = 0; x < search->file->columns; ++x)
        {
            csv_token *token = csv_reader_get_token(x, y, search->file);
            if (!token || !token->data)
            {
                continue;
            }
            if (strstr(token->data, search->pattern) == NULL)
            {
                continue;
            }

            if (search->match_count >= capacity)
            {
                capacity *= 2;
                size_t *new_rows = realloc(search->match_rows, capacity * sizeof(size_t));
                size_t *new_cols = realloc(search->match_cols, capacity * sizeof(size_t));
                if (!new_rows || !new_cols)
                {
                    free(new_rows);
                    free(new_cols);
                    search_clear_matches(search);
                    return false;
                }
                search->match_rows = new_rows;
                search->match_cols = new_cols;
            }

            search->match_rows[search->match_count] = y;
            search->match_cols[search->match_count] = x;
            search->match_count++;
        }
    }

    if (search->match_count > 0)
    {
        search->match_index = search->match_count - 1;
    }

    return search->match_count > 0;
}

static bool search_goto(csvi_search_t *search, coordinates_t *match, int delta)
{
    if (!search || search->match_count == 0)
    {
        return false;
    }

    if (delta > 0)
    {
        search->match_index = (search->match_index + 1) % search->match_count;
    }
    else
    {
        search->match_index = (search->match_index + search->match_count - 1) % search->match_count;
    }

    match->y = search->match_rows[search->match_index];
    match->x = search->match_cols[search->match_index];
    return true;
}

bool csvi_search_next(csvi_search_t *search, coordinates_t *match)
{
    return search_goto(search, match, 1);
}

bool csvi_search_prev(csvi_search_t *search, coordinates_t *match)
{
    return search_goto(search, match, -1);
}

size_t csvi_search_match_count(const csvi_search_t *search)
{
    return search ? search->match_count : 0;
}

size_t csvi_search_match_index(const csvi_search_t *search)
{
    return search ? search->match_index : 0;
}

const char *csvi_search_pattern(const csvi_search_t *search)
{
    return search ? search->pattern : NULL;
}

void csvi_search_refresh(csvi_search_t *search)
{
    if (!search || !search->pattern || search->pattern[0] == '\0')
    {
        return;
    }

    const char *pattern = search->pattern;
    char *saved = strdup(pattern);
    if (!saved)
    {
        return;
    }

    csvi_search_find(search, saved);
    free(saved);
}
