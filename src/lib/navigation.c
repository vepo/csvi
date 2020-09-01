#include "navigation.h"

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

NavigationResult navigate_page_up(coordinates_t *top_cell,
                                  coordinates_t *cursor_position,
                                  const screen_size_t *screen_size)
{
    if (cursor_position->y > 0)
    {
        if (top_cell->y > screen_size->height)
        {
            top_cell->y -= screen_size->height;
            cursor_position->y -= screen_size->height;
        }
        else
        {
            top_cell->y = cursor_position->y = 0;
        }
        return CURSOR_UPDATED;
    }
    else
    {
        return BEEP;
    }
}

NavigationResult navigate_page_down(coordinates_t *top_cell,
                                    coordinates_t *cursor_position,
                                    const screen_size_t *screen_size,
                                    size_t num_lines)
{
    if (cursor_position->y < num_lines - 1)
    {
        if (top_cell->y + screen_size->height < num_lines &&
            cursor_position->y + screen_size->height < num_lines)
        {
            top_cell->y += screen_size->height;
            cursor_position->y += screen_size->height;
        }
        else
        {
            top_cell->y = cursor_position->y = num_lines - 1;
        }

        return CURSOR_UPDATED;
    }
    else
    {
        return BEEP;
    }
}

NavigationResult navigate_page_previous(coordinates_t *top_cell,
                                        coordinates_t *cursor_position,
                                        const screen_size_t *screen_size)
{
    if (cursor_position->x > 0)
    {
        if (top_cell->x > screen_size->width)
        {
            top_cell->x -= screen_size->width - 1;
            cursor_position->x -= screen_size->width - 1;
        }
        else
        {
            top_cell->x = cursor_position->x = 0;
        }
        return CURSOR_UPDATED;
    }
    else
    {
        return BEEP;
    }
}

NavigationResult navigate_page_next(coordinates_t *top_cell,
                                    coordinates_t *cursor_position,
                                    const screen_size_t *screen_size,
                                    size_t num_columns)
{
    if (cursor_position->x < num_columns - 1)
    {
        if (top_cell->x + screen_size->width < num_columns &&
            cursor_position->x + screen_size->width < num_columns)
        {
            top_cell->x += screen_size->width;
            cursor_position->x += screen_size->width;
        }
        else
        {
            cursor_position->x = num_columns - 1;
        }

        return CURSOR_UPDATED;
    }
    else
    {
        return BEEP;
    }
}