#include "app/viewer.h"

#include <stdbool.h>
#include <stdlib.h>

#include "cmd/commands.h"
#include "common/errors.h"
#include "common/log.h"
#include "io/csv-reader.h"
#include "layout/matrix-config.h"
#include "nav/navigation.h"
#include "ui/matrix-presentation.h"

struct csvi_viewer
{
    csv_contents *file;
    screen_size_t presented_matrix;
    matrix_properties_t properties;
    coordinates_t top_cell;
    coordinates_t selected_cell;
    int exit_code;
    bool owns_file;
};

static csvi_viewer_t *active_viewer = NULL;

static void handle_navigation(csvi_viewer_t *viewer, NavigationResult result)
{
    switch (result)
    {
    case CURSOR_UPDATED:
        matrix_presentation_set_selected(&viewer->selected_cell);
        matrix_presentation_flash();
        break;
    case BEEP:
        matrix_presentation_beep();
        break;
    default:
        break;
    }
}

static void viewer_up_impl(csvi_viewer_t *viewer)
{
    handle_navigation(viewer, navigate_up(&viewer->top_cell, &viewer->selected_cell));
}

static void viewer_left_impl(csvi_viewer_t *viewer)
{
    handle_navigation(viewer, navigate_left(&viewer->top_cell, &viewer->selected_cell));
}

static void viewer_right_impl(csvi_viewer_t *viewer)
{
    handle_navigation(viewer, navigate_right(&viewer->top_cell, &viewer->selected_cell, &viewer->presented_matrix, viewer->file->columns));
}

static void viewer_down_impl(csvi_viewer_t *viewer)
{
    handle_navigation(viewer, navigate_down(&viewer->top_cell, &viewer->selected_cell, &viewer->presented_matrix, viewer->file->lines));
}

static void viewer_page_up_impl(csvi_viewer_t *viewer)
{
    handle_navigation(viewer, navigate_page_up(&viewer->top_cell, &viewer->selected_cell, &viewer->presented_matrix));
}

static void viewer_page_down_impl(csvi_viewer_t *viewer)
{
    handle_navigation(viewer, navigate_page_down(&viewer->top_cell, &viewer->selected_cell, &viewer->presented_matrix, viewer->file->lines));
}

static void viewer_home_impl(csvi_viewer_t *viewer)
{
    handle_navigation(viewer, navigate_page_previous(&viewer->top_cell, &viewer->selected_cell, &viewer->presented_matrix));
}

static void viewer_end_impl(csvi_viewer_t *viewer)
{
    handle_navigation(viewer, navigate_page_next(&viewer->top_cell, &viewer->selected_cell, &viewer->presented_matrix, viewer->file->columns));
}

static void viewer_notify_selection(csvi_viewer_t *viewer)
{
#ifndef CSVI_TEST
    matrix_presentation_set_selected(&viewer->selected_cell);
    matrix_presentation_flash();
#else
    (void)viewer;
#endif
}

static void viewer_beep(void)
{
#ifndef CSVI_TEST
    matrix_presentation_beep();
#endif
}

static void viewer_go_to_line(csvi_viewer_t *viewer, size_t line)
{
    if (line < viewer->file->lines)
    {
        viewer->top_cell.y = viewer->selected_cell.y = line;
        viewer_notify_selection(viewer);
    }
    else
    {
        viewer_beep();
    }
}

static void viewer_go_to_column(csvi_viewer_t *viewer, size_t column)
{
    if (column < viewer->file->columns)
    {
        viewer->top_cell.x = viewer->selected_cell.x = column;
        viewer_notify_selection(viewer);
    }
    else
    {
        viewer_beep();
    }
}

static void viewer_go_to_cell(csvi_viewer_t *viewer, size_t column, size_t line)
{
    if (column < viewer->file->columns && line < viewer->file->lines)
    {
        viewer->top_cell.x = viewer->selected_cell.x = column;
        viewer->top_cell.y = viewer->selected_cell.y = line;
        viewer_notify_selection(viewer);
    }
    else
    {
        viewer_beep();
    }
}

static void viewer_go_to_first_line(csvi_viewer_t *viewer)
{
    if (viewer->selected_cell.y > 0)
    {
        viewer->top_cell.y = viewer->selected_cell.y = 0;
        viewer_notify_selection(viewer);
    }
    else
    {
        viewer_beep();
    }
}

static void viewer_go_to_last_line(csvi_viewer_t *viewer)
{
    if (viewer->selected_cell.y < viewer->file->lines)
    {
        viewer->top_cell.y = viewer->selected_cell.y = viewer->file->lines - 1;
        viewer_notify_selection(viewer);
    }
    else
    {
        viewer_beep();
    }
}

