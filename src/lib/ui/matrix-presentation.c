#include "ui/matrix-presentation.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "common/helper.h"
#include "common/log.h"
#include "layout/matrix-config.h"

static screen_size_t configuration = {.width = 0, .height = 0};
static WINDOW *grid_window = NULL;
static bool running = true;
static matrix_display_options_t display_options = {.color_enabled = false,
                                                   .grid_enabled = false,
                                                   .header_enabled = false};

enum
{
    COLOR_PAIR_EVEN = 1,
    COLOR_PAIR_ODD = 2,
    COLOR_PAIR_SELECTED = 3,
    COLOR_PAIR_EDITING = 4,
};

static bool use_color(void)
{
    return display_options.color_enabled && has_colors();
}

static void destroy_grid_window(void)
{
    if (grid_window)
    {
        delwin(grid_window);
        grid_window = NULL;
    }
}

static void ensure_grid_window(const matrix_properties_t *properties)
{
    int height = configuration.height - properties->margin_bottom;
    int width = configuration.width - properties->margin_right;
    if (height < 1)
    {
        height = 1;
    }
    if (width < 1)
    {
        width = 1;
    }

    if (!grid_window)
    {
        grid_window = newwin(height, width, properties->margin_top, properties->margin_left);
    }
    else
    {
        wresize(grid_window, height, width);
        mvwin(grid_window, properties->margin_top, properties->margin_left);
    }
}

void matrix_presentation_init(const matrix_display_options_t *options)
{
    running = true;
    if (options)
    {
        display_options = *options;
    }

    initscr();
    if (display_options.color_enabled)
    {
        start_color();
        init_pair(COLOR_PAIR_EVEN, COLOR_BLACK, COLOR_WHITE);
        init_pair(COLOR_PAIR_ODD, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_PAIR_SELECTED, COLOR_BLACK, COLOR_GREEN);
        init_pair(COLOR_PAIR_EDITING, COLOR_BLACK, COLOR_CYAN);
    }

    noecho();
    curs_set(0);
    cbreak();
    keypad(stdscr, true);
    timeout(50);
    refresh();
}

screen_size_t *matrix_presentation_get_screen_size(void)
{
    screen_size_t curr_config;
    getmaxyx(stdscr, curr_config.height, curr_config.width);
    if (curr_config.width != configuration.width || curr_config.height != configuration.height)
    {
        configuration.width = curr_config.width;
        configuration.height = curr_config.height;
        csvi_log_info("screen size changed: (%d,%d)\n", curr_config.width, curr_config.height);
    }
    return &configuration;
}

screen_size_t *matrix_presentation_get_grid_size(const matrix_properties_t *properties)
{
    static screen_size_t grid_size;
    grid_size.width = configuration.width - properties->margin_left - properties->margin_right;
    grid_size.height = configuration.height - properties->margin_top - properties->margin_bottom;
    if (grid_size.width < 1)
    {
        grid_size.width = 1;
    }
    if (grid_size.height < 1)
    {
        grid_size.height = 1;
    }
    return &grid_size;
}

void matrix_presentation_shutdown(void)
{
    destroy_grid_window();
}

void matrix_presentation_request_stop(void)
{
    running = false;
}

void matrix_presentation_beep(void)
{
    beep();
    flash();
}

void matrix_presentation_clear_grid(const matrix_properties_t *properties)
{
    ensure_grid_window(properties);
    werase(grid_window);
    touchwin(grid_window);
}

void matrix_presentation_refresh_partial(void)
{
    if (grid_window)
    {
        wnoutrefresh(grid_window);
    }
    wnoutrefresh(stdscr);
    doupdate();
}

static void calculate_offsets(const coordinates_t *cell,
                              const matrix_config_t *config,
                              const matrix_properties_t *properties,
                              coordinates_t *position)
{
    size_t curr_x = 0;
    position->x = 0;
    size_t cell_horizontal_padding = properties->cell_padding_left + properties->cell_padding_right;
    size_t cell_vertical_padding = properties->cell_padding_top + properties->cell_padding_bottom;
    while (curr_x < cell->x)
    {
        position->x += cell_horizontal_padding + config->column_width[curr_x];
        ++curr_x;
    }

    size_t curr_y = 0;
    position->y = 0;
    while (curr_y < cell->y)
    {
        position->y += cell_vertical_padding + config->line_height[curr_y];
        ++curr_y;
    }
}

