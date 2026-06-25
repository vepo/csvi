#include "nav/navigation.h"

static NavigationResult nav_result(coordinates_t *top_before,
                                   coordinates_t *top_after,
                                   coordinates_t *cursor_before,
                                   coordinates_t *cursor_after,
                                   NavigationResult base)
{
    (void)top_before;
    (void)top_after;
    if (base == BEEP)
    {
        return BEEP;
    }
    if (cursor_before->x != cursor_after->x || cursor_before->y != cursor_after->y)
    {
        return CURSOR_UPDATED;
    }
    if (top_before->x != top_after->x || top_before->y != top_after->y)
    {
        return CURSOR_UPDATED;
    }
    return NONE;
}

void nav_scroll_into_view(coordinates_t *top_cell,
                            const coordinates_t *cursor_position,
                            size_t viewport_width,
                            size_t viewport_height,
                            size_t num_columns,
                            size_t num_lines)
{
    size_t vw = viewport_width > 0 ? viewport_width : 1;
    size_t vh = viewport_height > 0 ? viewport_height : 1;

    if (cursor_position->x < top_cell->x)
    {
        top_cell->x = cursor_position->x;
    }
    if (cursor_position->y < top_cell->y)
    {
        top_cell->y = cursor_position->y;
    }
    if (cursor_position->x >= top_cell->x + vw)
    {
        top_cell->x = cursor_position->x >= vw ? cursor_position->x - vw + 1 : 0;
    }
    if (cursor_position->y >= top_cell->y + vh)
    {
        top_cell->y = cursor_position->y >= vh ? cursor_position->y - vh + 1 : 0;
    }

    if (num_columns > 0 && num_columns > vw && top_cell->x + vw > num_columns)
    {
        top_cell->x = num_columns - vw;
    }
    if (num_lines > 0 && num_lines > vh && top_cell->y + vh > num_lines)
    {
        top_cell->y = num_lines - vh;
    }
}

NavigationResult navigate_up(coordinates_t *top_cell,
                           coordinates_t *cursor_position,
                           const screen_size_t *screen_size)
{
    coordinates_t top_before = *top_cell;
    coordinates_t cursor_before = *cursor_position;

    if (cursor_position->y == 0)
    {
        return BEEP;
    }

    cursor_position->y--;
    nav_scroll_into_view(top_cell,
                         cursor_position,
                         screen_size ? (size_t)screen_size->width : 1,
                         screen_size ? (size_t)screen_size->height : 1,
                         0,
                         0);
    return nav_result(&top_before, top_cell, &cursor_before, cursor_position, CURSOR_UPDATED);
}

NavigationResult navigate_down(coordinates_t *top_cell,
                               coordinates_t *cursor_position,
                               const screen_size_t *screen_size,
                               size_t num_lines)
{
    coordinates_t top_before = *top_cell;
    coordinates_t cursor_before = *cursor_position;

    if (cursor_position->y >= num_lines - 1)
    {
        return BEEP;
    }

    cursor_position->y++;
    nav_scroll_into_view(top_cell,
                         cursor_position,
                         screen_size ? (size_t)screen_size->width : 1,
                         screen_size ? (size_t)screen_size->height : 1,
                         0,
                         num_lines);
    return nav_result(&top_before, top_cell, &cursor_before, cursor_position, CURSOR_UPDATED);
}

NavigationResult navigate_left(coordinates_t *top_cell,
                             coordinates_t *cursor_position,
                             const screen_size_t *screen_size)
{
    coordinates_t top_before = *top_cell;
    coordinates_t cursor_before = *cursor_position;

    if (cursor_position->x == 0)
    {
        return BEEP;
    }

    cursor_position->x--;
    nav_scroll_into_view(top_cell,
                         cursor_position,
                         screen_size ? (size_t)screen_size->width : 1,
                         screen_size ? (size_t)screen_size->height : 1,
                         0,
                         0);
    return nav_result(&top_before, top_cell, &cursor_before, cursor_position, CURSOR_UPDATED);
}

NavigationResult navigate_right(coordinates_t *top_cell,
                                coordinates_t *cursor_position,
                                const screen_size_t *screen_size,
                                size_t num_columns)
{
    coordinates_t top_before = *top_cell;
    coordinates_t cursor_before = *cursor_position;

    if (cursor_position->x >= num_columns - 1)
    {
        return BEEP;
    }

    cursor_position->x++;
    nav_scroll_into_view(top_cell,
                         cursor_position,
                         screen_size ? (size_t)screen_size->width : 1,
                         screen_size ? (size_t)screen_size->height : 1,
                         num_columns,
                         0);
    return nav_result(&top_before, top_cell, &cursor_before, cursor_position, CURSOR_UPDATED);
}

NavigationResult navigate_page_up(coordinates_t *top_cell,
                                  coordinates_t *cursor_position,
                                  const screen_size_t *screen_size)
{
    coordinates_t top_before = *top_cell;
    coordinates_t cursor_before = *cursor_position;

    if (cursor_position->y > 0)
    {
        if (top_cell->y >= (size_t)screen_size->height)
        {
            top_cell->y -= screen_size->height;
            cursor_position->y -= screen_size->height;
        }
        else
        {
            top_cell->y = 0;
            cursor_position->y = 0;
        }
        return nav_result(&top_before, top_cell, &cursor_before, cursor_position, CURSOR_UPDATED);
    }

    return BEEP;
}

