#include "navigation.h"

#include "helper.h"

NavigationResult navigate_up(coordinates_t *top_cell,
                             coordinates_t *cursor_position)
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

NavigationResult navigate_down(coordinates_t *top_cell,
                               coordinates_t *cursor_position,
                               const screen_size_t *screen_size,
                               size_t num_lines)
{
    if (top_cell->y + screen_size->height < num_lines && top_cell->y + screen_size->height == cursor_position->y + 1)
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

NavigationResult navigate_left(coordinates_t *top_cell,
                               coordinates_t *cursor_position)
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

NavigationResult navigate_right(coordinates_t *top_cell,
                                coordinates_t *cursor_position,
                                const screen_size_t *screen_size,
                                size_t num_columns)
{
    LOGGER_INFO("top_cell->x=%d screen_size->width=%d num_columns=%d cursor_position->x=%d\n", top_cell->x, screen_size->width, num_columns, cursor_position->x);
    if (top_cell->x + screen_size->width < num_columns && top_cell->x + screen_size->width == cursor_position->x + 1)
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