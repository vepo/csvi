#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "actions.h"
#include "csv-reader.h"
#include "matrix-presentation.h"

csv_contents *open_file = NULL;
size_t selection_x = 0;
size_t selection_y = 0;

size_t top_x = 0;
size_t top_y = 0;

void up()
{
    if (selection_x > 0)
    {
        selection_x--;
        matrix_presentation_flash();
    }
    else
    {
        matrix_presentation_beep();
    }
}

void left()
{
    if (selection_y > 0)
    {
        selection_y--;
        matrix_presentation_flash();
    }
    else
    {
        matrix_presentation_beep();
    }
}

void right()
{
    if (selection_y < open_file->columns)
    {
        selection_y++;
        matrix_presentation_flash();
    }
    else
    {
        matrix_presentation_beep();
    }
}

void down()
{
    if (selection_y < open_file->lines)
    {
        selection_y++;
        matrix_presentation_flash();
    }
    else
    {
        matrix_presentation_beep();
    }
}
size_t str_count_lines(char *data)
{
    size_t len = strlen(data);
    size_t pos = 0;
    size_t lines = 1;
    while (pos < len)
    {
        if (data[pos] == '\n')
        {
            lines++;
        }
    }
    return lines;
}

typedef struct matrix_info
{
    size_t *column_width;
    size_t *line_height;
} matrix_info;

matrix_info *matrix_info_initialize(size_t width, size_t height)
{
    matrix_info *info = (matrix_info *)malloc(sizeof(matrix_info));
    info->column_width = (size_t *)calloc(width, sizeof(size_t));
    info->line_height = (size_t *)calloc(height, sizeof(size_t));
    return info;
}

void matrix_info_load(size_t width, size_t height, csv_token *start_token, matrix_info *info)
{
    csv_token *curr_token = start_token;
    if (curr_token)
    {
        if (curr_token->x >= start_token->x && curr_token->x < start_token->x + width)
        {
            if (info->column_width[curr_token->x - start_token->x] < strlen(curr_token->data))
            {
                info->column_width[curr_token->x - start_token->x] = strlen(curr_token->data);
            }
        }

        if (curr_token->y >= start_token->y && curr_token->y < start_token->y + height)
        {
            if (info->line_height[curr_token->y - start_token->y] < str_count_lines(curr_token->data))
            {
                info->line_height[curr_token->y - start_token->y] = str_count_lines(curr_token->data);
            }
        }
        curr_token = curr_token->next;
    }
}

void matrix_info_dispose(matrix_info *info)
{
    free(info->column_width);
    free(info->line_height);
    free(info);
}

bool can_show(csv_token *start_token, size_t width, size_t height, screen *screen)
{
    matrix_info *info = matrix_info_initialize(width, height);
    matrix_info_load(width, height, start_token, info);

    size_t required_width = 0;
    size_t required_height = 0;

    for (size_t i = 0; i < width; ++i)
    {
        required_width += info->column_width[i];
    }

    for (size_t i = 0; i < width; ++i)
    {
        required_height += info->line_height[i];
    }
    matrix_info_dispose(info);
    return required_width <= screen->width && required_height <= screen->height;
}
void expand_view(screen *available, csv_token *start_token, screen *used)
{
}
void paint()
{
    screen *scr = matrix_presentation_get_screen();
    csv_token *curr_token = csv_reader_get_token(top_x, top_y, open_file);
    screen curr = {.width = 1, .height = 1};

    bool can_grow_x = true, can_grow_y = true;
    while (can_grow_x && can_grow_y)
    {
        if (can_grow_x && can_grow_y)
        {
            if (can_show(curr_token, curr.width + 1, curr.height + 1, scr))
            {
                curr.width++;
                curr.height++;
            }
        }
        else if (can_grow_x)
        {
            if (can_show(curr_token, curr.width + 1, curr.height, scr))
            {
                curr.width++;
            }
            else
            {
                can_grow_x = false;
            }
        }
        else
        {
            if (can_show(curr_token, curr.width, curr.height + 1, scr))
            {
                curr.height++;
            }
            else
            {
                can_grow_y = false;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "No file to read...");
        exit(1);
    }
    open_file = csv_reader_read_file(argv[1]);
    matrix_presentation_init();
    matrix_presentation_configure_handler(UP, &up);
    matrix_presentation_configure_handler(LEFT, &left);
    matrix_presentation_configure_handler(RIGHT, &right);
    matrix_presentation_configure_handler(DOWN, &down);
    matrix_presentation_configure_handler(PAINT, &paint);
    matrix_presentation_handle();
    matrix_presentation_exit();
    return 0;
}
