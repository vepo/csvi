#ifndef MATRIX_CONFIG_H_
#define MATRIX_CONFIG_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct matrix_config
{
    size_t *column_width;
    size_t *line_height;
} matrix_config_t;

matrix_config_t *matrix_config_initialize(size_t width, size_t height);

void matrix_config_dispose(matrix_config_t *config);

#endif