#include "matrix-config.h"

#include <stdlib.h>
#include <string.h>

#include "logger.h"

matrix_config_t *matrix_config_initialize(size_t width, size_t height)
{
    matrix_config_t *config = (matrix_config_t *)malloc(sizeof(matrix_config_t));
    config->column_width = (size_t *)calloc(width, sizeof(size_t));
    config->line_height = (size_t *)calloc(height, sizeof(size_t));
    return config;
}

void matrix_config_dispose(matrix_config_t *config)
{
    free(config->column_width);
    free(config->line_height);
    free(config);
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

void matrix_config_get_most_expanded(screen_config_t *available, csv_token *start_token, size_t max_lines, size_t max_columns, screen_config_t *used)
{
    bool can_grow_x = true, can_grow_y = true;
    while (can_grow_x || can_grow_y)
    {
        if (can_grow_x && can_grow_y)
        {
            if (can_show(start_token, used->width + 1, used->height + 1, available) &&
                used->width < max_columns &&
                used->height < max_lines)
            {
                used->width++;
                used->height++;
            }
        }

        if (can_grow_x)
        {
            if (can_show(start_token, used->width + 1, used->height, available) &&
                used->width < max_columns)
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
                used->height < max_lines)
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
    log_info("Size (%d, %d)\n", width, height);
    csv_token *curr_token = start_token;
    if (curr_token)
    {
        log_info("Curr token (%d, %d)\n", curr_token->x, curr_token->y);
        if (curr_token->x >= start_token->x && curr_token->x < start_token->x + width)
        {
            size_t len = strlen(curr_token->data);
            if (config->column_width[curr_token->x - start_token->x] < len)
            {
                config->column_width[curr_token->x - start_token->x] = len;
                log_info("Updating width[%d]=%d\n", curr_token->x - start_token->x, len);
            }
        }

        if (curr_token->y >= start_token->y && curr_token->y < start_token->y + height)
        {
            size_t len = str_count_lines(curr_token->data);
            if (config->line_height[curr_token->y - start_token->y] < len)
            {
                config->line_height[curr_token->y - start_token->y] = len;
                log_info("Updating height[%d]=%d\n", curr_token->y - start_token->y, len);
            }
        }
        curr_token = curr_token->next;
    }
}