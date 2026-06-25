#ifndef CSVI_SEARCH_H_
#define CSVI_SEARCH_H_

#include <stddef.h>
#include <stdbool.h>

#include "io/csv-reader.h"
#include "layout/types.h"

typedef struct csvi_search csvi_search_t;

csvi_search_t *csvi_search_create(const csv_contents *file);
void csvi_search_dispose(csvi_search_t *search);
bool csvi_search_find(csvi_search_t *search, const char *pattern);
bool csvi_search_next(csvi_search_t *search, coordinates_t *match);
bool csvi_search_prev(csvi_search_t *search, coordinates_t *match);
size_t csvi_search_match_count(const csvi_search_t *search);
size_t csvi_search_match_index(const csvi_search_t *search);
const char *csvi_search_pattern(const csvi_search_t *search);

#endif
