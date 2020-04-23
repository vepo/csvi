#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "actions.h"
#include "csv-reader.h"
#include "matrix-presentation.h"
#include "matrix-config.h"
#include "helper.h"

screen_config_t current_screen = {.width = 1,
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
size_t selection_x = 0;
size_t selection_y = 0;

size_t top_x = 0;
size_t top_y = 0;

void up()
{
    if (top_y > 0 && selection_y == top_y)
    {
        top_y--;
    }

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

void left()
{
    if (top_x > 0 && selection_x == top_x)
    {
        top_x--;
    }

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

void right()
{
    if (top_x < open_file->columns - 1 && selection_x == current_screen.width + top_x)
    {
        top_x++;
    }

    if (selection_x < open_file->columns - 1)
    {
        selection_x++;
        matrix_presentation_flash();
    }
    else
    {
        matrix_presentation_beep();
    }
}

void down()
{
    if (top_y < open_file->lines && selection_y + 1 >= current_screen.height + top_y)
    {
        top_y++;
    }

    if (selection_y < open_file->lines - 1)
    {
        selection_y++;
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
    csv_token *token = csv_reader_get_token(top_x, top_y, open_file);
    matrix_config_get_most_expanded(scr_config, &m_properties, token, open_file->columns, open_file->lines, &current_screen);
    matrix_config_t *config = matrix_config_initialize(current_screen.width, current_screen.height);
    matrix_config_load_sizes(token, config);
    while (token)
    {
        if (token->x >= top_x && token->x < top_x + current_screen.width &&
            token->y >= top_y && token->y < top_y + current_screen.height)
        {
            coordinates_t position = {.x = token->x - top_x, .y = token->y - top_y};
            LOGGER_INFO("SET (%ld, %ld)\n", position.x, position.y)
            matrix_presentation_set_value(&position, token->data, token->x == selection_x && token->y == selection_y, config, &m_properties);
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
