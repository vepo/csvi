#include "matrix-config.h"

#include <stdlib.h>
#include <string.h>

matrix_config_t *matrix_config_initialize(size_t width, size_t height)
{
    matrix_config_t *config = (matrix_config_t *)malloc(sizeof(matrix_config_t));
    config->columns = width;
    config->heights = height;
    config->column_width = (size_t *)calloc(width, sizeof(size_t));
    config->line_height = (size_t *)calloc(height, sizeof(size_t));
    return config;
}

void matrix_config_dispose(matrix_config_t *config)
{
    if (config)
    {
        free(config->column_width);
        free(config->line_height);
        free(config);
    }
}

void matrix_config_load_cell_info(char *cell_data, cell_info_t *cell_info)
{
    size_t cell_data_len = strlen(cell_data);
    cell_info->height = 1;
    cell_info->width = 0;
    size_t curr_width = 0;
    for (size_t index = 0; index < cell_data_len; ++index)
    {
        if (cell_data[index] == '\n')
        {
            if (cell_info->width < curr_width)
            {
                cell_info->width = curr_width;
            }
            curr_width = 0;
            cell_info->height++;
        }
        else if (cell_data[index] != '\r')
        {
            curr_width++;
        }
    }

    if (cell_info->width < curr_width)
    {
        cell_info->width = curr_width;
    }
}

bool can_show(screen_config_t *available, matrix_properties_t *properties, size_t *widths, size_t *heights, size_t columns, size_t lines)
{
    int available_width = available->width - properties->margin_right - properties->margin_left;
    for (size_t index = 0; index < columns && available_width >= 0; ++index)
    {
        available_width -= widths[index] + properties->cell_padding_left + properties->cell_padding_right;
    }

    int available_height = available->height - properties->margin_top - properties->margin_bottom;
    for (size_t index = 0; index < lines && available_height >= 0; ++index)
    {
        available_height -= heights[index] + properties->cell_padding_top + properties->cell_padding_bottom;
    }
    return available_width >= 0 && available_height >= 0;
}

void matrix_config_get_most_expanded(screen_config_t *available, matrix_properties_t *properties, csv_token *start_token, size_t max_columns, size_t max_lines, screen_config_t *used)
{
    csv_token *curr_token = start_token;
    size_t start_column = start_token->x;
    size_t start_line = start_token->y;
    size_t widths[max_columns - start_column];
    size_t heights[max_lines - start_line];
    for (size_t index = 0; index < (max_columns - start_column); ++index)
    {
        widths[index] = 0;
    }

    for (size_t index = 0; index < (max_lines - start_line); ++index)
    {
        heights[index] = 0;
    }

    while (curr_token)
    {
        if (curr_token->x >= start_column && curr_token->y >= start_line)
        {
            cell_info_t cell_info;
            matrix_config_load_cell_info(curr_token->data, &cell_info);

            size_t offset_x = curr_token->x - start_column;
            if (widths[offset_x] < cell_info.width)
            {
                widths[offset_x] = cell_info.width;
            }

            size_t offset_y = curr_token->y - start_line;
            if (heights[offset_y] < cell_info.height)
            {
                heights[offset_y] = cell_info.height;
            }

            if (offset_x >= used->width || offset_y >= used->height)
            {
                size_t columns = offset_x + 1;
                size_t lines = offset_y + 1;
                if (can_show(available, properties, widths, heights, columns, lines))
                {
                    used->width = columns;
                    used->height = lines;
                }
            }
        }
        curr_token = curr_token->next;
    }
}

void matrix_config_load_sizes(csv_token *start_token, matrix_config_t *config)
{
    csv_token *curr_token = start_token;
    while (curr_token)
    {
        cell_info_t cell_info;
        matrix_config_load_cell_info(curr_token->data, &cell_info);
        if (curr_token->x >= start_token->x && curr_token->x < start_token->x + config->columns)
        {
            if (config->column_width[curr_token->x - start_token->x] < cell_info.width)
            {
                config->column_width[curr_token->x - start_token->x] = cell_info.width;
            }
        }

        if (curr_token->y >= start_token->y && curr_token->y < start_token->y + config->heights)
        {
            if (config->line_height[curr_token->y - start_token->y] < cell_info.height)
            {
                config->line_height[curr_token->y - start_token->y] = cell_info.height;
            }
        }
        curr_token = curr_token->next;
    }
}