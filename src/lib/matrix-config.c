#include "matrix-config.h"

#include <stdlib.h>
#include <string.h>
#include "helper.h"

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

bool can_show(const screen_size_t *available,
              const matrix_properties_t *properties,
              const size_t *widths,
              const size_t *heights,
              const screen_size_t *possible)
{
    int available_width = available->width - properties->margin_right - properties->margin_left;
    for (size_t index = 0; index < possible->width && available_width >= 0; ++index)
    {
        available_width -= widths[index] + properties->cell_padding_left + properties->cell_padding_right;
    }

    int available_height = available->height - properties->margin_top - properties->margin_bottom;
    for (size_t index = 0; index < possible->height && available_height >= 0; ++index)
    {
        available_height -= heights[index] + properties->cell_padding_top + properties->cell_padding_bottom;
    }
    return available_width >= 0 && available_height >= 0;
}

bool can_fit(const screen_size_t *available,
             const matrix_properties_t *properties,
             csv_token *start_token,
             const screen_size_t *possible)
{
    size_t widths[possible->width];
    size_t heights[possible->height];
    matrix_config_t config = {.columns = possible->width,
                              .heights = possible->height,
                              .column_width = widths,
                              .line_height = heights};
    matrix_config_load_sizes(start_token, &config);
    return can_show(available, properties, widths, heights, possible);
}

void matrix_config_get_most_expanded(const screen_size_t *available,
                                     const matrix_properties_t *properties,
                                     csv_token *start_token,
                                     size_t max_columns,
                                     size_t max_lines,
                                     screen_size_t *used)
{
    bool can_grow_xy = true;
    bool can_grow_x = true;
    bool can_grow_y = true;
    while (can_grow_xy || can_grow_x || can_grow_y)
    {
        if (can_grow_xy)
        {
            screen_size_t next = {
                .height = used->height + 1,
                .width = used->width + 1};

            if (next.width + start_token->x <= max_columns &&
                next.height + start_token->y <= max_lines &&
                can_fit(available, properties, start_token, &next))
            {
                used->height = next.height;
                used->width = next.width;
            }
            else
            {
                can_grow_xy = false;
            }
        }
        else if (can_grow_y)
        {
            screen_size_t next = {
                .height = used->height + 1,
                .width = used->width};

            if (next.width + start_token->x <= max_columns &&
                next.height + start_token->y <= max_lines &&
                can_fit(available, properties, start_token, &next))
            {
                used->height = next.height;
                used->width = next.width;
            }
            else
            {
                can_grow_y = false;
            }
        }
        else if (can_grow_x)
        {
            screen_size_t next = {
                .height = used->height,
                .width = used->width + 1};

            if (next.width + start_token->x <= max_columns &&
                next.height + start_token->y <= max_lines &&
                can_fit(available, properties, start_token, &next))
            {
                used->height = next.height;
                used->width = next.width;
            }
            else
            {
                can_grow_x = false;
            }
        }
    }
}

void matrix_config_load_sizes(csv_token *start_token, matrix_config_t *config)
{
    // clean the values
    memset(config->column_width, 0, sizeof(size_t) * config->columns);
    memset(config->line_height, 0, sizeof(size_t) * config->heights);

    csv_token *curr_token = start_token;
    while (curr_token &&
           (curr_token->x >= start_token->x ||
            curr_token->x < start_token->x + config->columns ||
            curr_token->y < start_token->y + config->heights))
    {
        if (curr_token->x >= start_token->x &&
            curr_token->x < start_token->x + config->columns &&
            curr_token->y < start_token->y + config->heights)
        {
            cell_info_t cell_info = {.height = 1, .width = 0};
            matrix_config_load_cell_info(curr_token->data, &cell_info);

            size_t offset_x = curr_token->x - start_token->x;
            if (config->column_width[offset_x] < cell_info.width)
            {
                config->column_width[offset_x] = cell_info.width;
            }

            size_t offset_y = curr_token->y - start_token->y;
            if (config->line_height[offset_y] < cell_info.height)
            {
                config->line_height[offset_y] = cell_info.height;
            }
        }

        curr_token = curr_token->next;
    }
}