static void viewer_go_to_first_column(csvi_viewer_t *viewer)
{
    if (viewer->selected_cell.x > 0)
    {
        viewer->top_cell.x = viewer->selected_cell.x = 0;
        viewer_notify_selection(viewer);
    }
    else
    {
        viewer_beep();
    }
}

static void viewer_go_to_last_column(csvi_viewer_t *viewer)
{
    if (viewer->selected_cell.x < viewer->file->columns)
    {
        viewer->top_cell.x = viewer->selected_cell.x = viewer->file->columns - 1;
        viewer_notify_selection(viewer);
    }
    else
    {
        viewer_beep();
    }
}

static void viewer_show_error(char *error_message)
{
    matrix_presentation_error(error_message);
    matrix_presentation_beep();
}

static void viewer_exit(int exit_code)
{
    if (active_viewer)
    {
        active_viewer->exit_code = exit_code;
    }
    matrix_presentation_request_stop();
}

static void viewer_update_screen(csvi_viewer_t *viewer,
                                 const screen_size_t *scr_config,
                                 screen_size_t *current_screen,
                                 csv_token *top_left_token)
{
    if (current_screen->height != viewer->presented_matrix.height || current_screen->width != viewer->presented_matrix.width)
    {
        matrix_presentation_refresh(&viewer->properties);
        viewer->presented_matrix.height = current_screen->height;
        viewer->presented_matrix.width = current_screen->width;
        matrix_presentation_set_selected(&viewer->selected_cell);
    }

    if (viewer->selected_cell.x == (size_t)viewer->presented_matrix.width + viewer->top_cell.x)
    {
        viewer->top_cell.x += 2;
        matrix_presentation_refresh(&viewer->properties);
        matrix_presentation_set_selected(&viewer->selected_cell);
        top_left_token = csv_reader_get_token(viewer->top_cell.x, viewer->top_cell.y, viewer->file);
        current_screen->width = 1;
        current_screen->height = 1;
        matrix_config_get_most_expanded(scr_config, &viewer->properties, top_left_token, viewer->file->columns, viewer->file->lines, current_screen);

        if (current_screen->height != viewer->presented_matrix.height || current_screen->width != viewer->presented_matrix.width)
        {
            matrix_presentation_refresh(&viewer->properties);
            matrix_presentation_set_selected(&viewer->selected_cell);
            viewer->presented_matrix.height = current_screen->height;
            viewer->presented_matrix.width = current_screen->width;
        }
    }
}

static void viewer_paint(void)
{
    csvi_viewer_t *viewer = active_viewer;
    const screen_size_t *scr_config = matrix_presentation_get_screen_size();
    csv_token *token = csv_reader_get_token(viewer->top_cell.x, viewer->top_cell.y, viewer->file);
    screen_size_t current_screen = {.width = 1, .height = 1};
    matrix_config_get_most_expanded(scr_config, &viewer->properties, token, viewer->file->columns, viewer->file->lines, &current_screen);
    viewer_update_screen(viewer, scr_config, &current_screen, token);

    matrix_config_t *config = matrix_config_initialize(current_screen.width, current_screen.height);
    matrix_config_load_sizes(token, config);
    while (token)
    {
        if (token->x >= viewer->top_cell.x && token->x < viewer->top_cell.x + (size_t)current_screen.width &&
            token->y >= viewer->top_cell.y && token->y < viewer->top_cell.y + (size_t)current_screen.height)
        {
            coordinates_t position = {.x = token->x - viewer->top_cell.x,
                                      .y = token->y - viewer->top_cell.y};
            matrix_presentation_set_value(&position, token->data, token->x == viewer->selected_cell.x && token->y == viewer->selected_cell.y, config, &viewer->properties);
        }
        else if (token->x > viewer->top_cell.x + (size_t)current_screen.width &&
                 token->y > viewer->top_cell.y + (size_t)current_screen.height)
        {
            break;
        }
        token = token->next;
    }

    matrix_config_dispose(config);
}

static void viewer_command(void)
{
    matrix_presentation_read_command(&commands_execute);
}

static void viewer_up(void)
{
    viewer_up_impl(active_viewer);
}

static void viewer_left(void)
{
    viewer_left_impl(active_viewer);
}

static void viewer_right(void)
{
    viewer_right_impl(active_viewer);
}

static void viewer_down(void)
{
    viewer_down_impl(active_viewer);
}

static void viewer_page_up(void)
{
    viewer_page_up_impl(active_viewer);
}

static void viewer_page_down(void)
{
    viewer_page_down_impl(active_viewer);
}

static void viewer_home(void)
{
    viewer_home_impl(active_viewer);
}

static void viewer_end(void)
{
    viewer_end_impl(active_viewer);
}

static void viewer_go_to_line_cb(size_t line)
{
    viewer_go_to_line(active_viewer, line);
}

static void viewer_go_to_column_cb(size_t column)
{
    viewer_go_to_column(active_viewer, column);
}

