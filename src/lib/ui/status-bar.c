#include "ui/status-bar.h"

#include <curses.h>
#include <stdio.h>
#include <string.h>

static int bar_row = 0;
static int bar_width = 0;

void status_bar_init(int screen_width, int screen_row)
{
    bar_width = screen_width;
    bar_row = screen_row;
}

void status_bar_resize(int screen_width, int screen_row)
{
    bar_width = screen_width;
    bar_row = screen_row;
}

static const char *mode_label(input_mode_t mode)
{
    switch (mode)
    {
    case INPUT_COMMAND:
        return "COMMAND";
    case INPUT_SEARCH:
        return "SEARCH";
    case INPUT_HELP:
        return "HELP";
    case INPUT_INSERT:
        return "INSERT";
    default:
        return "NORMAL";
    }
}

void status_bar_draw(const char *filename,
                     size_t row,
                     size_t total_rows,
                     size_t col,
                     size_t total_cols,
                     char separator,
                     input_mode_t mode,
                     bool file_modified,
                     const char *line_text,
                     const char *message)
{
    if (bar_width <= 0)
    {
        return;
    }

    move(bar_row, 0);
    clrtoeol();
    attron(A_REVERSE);
    char left[160];
    const char *name = filename ? filename : "-";
    if (file_modified)
    {
        snprintf(left,
                 sizeof(left),
                 " %s* | row %zu/%zu | col %zu/%zu | sep %c | %s ",
                 name,
                 row + 1,
                 total_rows,
                 col + 1,
                 total_cols,
                 separator,
                 mode_label(mode));
    }
    else
    {
        snprintf(left,
                 sizeof(left),
                 " %s | row %zu/%zu | col %zu/%zu | sep %c | %s ",
                 name,
                 row + 1,
                 total_rows,
                 col + 1,
                 total_cols,
                 separator,
                 mode_label(mode));
    }
    printw("%s", left);
    attroff(A_REVERSE);

    if (message && message[0])
    {
        mvprintw(bar_row, (int)strlen(left), "| %s", message);
    }
    else if (line_text && line_text[0])
    {
        int max = bar_width - (int)strlen(left) - 2;
        if (max > 0)
        {
            mvprintw(bar_row, (int)strlen(left), "| %.*s", max, line_text);
        }
    }
}

void status_bar_draw_input(input_mode_t mode, const char *prefix, const char *input)
{
    if (bar_width <= 0)
    {
        return;
    }

    move(bar_row, 0);
    clrtoeol();
    attron(A_REVERSE);
    const char *label = mode == INPUT_SEARCH ? "SEARCH" : "COMMAND";
    printw(" %s ", label);
    attroff(A_REVERSE);
    printw("%s%s", prefix ? prefix : "", input ? input : "");
}
