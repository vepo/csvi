#include "matrix-presentation.h"

#include <stdlib.h>
#include <string.h>
#include <curses.h>

struct screen
{
    int x;
    int y;
};

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

    struct screen field;
    getmaxyx(stdscr, field.y, field.x);
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

void matrix_presentation_handle()
{
    char command[1024];
    size_t command_length = 0;
    command[command_length] = '\0';
    char key = 0;
    key = getch();
    void (*handler)() = NULL;
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

void matrix_presentation_exit()
{
    endwin();
}