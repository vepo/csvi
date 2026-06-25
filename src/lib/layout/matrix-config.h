#ifndef CSVI_MATRIX_CONFIG_H_
#define CSVI_MATRIX_CONFIG_H_

#include <stdbool.h>
#include <stddef.h>

#include "io/csv-reader.h"
#include "layout/types.h"

typedef struct matrix_config
{
    size_t *column_width;
    size_t *line_height;
    size_t heights;
    size_t columns;
} matrix_config_t;

bool layout_can_show_sizes(const screen_size_t *grid_px,
                           const matrix_properties_t *properties,
                           const size_t *widths,
                           const size_t *heights,
                           const screen_size_t *cell_counts);

void matrix_config_get_most_expanded(const screen_size_t *available,
                                     const matrix_properties_t *properties,
                                     csv_token *start_token,
                                     size_t max_columns,
                                     size_t max_lines,
                                     screen_size_t *used);

void matrix_config_load_sizes(csv_token *start_token,
                              matrix_config_t *config);

matrix_config_t *matrix_config_initialize(size_t width,
                                          size_t height);

void matrix_config_dispose(matrix_config_t *config);

void matrix_config_load_cell_info(char *cell_data,
                                  cell_info_t *cell_info);

#endif
