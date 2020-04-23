#include "matrix-presentation.h"

#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include "helper.h"

screen_config_t configuration = {.width = 0, .height = 0};

typedef struct actions_config
{
    Action action;
    void *callback;
    struct actions_config *next;
} actions_config;

actions_config *INITIAL_ACTION = NULL;

void rectangle(int y1, int x1, int y2, int x2)
{
    mvhline(y1, x1, 0, x2 - x1);
    mvhline(y2, x1, 0, x2 - x1);
    mvvline(y1, x1, 0, y2 - y1);
    mvvline(y1, x2, 0, y2 - y1);
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
}

void matrix_presentation_init()
{
    initscr();
    start_color();

    init_pair(SELECTED_CELL, COLOR_BLACK, COLOR_GREEN);
    init_pair(EVEN_CELL, COLOR_BLACK, COLOR_WHITE);
    init_pair(ODD_CELL, COLOR_WHITE, COLOR_BLACK);

    noecho();
    curs_set(0);
    timeout(0);
    cbreak();
    keypad(stdscr, true);
    nodelay(stdscr, true);
}

screen_config_t *matrix_presentation_get_screen_config()
{
    screen_config_t curr_config;
    getmaxyx(stdscr, curr_config.height, curr_config.width);
    if (curr_config.width != configuration.width || curr_config.height != configuration.height)
    {
        configuration.width = curr_config.width;
        configuration.height = curr_config.height;
    }

    return &configuration;
}

void *matrix_presentation_get_handler(Action action)
{
    actions_config *ac = INITIAL_ACTION;
    while (ac)
    {
        if (ac->action == action)
        {
            return ac->callback;
        }
        ac = ac->next;
    }
    return NULL;
}

void matrix_presentation_beep()
{
    beep();
}

void matrix_presentation_flash()
{
    flash();
}

void matrix_presentation_configure_handler(Action action, void (*callback)())
{
    actions_config *aconfig = (actions_config *)malloc(sizeof(actions_config));
    aconfig->action = action;
    aconfig->callback = callback;
    aconfig->next = NULL;
    if (INITIAL_ACTION)
    {
        actions_config *last_action = INITIAL_ACTION;
        while (last_action->next)
        {
            last_action = last_action->next;
        }
        last_action->next = aconfig;
    }
    else
    {
        INITIAL_ACTION = aconfig;
    }
}

void mp_repeaint()
{
    void (*handler)() = matrix_presentation_get_handler(PAINT);
    if (handler)
    {
        handler();
        refresh();
    }
}

void matrix_presentation_refresh(matrix_properties_t *m_properties)
{
    clear();
    rectangle(0, 0, configuration.height - m_properties->margin_bottom, configuration.width - m_properties->margin_right);
}

void matrix_presentation_handle()
{
    mp_repeaint();
    while (true)
    {
        void (*handler)() = NULL;
        switch (getch())
        {
        case KEY_UP:
            // code for arrow up
            handler = matrix_presentation_get_handler(UP);
            LOGGER_INFO("Detected: KEY UP\n");
            break;
        case KEY_DOWN:
            // code for arrow down
            handler = matrix_presentation_get_handler(DOWN);
            LOGGER_INFO("Detected: KEY DOWN\n");
            break;
        case KEY_RIGHT:
            // code for arrow right
            handler = matrix_presentation_get_handler(RIGHT);
            LOGGER_INFO("Detected: KEY RIGHT\n");
            break;
        case KEY_LEFT:
            // code for arrow left
            handler = matrix_presentation_get_handler(LEFT);
            LOGGER_INFO("Detected: KEY LEFT\n");
            break;
        default:
            break;
        }

        if (handler)
        {
            (*handler)();
            mp_repeaint();
        }
    }
}

void calculate_offsets(coordinates_t *cell,
                       matrix_config_t *config,
                       matrix_properties_t *m_properties,
                       coordinates_t *position)
{
    size_t curr_x = 0;
    position->x = m_properties->margin_left;
    size_t cell_horizontal_padding = m_properties->cell_padding_left + m_properties->cell_padding_right;
    size_t cell_vertical_padding = m_properties->cell_padding_top + m_properties->cell_padding_bottom;
    while (curr_x < cell->x)
    {
        position->x += cell_horizontal_padding + config->column_width[curr_x];
        ++curr_x;
    }

    size_t curr_y = 0;
    position->y = m_properties->margin_top;
    while (curr_y < cell->y)
    {
        position->y += cell_vertical_padding + config->line_height[curr_y];
        ++curr_y;
    }
}

void matrix_presentation_set_value(coordinates_t *cell,
                                   char *data,
                                   bool selected,
                                   matrix_config_t *config,
                                   matrix_properties_t *m_properties)
{
    CHECK_FATAL_FN(!config, "Matrix not configured!\n", matrix_presentation_exit);
    coordinates_t position;
    calculate_offsets(cell, config, m_properties, &position);

    //LOGGER_INFO("Wrinting (%ld, %ld) in (%ld, %ld) with (%ld, %ld)\n", cell->x, cell->y, position.x, position.y, config->column_width[cell->x], config->line_height[cell->y]);

    WINDOW *cell_scr = subwin(stdscr,
                              m_properties->cell_padding_top + config->line_height[cell->y] + m_properties->cell_padding_bottom,
                              m_properties->cell_padding_left + config->column_width[cell->x] + m_properties->cell_padding_right,
                              position.y,
                              position.x);
    wclear(cell_scr);
    if (selected)
    {
        wbkgd(cell_scr, COLOR_PAIR(SELECTED_CELL));
    }
    else if (cell->x % 2 == 0)
    {

        if (cell->y % 2 == 0)
        {
            wbkgd(cell_scr, COLOR_PAIR(ODD_CELL));
        }
        else
        {
            wbkgd(cell_scr, COLOR_PAIR(EVEN_CELL));
        }
    }
    else
    {
        if (cell->y % 2 == 0)
        {
            wbkgd(cell_scr, COLOR_PAIR(EVEN_CELL));
        }
        else
        {
            wbkgd(cell_scr, COLOR_PAIR(ODD_CELL));
        }
    }

    cell_info_t cell_info;
    matrix_config_load_cell_info(data, &cell_info);
    if (cell_info.height == 1)
    {
        mvwprintw(cell_scr, m_properties->cell_padding_top, m_properties->cell_padding_left, data);
    }
    else if (cell_info.height > 1)
    {
        size_t start_line = 0;
        for (size_t line_index = 0; line_index < cell_info.height; ++line_index)
        {
            char buffer[strlen(data)];
            size_t end_line = 0;
            while (data[start_line + end_line] != '\n')
            {
                buffer[end_line] = data[start_line + end_line];
                ++end_line;
            }
            buffer[end_line] = '\0';

            mvwprintw(cell_scr, m_properties->cell_padding_top + line_index, m_properties->cell_padding_left, buffer);
            start_line = end_line + 1;
        }
    }
    delwin(cell_scr);
}

void matrix_presentation_set_selected(coordinates_t *cell)
{
    WINDOW *pos_scr = subwin(stdscr, 1, 9, configuration.height - 1, configuration.width - 10);
    char str[11];
    sprintf(str, "%03ld x %03ld", cell->x, cell->y);
    mvwprintw(pos_scr, 0, 0, str);
    delwin(pos_scr);
}

void matrix_presentation_exit()
{
    endwin();
}