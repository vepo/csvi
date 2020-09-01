#ifndef NAVIGATION_H_
#define NAVIGATION_H_

#include "matrix-config.h"

enum NavigationResults
{
    NONE = 1,
    BEEP = 2,
    CURSOR_UPDATED = 3
};

typedef enum NavigationResults NavigationResult;

NavigationResult navigate_up(coordinates_t *top_cell, coordinates_t *cursor_position);
NavigationResult navigate_down(coordinates_t *top_cell, coordinates_t *cursor_position, const screen_size_t *screen_size, size_t num_lines);
NavigationResult navigate_left(coordinates_t *top_cell, coordinates_t *cursor_position);
NavigationResult navigate_right(coordinates_t *top_cell, coordinates_t *cursor_position, const screen_size_t *screen_size, size_t num_columns);
NavigationResult navigate_page_up(coordinates_t *top_cell, coordinates_t *cursor_position, const screen_size_t *screen_size);

#endif