#ifndef MATRIX_CONFIG_H_
#define MATRIX_CONFIG_H_

#include <stdbool.h>
#include <stddef.h>
#include "csv-reader.h"

typedef struct matrix_properties
{
    size_t cell_padding_top;
    size_t cell_padding_right;
    size_t cell_padding_bottom;
    size_t cell_padding_left;
    size_t margin_top;
    size_t margin_right;
    size_t margin_bottom;
    size_t margin_left;
} matrix_properties_t;

typedef struct matrix_config
{
    size_t *column_width;
    size_t *line_height;
} matrix_config_t;

typedef struct screen_config
{
    int width;
    int height;
} screen_config_t;

void matrix_config_get_most_expanded(screen_config_t *available, matrix_properties_t *properties, csv_token *start_token, size_t max_lines, size_t max_columns, screen_config_t *used);
void matrix_config_load(size_t width, size_t height, csv_token *start_token, matrix_config_t *config);
matrix_config_t *matrix_config_initialize(size_t width, size_t height);
void matrix_config_dispose(matrix_config_t *config);

#endif