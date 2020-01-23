#include "matrix-config.h"

#include <stdlib.h>

matrix_config_t *matrix_config_initialize(size_t width, size_t height)
{
    matrix_config_t *config = (matrix_config_t *)malloc(sizeof(matrix_config_t));
    config->column_width = (size_t *)calloc(width, sizeof(size_t));
    config->line_height = (size_t *)calloc(height, sizeof(size_t));
    return config;
}

void matrix_config_dispose(matrix_config_t *config)
{
    free(config->column_width);
    free(config->line_height);
    free(config);
}
