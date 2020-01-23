#include "matrix-presentation.h"

#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include "helper.h"

screen_config_t configuration;

typedef struct actions_config
{
    Action action;
    void *callback;
    struct actions_config *next;
} actions_config;

actions_config *INITIAL_ACTION = NULL;

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

void matrix_presentation_set_value(size_t x, size_t y, char *data, matrix_config_t *config)
{
    CHECK_FATAL_FN(!config, "Matrix no configured!\n", matrix_presentation_exit);
    mvprintw(0, 0, "0,0");
    mvprintw(configuration.height - 1, 0, "y");
    mvprintw(0, configuration.width - 1, "x");
}

void matrix_presentation_exit()
{
    endwin();
}