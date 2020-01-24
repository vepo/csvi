#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "actions.h"
#include "csv-reader.h"
#include "matrix-presentation.h"
#include "matrix-config.h"
#include "helper.h"

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
        pos++;
    }
    return lines;
}

void matrix_config_load(size_t width, size_t height, csv_token *start_token, matrix_config_t *config)
{
    csv_token *curr_token = start_token;
    if (curr_token)
    {
        if (curr_token->x >= start_token->x && curr_token->x < start_token->x + width)
        {
            size_t len = strlen(curr_token->data);
            if (config->column_width[curr_token->x - start_token->x] < len)
            {
                config->column_width[curr_token->x - start_token->x] = len;
            }
        }

        if (curr_token->y >= start_token->y && curr_token->y < start_token->y + height)
        {
            size_t len = str_count_lines(curr_token->data);
            if (config->line_height[curr_token->y - start_token->y] < len)
            {
                config->line_height[curr_token->y - start_token->y] = len;
            }
        }
        curr_token = curr_token->next;
    }
}

bool can_show(csv_token *start_token, size_t width, size_t height, screen_config_t *screen)
{
    matrix_config_t *config = matrix_config_initialize(width, height);
    matrix_config_load(width, height, start_token, config);

    size_t required_width = 0;
    size_t required_height = 0;

    for (size_t i = 0; i < width; ++i)
    {
        required_width += config->column_width[i];
    }

    for (size_t i = 0; i < width; ++i)
    {
        required_height += config->line_height[i];
    }
    matrix_config_dispose(config);
    return required_width <= screen->width && required_height <= screen->height;
}

void expand_view(screen_config_t *available, csv_token *start_token, screen_config_t *used)
{
    bool can_grow_x = true, can_grow_y = true;
    while (can_grow_x || can_grow_y)
    {
        if (can_grow_x && can_grow_y)
        {
            if (can_show(start_token, used->width + 1, used->height + 1, available) &&
                used->width < open_file->columns &&
                used->height < open_file->lines)
            {
                used->width++;
                used->height++;
            }
        }

        if (can_grow_x)
        {
            if (can_show(start_token, used->width + 1, used->height, available) &&
                used->width < open_file->columns)
            {
                used->width++;
            }
            else
            {
                can_grow_x = false;
            }
        }
        else
        {
            if (can_show(start_token, used->width, used->height + 1, available) &&
                used->height < open_file->lines)
            {
                used->height++;
            }
            else
            {
                can_grow_y = false;
            }
        }
    }
}

void paint()
{
    screen_config_t *scr_config = matrix_presentation_get_screen_config();
    csv_token *token = csv_reader_get_token(top_x, top_y, open_file);
    screen_config_t curr = {.width = 1, .height = 1};
    expand_view(scr_config, token, &curr);
    matrix_config_t *config = matrix_config_initialize(curr.width, curr.height);
    matrix_config_load(curr.width, curr.height, token, config);

    while (token)
    {
        if (token->x >= top_x && token->x < top_x + curr.width &&
            token->y >= top_y && token->y < top_y + curr.height)
        {
            matrix_presentation_set_value(token->x - top_x, token->y - top_y, token->data, config);
        }
        token = token->next;
    }

    matrix_config_dispose(config);
}

int main(int argc, char *argv[])
{
    CHECK_FATAL(argc != 2, "No file to read...\n");

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
