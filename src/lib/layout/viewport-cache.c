#include "layout/viewport-cache.h"

#include <stdlib.h>
#include <string.h>

#include "layout/matrix-config.h"

viewport_cache_t *viewport_cache_build(const csv_contents *contents)
{
    if (!contents || contents->columns == 0 || contents->lines == 0)
    {
        return NULL;
    }

    viewport_cache_t *cache = calloc(1, sizeof(viewport_cache_t));
    if (!cache)
    {
        return NULL;
    }

    cache->columns = contents->columns;
    cache->lines = contents->lines;
    cache->col_width = calloc(cache->columns, sizeof(size_t));
    cache->row_height = calloc(cache->lines, sizeof(size_t));
    if (!cache->col_width || !cache->row_height)
    {
        viewport_cache_dispose(cache);
        return NULL;
    }

    for (size_t y = 0; y < cache->lines; ++y)
    {
        for (size_t x = 0; x < cache->columns; ++x)
        {
            csv_token *token = csv_reader_get_token(x, y, contents);
            if (!token || !token->data)
            {
                continue;
            }

            cell_info_t info = {.width = 0, .height = 1};
            matrix_config_load_cell_info(token->data, &info);
            if (cache->col_width[x] < info.width)
            {
                cache->col_width[x] = info.width;
            }
            if (cache->row_height[y] < info.height)
            {
                cache->row_height[y] = info.height;
            }
        }
    }

    for (size_t i = 0; i < cache->columns; ++i)
    {
        if (cache->col_width[i] == 0)
        {
            cache->col_width[i] = 1;
        }
    }
    for (size_t i = 0; i < cache->lines; ++i)
    {
        if (cache->row_height[i] == 0)
        {
            cache->row_height[i] = 1;
        }
    }

    return cache;
}

void viewport_cache_dispose(viewport_cache_t *cache)
{
    if (!cache)
    {
        return;
    }
    free(cache->col_width);
    free(cache->row_height);
    free(cache);
}

size_t viewport_cache_col_width(const viewport_cache_t *cache, size_t col)
{
    if (!cache || col >= cache->columns)
    {
        return 1;
    }
    return cache->col_width[col];
}

size_t viewport_cache_row_height(const viewport_cache_t *cache, size_t row)
{
    if (!cache || row >= cache->lines)
    {
        return 1;
    }
    return cache->row_height[row];
}
