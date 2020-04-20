#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "actions.h"
#include "csv-reader.h"
#include "matrix-presentation.h"
#include "matrix-config.h"
#include "helper.h"
#include "logger.h"

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

void paint()
{
    screen_config_t *scr_config = matrix_presentation_get_screen_config();
    matrix_properties_t m_properties = {.cell_padding_top = 0,
                                        .cell_padding_right = 2,
                                        .cell_padding_bottom = 0,
                                        .cell_padding_left = 1,
                                        .margin_top = 1,
                                        .margin_right = 1,
                                        .margin_bottom = 2,
                                        .margin_left = 1};
    csv_token *token = csv_reader_get_token(top_x, top_y, open_file);
    screen_config_t curr = {.width = 1, .height = 1};
    matrix_config_get_most_expanded(scr_config, &m_properties, token, open_file->columns, open_file->lines, &curr);
    matrix_config_t *config = matrix_config_initialize(curr.width, curr.height);
    matrix_config_load(curr.width, curr.height, token, config);
    //log_info("Initializing...\n");
    while (token)
    {
        //  log_info("Token (%d, %d)\n", token->x, token->y);
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
    csv_token *curr = open_file->first;
    while (curr)
    {
        log_info("Contents: (%d, %d) \"%d\"\n", curr->x, curr->y, curr->data);
        curr = curr->next;
    }

    matrix_presentation_init();
    //matrix_presentation_configure_handler(UP, &up);
    //matrix_presentation_configure_handler(LEFT, &left);
    //matrix_presentation_configure_handler(RIGHT, &right);
    //matrix_presentation_configure_handler(DOWN, &down);
    //matrix_presentation_configure_handler(PAINT, &paint);
    paint();
    matrix_presentation_handle();
    matrix_presentation_exit();
    return 0;
}
