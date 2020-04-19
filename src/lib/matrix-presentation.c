#include "matrix-presentation.h"

#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include "helper.h"
#include "logger.h"

screen_config_t configuration;

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

    noecho();
    curs_set(0);
    timeout(0);
    cbreak();
    nodelay(stdscr, true);

    getmaxyx(stdscr, configuration.height, configuration.width);
    rectangle(0, 0, configuration.height - 2, configuration.width - 1);
}

screen_config_t *matrix_presentation_get_screen_config()
{
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

void matrix_presentation_handle()
{
    while (true)
    {
        mp_repeaint();
        char command[1024];
        size_t command_length = 0;
        command[command_length] = '\0';
        char key = 0;
        key = getch();
        void (*handler)() = NULL;
        command[command_length++] = key;
        if (strcmp(command, "\033A") == 0)
        {
            // code for arrow up
            handler = matrix_presentation_get_handler(UP);
        }
        else if (strcmp(command, "\033B") == 0)
        {
            // code for arrow down
            handler = matrix_presentation_get_handler(DOWN);
        }
        else if (strcmp(command, "\033C") == 0)
        {
            // code for arrow right
            handler = matrix_presentation_get_handler(RIGHT);
        }
        else if (strcmp(command, "\033D") == 0)
        {
            // code for arrow left
            handler = matrix_presentation_get_handler(LEFT);
        }

        if (handler)
        {
            (*handler)();
        }
    }
}

size_t calculate_offset(size_t pos, size_t *sizes)
{
    size_t curr = 0;
    size_t offset = 0;
    //log_info("Calculating: pos=%d...\n", pos);
    while (curr < pos)
    {
        //  log_info("Add offset offset=%d size=%d\n", offset, sizes[curr]);
        offset += sizes[curr] + 1;
        curr++;
    }
    //log_info("Returning... offset=%d\n", offset);
    return offset;
}

void matrix_presentation_set_value(size_t x, size_t y, char *data, matrix_config_t *config)
{
    CHECK_FATAL_FN(!config, "Matrix no configured!\n", matrix_presentation_exit);
    size_t offset_top = calculate_offset(y, config->line_height);
    size_t offset_left = calculate_offset(x, config->column_width);
    //log_info("x=%d y=%d top=%d left=%d\n", x, y, offset_top, offset_left);
    mvprintw(offset_top + 1, offset_left + 1, data);
}

void matrix_presentation_exit()
{
    endwin();
}