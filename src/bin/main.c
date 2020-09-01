#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h> /* for getopt_long; POSIX standard getopt is in unistd.h */
#include <regex.h>

#include "config.h"
#include "actions.h"
#include "csv-reader.h"
#include "matrix-presentation.h"
#include "matrix-config.h"
#include "navigation.h"
#include "helper.h"

screen_size_t last_screen = {.width = 1,
                             .height = 1};
screen_size_t current_screen = {.width = 1,
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

void handle_nagivation(NavigationResult result)
{
    switch (result)
    {
    case CURSOR_UPDATED:
        matrix_presentation_set_selected(&selected_cell);
        matrix_presentation_flash();
        break;
    case BEEP:
        matrix_presentation_beep();
        break;
    default:
        break;
    }
}

void up()
{
    handle_nagivation(navigate_up(&top_cell, &selected_cell));
}

void left()
{
    handle_nagivation(navigate_left(&top_cell, &selected_cell));
}

void right()
{
    handle_nagivation(navigate_right(&top_cell, &selected_cell, &current_screen, open_file->columns));
}

void down()
{
    handle_nagivation(navigate_down(&top_cell, &selected_cell, &current_screen, open_file->lines));
}

void page_up()
{
    handle_nagivation(navigate_page_up(&top_cell, &selected_cell, &current_screen));
}

void page_down()
{
    handle_nagivation(navigate_page_down(&top_cell, &selected_cell, &current_screen, open_file->lines));
}

void home()
{
    handle_nagivation(navigate_page_previous(&top_cell, &selected_cell, &current_screen));
}

void end()
{
    handle_nagivation(navigate_page_next(&top_cell, &selected_cell, &current_screen, open_file->columns));
}

regex_t regex_go_to_line;
regex_t regex_go_to_column;

void init_commands()
{
    regcomp(&regex_go_to_line, "^\\:([[:digit:]]+)$", REG_EXTENDED);
    regcomp(&regex_go_to_column, "^\\:c([[:digit:]]+)$", REG_EXTENDED);
}

void execute_command_go_to_line(char *command, regmatch_t *pmatch)
{
    size_t input_length = pmatch[1].rm_eo - pmatch[1].rm_so;
    char *go_to_line = (char *)malloc(input_length + 1);
    go_to_line[input_length] = '\0';
    strncpy(go_to_line, &command[pmatch[1].rm_so], input_length);
    int line = atoi(go_to_line) - 1; // zero index
    free(go_to_line);
    if (line < open_file->lines)
    {
        top_cell.y = line;
        selected_cell.y = line;
        matrix_presentation_set_selected(&selected_cell);
        matrix_presentation_flash();
    }
    else
    {
        matrix_presentation_beep();
    }
}

void execute_command_go_to_column(char *command, regmatch_t *pmatch)
{
    size_t input_length = pmatch[1].rm_eo - pmatch[1].rm_so;
    char *go_to_column = (char *)malloc(input_length + 1);
    go_to_column[input_length] = '\0';
    strncpy(go_to_column, &command[pmatch[1].rm_so], input_length);
    int column = atoi(go_to_column) - 1; // zero index
    free(go_to_column);
    if (column < open_file->columns)
    {
        top_cell.x = column;
        selected_cell.x = column;
        matrix_presentation_set_selected(&selected_cell);
        matrix_presentation_flash();
    }
    else
    {
        matrix_presentation_beep();
    }
}

void execute_command(char *command)
{
    if (command[0] == ':')
    {
        if (strcmp(":q", command) == 0)
        {
            matrix_presentation_exit();
            exit(0);
        }

        regmatch_t pmatch[2];
        if (!regexec(&regex_go_to_line, command, 2, pmatch, 0))
        {
            execute_command_go_to_line(command, pmatch);
            return;
        }

        if (!regexec(&regex_go_to_column, command, 2, pmatch, 0))
        {
            execute_command_go_to_column(command, pmatch);
            return;
        }
    }

    char error_message[255];
    snprintf(error_message, 255, "Unknown command: %s", command);
    matrix_presentation_error(error_message);
    matrix_presentation_beep();
}

void command()
{
    matrix_presentation_read_command(&execute_command);
}

void update_screen(const screen_size_t *scr_config, csv_token *top_left_token)
{
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
        top_left_token = csv_reader_get_token(top_cell.x, top_cell.y, open_file);
        current_screen.width = 1;
        current_screen.height = 1;
        matrix_config_get_most_expanded(scr_config, &m_properties, top_left_token, open_file->columns, open_file->lines, &current_screen);

        if (current_screen.height != last_screen.height || current_screen.width != last_screen.width)
        {
            matrix_presentation_refresh(&m_properties);
            matrix_presentation_set_selected(&selected_cell);
            last_screen.height = current_screen.height;
            last_screen.width = current_screen.width;
        }
    }
}

void paint()
{
    /**
     * TODO: current_screen is storing all available size on data instead of all available size on screen. This is creating a problem for PAGE_UP/PAGE_DOWN
     **/
    const screen_size_t *scr_config = matrix_presentation_get_screen_size();
    csv_token *token = csv_reader_get_token(top_cell.x, top_cell.y, open_file);
    current_screen.height = current_screen.width = 1;
    matrix_config_get_most_expanded(scr_config, &m_properties, token, open_file->columns, open_file->lines, &current_screen);
    update_screen(scr_config, token);

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

void usage()
{
    printf("csvi - CSV Viewer %s\n", PACKAGE_VERSION);
    printf("\n");
    printf("Usage: csvi [options] file");
    printf("\n");
    printf("Options:                                                      Default Values\n");
    printf("  --separator    -s     Cell Separator                        ;\n");
    printf("  --help         -h     Print this message\n");
    printf("  --version      -v     Print the version of this build\n");
    printf("\n");
    printf("Any questions: %s\n", PACKAGE_BUGREPORT);
    printf("\n");
}

int main(int argc, char *argv[])
{
    static struct option long_options[] = {
        /*   NAME       ARGUMENT           FLAG  SHORTNAME */
        {"separator", required_argument, NULL, 's'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'}};
    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "s:vh",
                            long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 's':
            printf("Separator: %s\n", optarg);
            csv_reader_set_separator(optarg[0]);
            break;
        case 'v':
            printf("csvi version: %s\n", PACKAGE_VERSION);
            exit(0);
        case 'h':
            usage();
            exit(0);
        case '?': //used for some unknown options
            printf("unknown option: %c\n", optopt);
            exit(1);
        default:
            fprintf(stderr, "Usage: %s [-ilw] [file...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    CHECK_FATAL(argc < 2, "No file to read...\n");
    open_file = csv_reader_read_file(argv[argc - 1]);

    csv_token *curr = open_file->first;
    while (curr)
    {
        curr = curr->next;
    }
    init_commands();
    matrix_presentation_init();
    matrix_presentation_configure_handler(UP, &up);
    matrix_presentation_configure_handler(LEFT, &left);
    matrix_presentation_configure_handler(RIGHT, &right);
    matrix_presentation_configure_handler(DOWN, &down);
    matrix_presentation_configure_handler(PAINT, &paint);
    matrix_presentation_configure_handler(PAGE_UP, &page_up);
    matrix_presentation_configure_handler(PAGE_DOWN, &page_down);
    matrix_presentation_configure_handler(HOME, &home);
    matrix_presentation_configure_handler(END, &end);
    matrix_presentation_configure_handler(COMMAND, &command);
    matrix_presentation_handle();
    matrix_presentation_exit();
    return 0;
}
