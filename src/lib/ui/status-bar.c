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

void status_bar_draw_edit_preview(int screen_row,
                                  int screen_width,
                                  const char *text,
                                  const char *original,
                                  size_t cursor)
{
    if (screen_width <= 0)
    {
        return;
    }

    const char *value = text ? text : "";
    const char *orig = original ? original : "";
    size_t len = strlen(value);
    size_t orig_len = strlen(orig);

    char header[48];
    if (len != orig_len || strcmp(value, orig) != 0)
    {
        snprintf(header, sizeof(header), " EDIT %zu/%zu* ", cursor + 1, len);
    }
    else
    {
        snprintf(header, sizeof(header), " EDIT %zu/%zu ", cursor + 1, len);
    }

    int header_len = (int)strlen(header);
    int content_width = screen_width - header_len;
    if (content_width < 8)
    {
        content_width = 8;
    }

    size_t offset = 0;
    if (cursor >= (size_t)content_width)
    {
        offset = cursor - (size_t)(content_width / 3);
    }
    if (offset + (size_t)content_width > len)
    {
        offset = len > (size_t)content_width ? len - (size_t)content_width : 0;
    }

    move(screen_row, 0);
    clrtoeol();
    attron(A_REVERSE);
    printw("%s", header);
    attroff(A_REVERSE);

    int col = header_len;
    bool truncated_left = offset > 0;
    if (truncated_left)
    {
        mvaddch(screen_row, col++, '<');
    }

    int max_chars = content_width - (truncated_left ? 1 : 0);
    bool truncated_right = len > offset + (size_t)max_chars;
    if (truncated_right)
    {
        max_chars--;
    }

    size_t show_len = len - offset;
    if (show_len > (size_t)max_chars)
    {
        show_len = (size_t)max_chars;
    }

    for (size_t i = 0; i < show_len; ++i)
    {
        size_t abs = offset + i;
        char ch = value[abs];
        bool changed = abs >= orig_len || value[abs] != orig[abs];

        int attrs = A_NORMAL;
        if (abs == cursor)
        {
            attrs = A_REVERSE;
        }
        else if (changed)
        {
            attrs = A_UNDERLINE;
        }

        attron(attrs);
        mvaddch(screen_row, col + (int)i, ch);
        attroff(attrs);
    }

    if (truncated_right)
    {
        mvaddch(screen_row, col + (int)show_len, '>');
    }

    if (cursor >= offset && cursor < offset + show_len)
    {
        int cursor_col = col + (int)(cursor - offset);
        move(screen_row, cursor_col);
        curs_set(1);
    }
    else
    {
        curs_set(0);
    }
}
