#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "actions.h"
#include "csv-reader.h"
#include "matrix-presentation.h"
#include "matrix-config.h"
#include "helper.h"

screen_config_t last_screen = {.width = 1,
                               .height = 1};

matrix_properties_t m_properties = {.cell_padding_top = 0,
                                    .cell_padding_right = 2,
                                    .cell_padding_bottom = 0,
                                    .cell_padding_left = 1,
                                    .margin_top = 1,
                                    .margin_right = 1,
                                    .margin_bottom = 2,
                                    .margin_left = 1};
csv_contents *open_file = NULL;

coordinates_t top_cell = {.x = 0,
                          .y = 0};

coordinates_t selected_cell = {.x = 0,
                               .y = 0};

void up()
{
    if (top_cell.y > 0 && selected_cell.y == top_cell.y)
    {
        top_cell.y--;
    }

    if (selected_cell.y > 0)
    {
        selected_cell.y--;
        matrix_presentation_set_selected(&selected_cell);
        matrix_presentation_flash();
    }
    else
    {
        matrix_presentation_beep();
    }
}

void left()
{
    if (top_cell.x > 0 && selected_cell.x == top_cell.x)
    {
        top_cell.x--;
    }

    if (selected_cell.x > 0)
    {
        selected_cell.x--;
        matrix_presentation_set_selected(&selected_cell);
        matrix_presentation_flash();
    }
    else
    {
        matrix_presentation_beep();
    }
}

void right()
{
    if (top_cell.x < open_file->columns - 1 && last_screen.width + top_cell.x - 1 == selected_cell.x + 1)
    {
        top_cell.x++;
    }

    if (selected_cell.x < open_file->columns - 1)
    {
        selected_cell.x++;
        matrix_presentation_set_selected(&selected_cell);
        matrix_presentation_flash();
    }
    else
    {
        matrix_presentation_beep();
    }
}

void down()
{
    if (top_cell.y < open_file->lines && selected_cell.y + 1 >= last_screen.height + top_cell.y)
    {
        top_cell.y++;
    }

    if (selected_cell.y < open_file->lines - 1)
    {
        selected_cell.y++;
        matrix_presentation_set_selected(&selected_cell);
        matrix_presentation_flash();
    }
    else
    {
        matrix_presentation_beep();
    }
}

void paint()
{
    screen_config_t *scr_config = matrix_presentation_get_screen_config();
    csv_token *token = csv_reader_get_token(top_cell.x, top_cell.y, open_file);
    screen_config_t current_screen = {.width = 1,
                                      .height = 1};
    matrix_config_get_most_expanded(scr_config, &m_properties, token, open_file->columns, open_file->lines, &current_screen);

    if (current_screen.height != last_screen.height || current_screen.width != last_screen.width)
    {
        matrix_presentation_refresh(&m_properties);
        last_screen.height = current_screen.height;
        last_screen.width = current_screen.width;
        matrix_presentation_set_selected(&selected_cell);
    }

    if (selected_cell.x == last_screen.width + top_cell.x)
    {
        top_cell.x += 2;
        matrix_presentation_refresh(&m_properties);
        matrix_presentation_set_selected(&selected_cell);
        token = csv_reader_get_token(top_cell.x, top_cell.y, open_file);
        current_screen.width = 1;
        current_screen.height = 1;
        matrix_config_get_most_expanded(scr_config, &m_properties, token, open_file->columns, open_file->lines, &current_screen);

        if (current_screen.height != last_screen.height || current_screen.width != last_screen.width)
        {
            matrix_presentation_refresh(&m_properties);
            matrix_presentation_set_selected(&selected_cell);
            last_screen.height = current_screen.height;
            last_screen.width = current_screen.width;
        }
    }

    matrix_config_t *config = matrix_config_initialize(current_screen.width, current_screen.height);
    matrix_config_load_sizes(token, config);
    while (token)
    {
        if (token->x >= top_cell.x && token->x < top_cell.x + current_screen.width &&
            token->y >= top_cell.y && token->y < top_cell.y + current_screen.height)
        {
            coordinates_t position = {.x = token->x - top_cell.x,
                                      .y = token->y - top_cell.y};
            matrix_presentation_set_value(&position, token->data, token->x == selected_cell.x && token->y == selected_cell.y, config, &m_properties);
        }
        else if (token->x > top_cell.x + current_screen.width &&
                 token->y > top_cell.y + current_screen.height)
        {
            break;
        }
        token = token->next;
    }

    matrix_config_dispose(config);
}

int main(int argc, char *argv[])
{
    CHECK_FATAL(argc != 2, "No file to read...\n");

    open_file = csv_reader_read_file(argv[1]);
    csv_token *curr = open_file->first;
    while (curr)
    {
        curr = curr->next;
    }

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