static void viewer_go_to_cell_cb(size_t column, size_t line)
{
    viewer_go_to_cell(active_viewer, column, line);
}

static void viewer_go_to_first_line_cb(void)
{
    viewer_go_to_first_line(active_viewer);
}

static void viewer_go_to_last_line_cb(void)
{
    viewer_go_to_last_line(active_viewer);
}

static void viewer_go_to_first_column_cb(void)
{
    viewer_go_to_first_column(active_viewer);
}

static void viewer_go_to_last_column_cb(void)
{
    viewer_go_to_last_column(active_viewer);
}

static csvi_viewer_t *viewer_alloc(void)
{
    csvi_viewer_t *viewer = calloc(1, sizeof(csvi_viewer_t));
    if (!viewer)
    {
        return NULL;
    }

    viewer->presented_matrix.width = 1;
    viewer->presented_matrix.height = 1;
    viewer->properties = (matrix_properties_t){
        .cell_padding_top = 0,
        .cell_padding_right = 2,
        .cell_padding_bottom = 0,
        .cell_padding_left = 1,
        .margin_top = 1,
        .margin_right = 1,
        .margin_bottom = 2,
        .margin_left = 1};
    viewer->exit_code = CSVI_EXIT_OK;
    viewer->owns_file = true;
    return viewer;
}

csvi_viewer_t *csvi_viewer_create(const csvi_viewer_options_t *opts)
{
    csvi_viewer_t *viewer = viewer_alloc();
    if (!viewer)
    {
        return NULL;
    }

    if (opts)
    {
        csv_reader_set_separator(opts->separator);
    }
    return viewer;
}

int csvi_viewer_open(csvi_viewer_t *viewer, const char *path)
{
    char errbuf[512];
    csv_contents *file = csv_reader_read_file(path, errbuf, sizeof(errbuf));
    if (!file)
    {
        csvi_log_error("%s\n", errbuf);
        return CSVI_EXIT_IO;
    }

    viewer->file = file;
    csvi_log_info("opened '%s' (%zu lines, %zu columns)\n", path, file->lines, file->columns);
    return CSVI_EXIT_OK;
}

int csvi_viewer_run(csvi_viewer_t *viewer)
{
    active_viewer = viewer;

    command_executors_t command_executors = {
        .go_to_line = viewer_go_to_line_cb,
        .go_to_column = viewer_go_to_column_cb,
        .go_to_cell = viewer_go_to_cell_cb,
        .go_to_first_line = viewer_go_to_first_line_cb,
        .go_to_last_line = viewer_go_to_last_line_cb,
        .go_to_first_column = viewer_go_to_first_column_cb,
        .go_to_last_column = viewer_go_to_last_column_cb,
        .show_error = viewer_show_error,
        .exit = viewer_exit};

    commands_init(&command_executors);
    matrix_presentation_init();
    matrix_presentation_configure_handler(UP, viewer_up);
    matrix_presentation_configure_handler(LEFT, viewer_left);
    matrix_presentation_configure_handler(RIGHT, viewer_right);
    matrix_presentation_configure_handler(DOWN, viewer_down);
    matrix_presentation_configure_handler(PAINT, viewer_paint);
    matrix_presentation_configure_handler(PAGE_UP, viewer_page_up);
    matrix_presentation_configure_handler(PAGE_DOWN, viewer_page_down);
    matrix_presentation_configure_handler(HOME, viewer_home);
    matrix_presentation_configure_handler(END, viewer_end);
    matrix_presentation_configure_handler(COMMAND, viewer_command);
    matrix_presentation_handle();
    matrix_presentation_exit();

    commands_shutdown();
    matrix_presentation_shutdown();
    active_viewer = NULL;
    return viewer->exit_code;
}

void csvi_viewer_destroy(csvi_viewer_t *viewer)
{
    if (!viewer)
    {
        return;
    }

    if (viewer->file && viewer->owns_file)
    {
        csv_contents_dispose(viewer->file);
    }
    free(viewer);
}

#ifdef CSVI_TEST
csvi_viewer_t *csvi_viewer_create_with_file(const csv_contents *file)
{
    csvi_viewer_t *viewer = viewer_alloc();
    if (!viewer)
    {
        return NULL;
    }
    viewer->file = (csv_contents *)file;
    viewer->owns_file = false;
    return viewer;
}

const coordinates_t *csvi_viewer_selected_cell(const csvi_viewer_t *viewer)
{
    return &viewer->selected_cell;
}

void csvi_viewer_exec_go_to_last_line(csvi_viewer_t *viewer)
{
    viewer_go_to_last_line(viewer);
}

void csvi_viewer_exec_go_to_last_column(csvi_viewer_t *viewer)
{
    viewer_go_to_last_column(viewer);
}
#endif
