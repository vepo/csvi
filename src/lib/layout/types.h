#ifndef CSVI_LAYOUT_TYPES_H_
#define CSVI_LAYOUT_TYPES_H_

#include <stddef.h>

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

typedef struct coordinates
{
    size_t x;
    size_t y;
} coordinates_t;

typedef struct cell_info
{
    size_t width;
    size_t height;
} cell_info_t;

typedef struct screen_size
{
    int width;
    int height;
} screen_size_t;

#endif
