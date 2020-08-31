#include "navigation.h"

NavigationResult navigate_up(coordinates_t *top_cell, coordinates_t *cursor_position, screen_size_t *screen_size, size_t num_lines, size_t num_columns)
{
    if (top_cell->y > 0 && cursor_position->y == top_cell->y)
    {
        top_cell->y--;
    }

    if (cursor_position->y > 0)
    {
        cursor_position->y--;
        return CURSOR_UPDATED;
    }
    else
    {
        return BEEP;
    }
}

NavigationResult navigate_down(coordinates_t *top_cell, coordinates_t *cursor_position, screen_size_t *screen_size, size_t num_lines, size_t num_columns)
{
    if (top_cell->y < num_lines && cursor_position->y + 1 >= screen_size->height + top_cell->y)
    {
        top_cell->y++;
    }

    if (cursor_position->y < num_lines - 1)
    {
        cursor_position->y++;
        return CURSOR_UPDATED;
    }
    else
    {
        return BEEP;
    }
}

NavigationResult navigate_left(coordinates_t *top_cell, coordinates_t *cursor_position, screen_size_t *screen_size, size_t num_lines, size_t num_columns)
{
    if (top_cell->x > 0 && cursor_position->x == top_cell->x)
    {
        top_cell->x--;
    }

    if (cursor_position->x > 0)
    {
        cursor_position->x--;
        return CURSOR_UPDATED;
    }
    else
    {
        return BEEP;
    }
}

NavigationResult navigate_right(coordinates_t *top_cell, coordinates_t *cursor_position, screen_size_t *screen_size, size_t num_lines, size_t num_columns)
{
    if (top_cell->x < num_columns - 1 && screen_size->width + top_cell->x - 1 == cursor_position->x + 1)
    {
        top_cell->x++;
    }

    if (cursor_position->x < num_columns - 1)
    {
        cursor_position->x++;
        return CURSOR_UPDATED;
    }
    else
    {
        return BEEP;
    }
}