NavigationResult navigate_page_down(coordinates_t *top_cell,
                                    coordinates_t *cursor_position,
                                    const screen_size_t *screen_size,
                                    size_t num_lines)
{
    coordinates_t top_before = *top_cell;
    coordinates_t cursor_before = *cursor_position;

    if (cursor_position->y < num_lines - 1)
    {
        size_t page = (size_t)screen_size->height;
        if (page == 0)
        {
            page = 1;
        }
        if (top_cell->y + page < num_lines && cursor_position->y + page < num_lines)
        {
            top_cell->y += page;
            cursor_position->y += page;
        }
        else
        {
            top_cell->y = num_lines > page ? num_lines - page : 0;
            cursor_position->y = num_lines - 1;
        }
        return nav_result(&top_before, top_cell, &cursor_before, cursor_position, CURSOR_UPDATED);
    }

    return BEEP;
}

NavigationResult navigate_row_start(coordinates_t *top_cell,
                                    coordinates_t *cursor_position,
                                    size_t num_columns)
{
    (void)num_columns;
    coordinates_t top_before = *top_cell;
    coordinates_t cursor_before = *cursor_position;

    if (cursor_position->x > 0)
    {
        top_cell->x = 0;
        cursor_position->x = 0;
        return nav_result(&top_before, top_cell, &cursor_before, cursor_position, CURSOR_UPDATED);
    }

    return BEEP;
}

NavigationResult navigate_row_end(coordinates_t *top_cell,
                                  coordinates_t *cursor_position,
                                  const screen_size_t *screen_size,
                                  size_t num_columns)
{
    coordinates_t top_before = *top_cell;
    coordinates_t cursor_before = *cursor_position;

    if (num_columns == 0)
    {
        return BEEP;
    }

    if (cursor_position->x < num_columns - 1)
    {
        cursor_position->x = num_columns - 1;
        nav_scroll_into_view(top_cell,
                             cursor_position,
                             screen_size ? (size_t)screen_size->width : 1,
                             screen_size ? (size_t)screen_size->height : 1,
                             num_columns,
                             0);
        return nav_result(&top_before, top_cell, &cursor_before, cursor_position, CURSOR_UPDATED);
    }

    return BEEP;
}

NavigationResult navigate_first_row(coordinates_t *top_cell, coordinates_t *cursor_position)
{
    coordinates_t top_before = *top_cell;
    coordinates_t cursor_before = *cursor_position;

    if (cursor_position->y > 0)
    {
        top_cell->y = 0;
        cursor_position->y = 0;
        return nav_result(&top_before, top_cell, &cursor_before, cursor_position, CURSOR_UPDATED);
    }

    return BEEP;
}

NavigationResult navigate_last_row(coordinates_t *top_cell,
                                   coordinates_t *cursor_position,
                                   size_t num_lines)
{
    coordinates_t top_before = *top_cell;
    coordinates_t cursor_before = *cursor_position;

    if (num_lines == 0)
    {
        return BEEP;
    }

    if (cursor_position->y < num_lines - 1)
    {
        cursor_position->y = num_lines - 1;
        top_cell->y = cursor_position->y;
        return nav_result(&top_before, top_cell, &cursor_before, cursor_position, CURSOR_UPDATED);
    }

    return BEEP;
}

NavigationResult navigate_page_left(coordinates_t *top_cell,
                                    coordinates_t *cursor_position,
                                    const screen_size_t *screen_size)
{
    coordinates_t top_before = *top_cell;
    coordinates_t cursor_before = *cursor_position;

    if (cursor_position->x > 0)
    {
        size_t page = (size_t)screen_size->width;
        if (page == 0)
        {
            page = 1;
        }
        if (top_cell->x >= page)
        {
            top_cell->x -= page;
            cursor_position->x -= page;
        }
        else
        {
            top_cell->x = 0;
            cursor_position->x = 0;
        }
        return nav_result(&top_before, top_cell, &cursor_before, cursor_position, CURSOR_UPDATED);
    }

    return BEEP;
}

NavigationResult navigate_page_right(coordinates_t *top_cell,
                                     coordinates_t *cursor_position,
                                     const screen_size_t *screen_size,
                                     size_t num_columns)
{
    coordinates_t top_before = *top_cell;
    coordinates_t cursor_before = *cursor_position;

    if (cursor_position->x < num_columns - 1)
    {
        size_t page = (size_t)screen_size->width;
        if (page == 0)
        {
            page = 1;
        }
        if (top_cell->x + page < num_columns && cursor_position->x + page < num_columns)
        {
            top_cell->x += page;
            cursor_position->x += page;
        }
        else
        {
            cursor_position->x = num_columns - 1;
            nav_scroll_into_view(top_cell,
                                 cursor_position,
                                 page,
                                 screen_size ? (size_t)screen_size->height : 1,
                                 num_columns,
                                 0);
        }
        return nav_result(&top_before, top_cell, &cursor_before, cursor_position, CURSOR_UPDATED);
    }

    return BEEP;
}
