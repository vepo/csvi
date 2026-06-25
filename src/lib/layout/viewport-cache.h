#ifndef CSVI_VIEWPORT_CACHE_H_
#define CSVI_VIEWPORT_CACHE_H_

#include <stddef.h>

#include "io/csv-reader.h"
#include "layout/types.h"

typedef struct viewport_cache
{
    size_t columns;
    size_t lines;
    size_t *col_width;
    size_t *row_height;
} viewport_cache_t;

viewport_cache_t *viewport_cache_build(const csv_contents *contents);
void viewport_cache_dispose(viewport_cache_t *cache);
size_t viewport_cache_col_width(const viewport_cache_t *cache, size_t col);
size_t viewport_cache_row_height(const viewport_cache_t *cache, size_t row);

#endif