static chtype cell_display_attrs(size_t abs_x, size_t abs_y, cell_draw_state_t state, bool search_match)
{
    if (use_color())
    {
        int pair;
        if (state == CELL_DRAW_EDITING)
        {
            pair = COLOR_PAIR_EDITING;
        }
        else if (state == CELL_DRAW_SELECTED)
        {
            pair = COLOR_PAIR_SELECTED;
        }
        else
        {
            pair = (abs_x + abs_y) % 2 == 0 ? COLOR_PAIR_ODD : COLOR_PAIR_EVEN;
        }
        chtype attrs = COLOR_PAIR(pair);
        if (search_match && state == CELL_DRAW_NORMAL)
        {
            attrs |= A_UNDERLINE;
        }
        return attrs;
    }

    if (state == CELL_DRAW_EDITING || state == CELL_DRAW_SELECTED)
    {
        return A_REVERSE;
    }
    return A_NORMAL;
}

static void fill_cell_area(int py, int px, int cell_h, int cell_w, chtype attrs)
{
    wattrset(grid_window, attrs);
    for (int row = 0; row < cell_h; ++row)
    {
        wmove(grid_window, py + row, px);
        whline(grid_window, ' ', cell_w);
    }
    wattrset(grid_window, A_NORMAL);
}

void matrix_presentation_draw_cell(const coordinates_t *viewport_pos,
                                   size_t abs_x,
                                   size_t abs_y,
                                   const char *data,
                                   cell_draw_state_t state,
                                   bool search_match,
                                   size_t edit_cursor_pos,
                                   const matrix_config_t *config,
                                   const matrix_properties_t *properties)
{
    if (!grid_window || !config || !viewport_pos)
    {
        return;
    }

    coordinates_t position;
    calculate_offsets(viewport_pos, config, properties, &position);

    int cell_h = (int)(properties->cell_padding_top + config->line_height[viewport_pos->y] + properties->cell_padding_bottom);
    int cell_w = (int)(properties->cell_padding_left + config->column_width[viewport_pos->x] + properties->cell_padding_right);
    if (cell_h < 1 || cell_w < 1)
    {
        return;
    }

    int py = position.y;
    int px = position.x;

    chtype attrs = cell_display_attrs(abs_x, abs_y, state, search_match);

    if (use_color() || state != CELL_DRAW_NORMAL)
    {
        fill_cell_area(py, px, cell_h, cell_w, attrs);
    }

    wattrset(grid_window, attrs);
    wmove(grid_window, py + (int)properties->cell_padding_top, px + (int)properties->cell_padding_left);
    if (data)
    {
        wprintw(grid_window, "%.*s", (int)config->column_width[viewport_pos->x], data);
    }
    wattrset(grid_window, A_NORMAL);

    if (display_options.grid_enabled && viewport_pos->x > 0)
    {
        mvwvline(grid_window, py, px - 1, '|', cell_h);
    }

    if (state == CELL_DRAW_EDITING && grid_window)
    {
        int cursor_screen_y = properties->margin_top + py + (int)properties->cell_padding_top;
        int cursor_screen_x = properties->margin_left + px + (int)properties->cell_padding_left + (int)edit_cursor_pos;
        matrix_presentation_show_cell_cursor(cursor_screen_y, cursor_screen_x);
    }
}

void matrix_presentation_show_cell_cursor(int screen_y, int screen_x)
{
    curs_set(1);
    move(screen_y, screen_x);
}

void matrix_presentation_hide_cursor(void)
{
    curs_set(0);
}

void matrix_presentation_run(matrix_key_handler_t handler)
{
    if (!handler)
    {
        return;
    }

    screen_size_t last_size = configuration;
    (void)handler(0);
    matrix_presentation_refresh_partial();

    while (running)
    {
        int key = getch();
        if (key == ERR)
        {
            screen_size_t prev = last_size;
            matrix_presentation_get_screen_size();
            if (configuration.width != prev.width || configuration.height != prev.height)
            {
                last_size = configuration;
                paint_action_t action = handler(0);
                if (action != PAINT_NONE)
                {
                    matrix_presentation_refresh_partial();
                }
            }
            continue;
        }

        paint_action_t action = handler(key);
        if (action != PAINT_NONE)
        {
            matrix_presentation_refresh_partial();
        }
    }
}

void matrix_presentation_exit(void)
{
    destroy_grid_window();
    endwin();
}
