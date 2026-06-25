#ifndef CSVI_NAVIGATION_H_
#define CSVI_NAVIGATION_H_

#include "layout/types.h"

enum NavigationResults
{
    NONE = 1,
    BEEP = 2,
    CURSOR_UPDATED = 3,
    CURSOR_ONLY = 3,
    VIEWPORT_SHIFT = 4
};

typedef enum NavigationResults NavigationResult;

void nav_scroll_into_view(coordinates_t *top_cell,
                            const coordinates_t *cursor_position,
                            size_t viewport_width,
                            size_t viewport_height,
                            size_t num_columns,
                            size_t num_lines);

NavigationResult navigate_up(coordinates_t *top_cell,
                           coordinates_t *cursor_position,
                           const screen_size_t *screen_size);
NavigationResult navigate_down(coordinates_t *top_cell, coordinates_t *cursor_position, const screen_size_t *screen_size, size_t num_lines);
NavigationResult navigate_left(coordinates_t *top_cell,
                               coordinates_t *cursor_position,
                               const screen_size_t *screen_size);
NavigationResult navigate_right(coordinates_t *top_cell, coordinates_t *cursor_position, const screen_size_t *screen_size, size_t num_columns);
NavigationResult navigate_page_up(coordinates_t *top_cell, coordinates_t *cursor_position, const screen_size_t *screen_size);
NavigationResult navigate_page_down(coordinates_t *top_cell, coordinates_t *cursor_position, const screen_size_t *screen_size, size_t num_lines);
NavigationResult navigate_row_start(coordinates_t *top_cell, coordinates_t *cursor_position, size_t num_columns);
NavigationResult navigate_row_end(coordinates_t *top_cell,
                                  coordinates_t *cursor_position,
                                  const screen_size_t *screen_size,
                                  size_t num_columns);
NavigationResult navigate_first_row(coordinates_t *top_cell, coordinates_t *cursor_position);
NavigationResult navigate_last_row(coordinates_t *top_cell, coordinates_t *cursor_position, size_t num_lines);
NavigationResult navigate_page_left(coordinates_t *top_cell, coordinates_t *cursor_position, const screen_size_t *screen_size);
NavigationResult navigate_page_right(coordinates_t *top_cell, coordinates_t *cursor_position, const screen_size_t *screen_size, size_t num_columns);

#endif
