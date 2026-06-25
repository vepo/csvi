#include "app/viewer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curses.h>

#include "app/search.h"
#include "cmd/commands.h"
#include "common/errors.h"
#include "common/log.h"
#include "io/csv-reader.h"
#include "io/csv-writer.h"
#include "layout/matrix-config.h"
#include "layout/viewport-cache.h"
#include "nav/navigation.h"
#include "ui/input-modes.h"
#include "ui/matrix-presentation.h"
#include "ui/paint.h"
#include "ui/status-bar.h"

#define CSVI_EDIT_BUFFER_SIZE 4096

struct csvi_viewer
{
    csv_contents *file;
    viewport_cache_t *cache;
    screen_size_t presented_matrix;
    screen_size_t viewport_cells;
    matrix_config_t *viewport_config;
    matrix_properties_t properties;
    matrix_display_options_t display;
    coordinates_t top_cell;
    coordinates_t selected_cell;
    coordinates_t prev_selected;
    coordinates_t search_match;
    csvi_search_t *search;
    char *file_path;
    char input_buffer[256];
    char status_message[256];
    char cell_preview[256];
    char edit_buffer[CSVI_EDIT_BUFFER_SIZE];
    char edit_original[CSVI_EDIT_BUFFER_SIZE];
    size_t edit_cursor;
    char separator;
    bool header_enabled;
    bool owns_file;
    bool has_search_match;
    bool read_only;
    bool file_modified;
    bool edit_dirty;
    int exit_code;
};

static csvi_viewer_t *active_viewer = NULL;

static bool cell_is_search_match(const csvi_viewer_t *viewer, size_t x, size_t y)
{
    if (!viewer->has_search_match)
    {
        return false;
    }
    return viewer->search_match.x == x && viewer->search_match.y == y;
}

static void viewer_clear_status(csvi_viewer_t *viewer)
{
    viewer->status_message[0] = '\0';
}

static void viewer_set_status(csvi_viewer_t *viewer, const char *message)
{
    if (message)
    {
        strncpy(viewer->status_message, message, sizeof(viewer->status_message) - 1);
        viewer->status_message[sizeof(viewer->status_message) - 1] = '\0';
    }
}

static void viewer_update_cell_preview(csvi_viewer_t *viewer)
{
    csv_token *token = csv_reader_get_token(viewer->selected_cell.x, viewer->selected_cell.y, viewer->file);
    if (token && token->data)
    {
        strncpy(viewer->cell_preview, token->data, sizeof(viewer->cell_preview) - 1);
        viewer->cell_preview[sizeof(viewer->cell_preview) - 1] = '\0';
    }
    else
    {
        viewer->cell_preview[0] = '\0';
    }
}

static void viewer_draw_status(csvi_viewer_t *viewer)
{
    const screen_size_t *scr = matrix_presentation_get_screen_size();
    status_bar_resize(scr->width, scr->height - 1);

    if (input_mode_get() == INPUT_INSERT)
    {
        status_bar_draw_edit_preview(scr->height - 2,
                                     scr->width,
                                     viewer->edit_buffer,
                                     viewer->edit_original,
                                     viewer->edit_cursor);
    }
    else
    {
        matrix_presentation_hide_cursor();
    }

    status_bar_draw(viewer->file_path,
                    viewer->selected_cell.y,
                    viewer->file->lines,
                    viewer->selected_cell.x,
                    viewer->file->columns,
                    viewer->separator,
                    input_mode_get(),
                    viewer->file_modified,
                    input_mode_get() == INPUT_INSERT ? NULL : viewer->cell_preview,
                    viewer->status_message);
}

static void viewer_set_edit_margin(csvi_viewer_t *viewer, bool editing)
{
    viewer->properties.margin_bottom = editing ? 2 : 1;
}

static cell_draw_state_t viewer_cell_draw_state(const csvi_viewer_t *viewer, size_t x, size_t y)
{
    if (input_mode_get() == INPUT_INSERT && x == viewer->selected_cell.x && y == viewer->selected_cell.y)
    {
        return CELL_DRAW_EDITING;
    }
    if (x == viewer->selected_cell.x && y == viewer->selected_cell.y)
    {
        return CELL_DRAW_SELECTED;
    }
    return CELL_DRAW_NORMAL;
}

static void viewer_edit_update_dirty(csvi_viewer_t *viewer)
{
    viewer->edit_dirty = strcmp(viewer->edit_buffer, viewer->edit_original) != 0;
}

static bool viewer_insert_blocks_navigation(csvi_viewer_t *viewer)
{
    (void)viewer;
    matrix_presentation_beep();
    viewer_set_status(viewer, "Commit (Enter) or cancel (Esc) first");
    viewer_draw_status(viewer);
    return true;
}

static void viewer_edit_insert_char(csvi_viewer_t *viewer, char ch)
{
    size_t len = strlen(viewer->edit_buffer);
    if (len + 1 >= sizeof(viewer->edit_buffer))
    {
        return;
    }

    if (viewer->edit_cursor > len)
    {
        viewer->edit_cursor = len;
    }

    memmove(viewer->edit_buffer + viewer->edit_cursor + 1,
            viewer->edit_buffer + viewer->edit_cursor,
            len - viewer->edit_cursor + 1);
    viewer->edit_buffer[viewer->edit_cursor] = ch;
    viewer->edit_cursor++;
    viewer_edit_update_dirty(viewer);
}

static void viewer_edit_backspace(csvi_viewer_t *viewer)
{
    if (viewer->edit_cursor == 0)
    {
        return;
    }

    size_t len = strlen(viewer->edit_buffer);
    memmove(viewer->edit_buffer + viewer->edit_cursor - 1,
            viewer->edit_buffer + viewer->edit_cursor,
            len - viewer->edit_cursor + 1);
    viewer->edit_cursor--;
    viewer_edit_update_dirty(viewer);
}

static void viewer_edit_move_cursor(csvi_viewer_t *viewer, int delta)
{
    if (delta < 0 && viewer->edit_cursor > 0)
    {
        viewer->edit_cursor--;
    }
    else if (delta > 0 && viewer->edit_cursor < strlen(viewer->edit_buffer))
    {
        viewer->edit_cursor++;
    }
}

static matrix_config_t *viewer_build_viewport_config(csvi_viewer_t *viewer)
{
    matrix_config_t *config = matrix_config_initialize(viewer->viewport_cells.width, viewer->viewport_cells.height);
    if (!config || !viewer->cache)
    {
        return config;
    }

    for (size_t vy = 0; vy < viewer->viewport_cells.height; ++vy)
    {
        size_t y = viewer->top_cell.y + vy;
        if (y < viewer->cache->lines)
        {
            config->line_height[vy] = viewport_cache_row_height(viewer->cache, y);
        }
    }

    for (size_t vx = 0; vx < viewer->viewport_cells.width; ++vx)
    {
        size_t x = viewer->top_cell.x + vx;
        if (x < viewer->cache->columns)
        {
            config->column_width[vx] = viewport_cache_col_width(viewer->cache, x);
        }
    }

    return config;
}

static void viewer_recompute_viewport(csvi_viewer_t *viewer)
{
    const screen_size_t *grid_px = matrix_presentation_get_grid_size(&viewer->properties);
    screen_size_t current = {.width = 1, .height = 1};

    if (viewer->cache)
    {
        viewport_cache_get_most_expanded(viewer->cache,
                                         &viewer->top_cell,
                                         grid_px,
                                         &viewer->properties,
                                         viewer->file->columns,
                                         viewer->file->lines,
                                         &current);
    }

    if (viewer->header_enabled && viewer->top_cell.y == 0 && current.height > 1)
    {
        current.height -= 1;
    }

    viewer->viewport_cells = current;
    viewer->presented_matrix = current;

    if (viewer->viewport_config)
    {
        matrix_config_dispose(viewer->viewport_config);
    }
    viewer->viewport_config = viewer_build_viewport_config(viewer);
}

static bool viewer_scroll_selection_into_view(csvi_viewer_t *viewer)
{
    bool changed = false;

    for (int pass = 0; pass < 2; ++pass)
    {
        coordinates_t top_before = viewer->top_cell;

        viewer_recompute_viewport(viewer);
        nav_scroll_into_view(&viewer->top_cell,
                             &viewer->selected_cell,
                             (size_t)viewer->viewport_cells.width,
                             (size_t)viewer->viewport_cells.height,
                             viewer->file->columns,
                             viewer->file->lines);

        if (top_before.x != viewer->top_cell.x || top_before.y != viewer->top_cell.y)
        {
            changed = true;
        }
        else
        {
            break;
        }
    }

    return changed;
}

static void viewer_draw_cell_at(csvi_viewer_t *viewer, size_t abs_x, size_t abs_y)
{
    if (!viewer->viewport_config)
    {
        return;
    }

    if (abs_x < viewer->top_cell.x || abs_y < viewer->top_cell.y ||
        abs_x >= viewer->top_cell.x + (size_t)viewer->viewport_cells.width ||
        abs_y >= viewer->top_cell.y + (size_t)viewer->viewport_cells.height)
    {
        return;
    }

    coordinates_t vp = {.x = abs_x - viewer->top_cell.x, .y = abs_y - viewer->top_cell.y};
    cell_draw_state_t state = viewer_cell_draw_state(viewer, abs_x, abs_y);
    const char *data;
    size_t edit_cursor_pos = 0;

    if (state == CELL_DRAW_EDITING)
    {
        data = viewer->edit_buffer;
        edit_cursor_pos = viewer->edit_cursor;
    }
    else
    {
        csv_token *token = csv_reader_get_token(abs_x, abs_y, viewer->file);
        data = (token && token->data) ? token->data : "";
    }

    matrix_presentation_draw_cell(&vp,
                                  abs_x,
                                  abs_y,
                                  data,
                                  state,
                                  cell_is_search_match(viewer, abs_x, abs_y),
                                  edit_cursor_pos,
                                  viewer->viewport_config,
                                  &viewer->properties);
}

static void viewer_paint_full(csvi_viewer_t *viewer)
{
    viewer_recompute_viewport(viewer);
    matrix_presentation_clear_grid(&viewer->properties);

    for (size_t vy = 0; vy < (size_t)viewer->viewport_cells.height; ++vy)
    {
        for (size_t vx = 0; vx < (size_t)viewer->viewport_cells.width; ++vx)
        {
            size_t x = viewer->top_cell.x + vx;
            size_t y = viewer->top_cell.y + vy;
            viewer_draw_cell_at(viewer, x, y);
        }
    }

    if (viewer->header_enabled && viewer->top_cell.y > 0)
    {
        for (size_t vx = 0; vx < viewer->file->columns && vx < 20; ++vx)
        {
            viewer_draw_cell_at(viewer, vx, 0);
        }
    }

    if (input_mode_get() != INPUT_INSERT)
    {
        matrix_presentation_hide_cursor();
    }

    viewer_update_cell_preview(viewer);
    viewer_draw_status(viewer);
}

static void viewer_paint_cursor(csvi_viewer_t *viewer)
{
    viewer_draw_cell_at(viewer, viewer->prev_selected.x, viewer->prev_selected.y);
    viewer_draw_cell_at(viewer, viewer->selected_cell.x, viewer->selected_cell.y);
    viewer_update_cell_preview(viewer);
    viewer_draw_status(viewer);
}

static void viewer_apply_paint(csvi_viewer_t *viewer, paint_action_t action)
{
    switch (action)
    {
    case PAINT_CURSOR:
        viewer_paint_cursor(viewer);
        break;
    case PAINT_VIEWPORT:
    case PAINT_FULL:
        viewer_paint_full(viewer);
        break;
    default:
        break;
    }
}

static paint_action_t viewer_navigate(csvi_viewer_t *viewer,
                                      NavigationResult result,
                                      const coordinates_t *top_before,
                                      const coordinates_t *cursor_before)
{
    if (result == BEEP)
    {
        matrix_presentation_beep();
        return PAINT_NONE;
    }
    if (result == CURSOR_UPDATED)
    {
        bool viewport_moved = top_before->x != viewer->top_cell.x || top_before->y != viewer->top_cell.y;
        viewport_moved = viewer_scroll_selection_into_view(viewer) || viewport_moved;
        viewer_update_cell_preview(viewer);
        return viewport_moved ? PAINT_VIEWPORT : PAINT_CURSOR;
    }
    return PAINT_NONE;
}

static void viewer_jump_to(csvi_viewer_t *viewer, size_t x, size_t y)
{
    viewer->prev_selected = viewer->selected_cell;
    viewer->selected_cell.x = x;
    viewer->selected_cell.y = y;

    if (viewer->header_enabled && y == 0)
    {
        viewer->top_cell.y = 0;
    }
    else if (viewer->header_enabled && viewer->top_cell.y == 0)
    {
        viewer->top_cell.y = 1;
    }

    (void)viewer_scroll_selection_into_view(viewer);
}

static paint_action_t viewer_jump_paint(csvi_viewer_t *viewer)
{
    viewer_update_cell_preview(viewer);
    return PAINT_FULL;
}

static void viewer_show_error(char *error_message)
{
    if (active_viewer)
    {
        viewer_set_status(active_viewer, error_message);
#ifndef CSVI_TEST
        viewer_draw_status(active_viewer);
        matrix_presentation_beep();
#endif
    }
}

static void viewer_exit(int exit_code)
{
    if (active_viewer)
    {
        active_viewer->exit_code = exit_code;
    }
    matrix_presentation_request_stop();
}

static void viewer_request_quit(void)
{
    if (!active_viewer)
    {
        return;
    }

    if (active_viewer->file_modified)
    {
        viewer_show_error("No write since last change");
        return;
    }

    viewer_exit(0);
}

static void viewer_exit_cb(int exit_code)
{
    viewer_request_quit();
    (void)exit_code;
}

static void viewer_exit_force_cb(int exit_code)
{
    viewer_exit(exit_code);
}

static int viewer_save_cb(void)
{
    if (!active_viewer || !active_viewer->file_path)
    {
        viewer_show_error("no file path");
        return -1;
    }

    char errbuf[512];
    if (csv_writer_write_file(active_viewer->file_path,
                              active_viewer->file,
                              active_viewer->separator,
                              errbuf,
                              sizeof(errbuf)) != 0)
    {
        viewer_show_error(errbuf);
        return -1;
    }

    active_viewer->file_modified = false;
    viewer_set_status(active_viewer, "written");
    viewer_draw_status(active_viewer);
    return 0;
}

static paint_action_t viewer_enter_insert(csvi_viewer_t *viewer)
{
    if (viewer->read_only)
    {
        matrix_presentation_beep();
        viewer_set_status(viewer, "File is read-only");
        viewer_draw_status(viewer);
        return PAINT_NONE;
    }

    csv_token *token = csv_reader_get_token(viewer->selected_cell.x, viewer->selected_cell.y, viewer->file);
    const char *current = (token && token->data) ? token->data : "";

    strncpy(viewer->edit_original, current, sizeof(viewer->edit_original) - 1);
    viewer->edit_original[sizeof(viewer->edit_original) - 1] = '\0';
    strncpy(viewer->edit_buffer, current, sizeof(viewer->edit_buffer) - 1);
    viewer->edit_buffer[sizeof(viewer->edit_buffer) - 1] = '\0';
    viewer->edit_cursor = strlen(viewer->edit_buffer);
    viewer->edit_dirty = false;
    viewer_clear_status(viewer);

    viewer_set_edit_margin(viewer, true);
    input_mode_set(INPUT_INSERT);
    viewer->prev_selected = viewer->selected_cell;
    return PAINT_FULL;
}

static paint_action_t viewer_cancel_edit(csvi_viewer_t *viewer)
{
    strncpy(viewer->edit_buffer, viewer->edit_original, sizeof(viewer->edit_buffer) - 1);
    viewer->edit_buffer[sizeof(viewer->edit_buffer) - 1] = '\0';
    viewer->edit_cursor = strlen(viewer->edit_buffer);
    viewer->edit_dirty = false;

    viewer_set_edit_margin(viewer, false);
    input_mode_set(INPUT_NORMAL);
    matrix_presentation_hide_cursor();
    viewer_clear_status(viewer);
    return PAINT_FULL;
}

static paint_action_t viewer_commit_cell(csvi_viewer_t *viewer)
{
    size_t col_before = viewport_cache_col_width(viewer->cache, viewer->selected_cell.x);
    size_t row_before = viewport_cache_row_height(viewer->cache, viewer->selected_cell.y);

    csv_token *token = csv_reader_get_token(viewer->selected_cell.x, viewer->selected_cell.y, viewer->file);
    const char *previous = (token && token->data) ? token->data : "";
    bool changed = strcmp(previous, viewer->edit_buffer) != 0;

    if (csv_reader_set_cell(viewer->file, viewer->selected_cell.x, viewer->selected_cell.y, viewer->edit_buffer) != 0)
    {
        matrix_presentation_beep();
        viewer_set_status(viewer, "could not update cell");
        viewer_draw_status(viewer);
        return PAINT_NONE;
    }

    viewport_cache_update_cell(viewer->cache, viewer->file, viewer->selected_cell.x, viewer->selected_cell.y);

    if (changed)
    {
        viewer->file_modified = true;
        if (viewer->search && csvi_search_pattern(viewer->search))
        {
            csvi_search_refresh(viewer->search);
        }
    }

    strncpy(viewer->edit_original, viewer->edit_buffer, sizeof(viewer->edit_original) - 1);
    viewer->edit_original[sizeof(viewer->edit_original) - 1] = '\0';
    viewer->edit_dirty = false;

    viewer_set_edit_margin(viewer, false);
    input_mode_set(INPUT_NORMAL);
    matrix_presentation_hide_cursor();
    viewer_clear_status(viewer);

    size_t col_after = viewport_cache_col_width(viewer->cache, viewer->selected_cell.x);
    size_t row_after = viewport_cache_row_height(viewer->cache, viewer->selected_cell.y);
    if (col_before != col_after || row_before != row_after)
    {
        return PAINT_FULL;
    }

    return PAINT_FULL;
}

static bool viewer_is_navigation_key(int key)
{
    switch (key)
    {
    case 'h':
    case 'j':
    case 'k':
    case 'l':
    case KEY_LEFT:
    case KEY_RIGHT:
    case KEY_UP:
    case KEY_DOWN:
    case KEY_HOME:
    case KEY_END:
    case KEY_NPAGE:
    case KEY_PPAGE:
    case 6:
    case 2:
    case 'g':
    case 'G':
    case 1:
    case 5:
    case ':':
    case '/':
    case '?':
        return true;
    default:
        return false;
    }
}

static paint_action_t viewer_handle_insert(csvi_viewer_t *viewer, int key)
{
    if (key == 27)
    {
        return viewer_cancel_edit(viewer);
    }

    if (key == 10 || key == KEY_ENTER)
    {
        return viewer_commit_cell(viewer);
    }

    if (key == KEY_BACKSPACE || key == 127)
    {
        viewer_edit_backspace(viewer);
        return PAINT_CURSOR;
    }

    if (key == KEY_LEFT)
    {
        viewer_edit_move_cursor(viewer, -1);
        return PAINT_CURSOR;
    }

    if (key == KEY_RIGHT)
    {
        viewer_edit_move_cursor(viewer, 1);
        return PAINT_CURSOR;
    }

    if (viewer_is_navigation_key(key))
    {
        viewer_insert_blocks_navigation(viewer);
        return PAINT_NONE;
    }

    if (key > 0 && key < 256 && key != 27)
    {
        viewer_edit_insert_char(viewer, (char)key);
        return PAINT_CURSOR;
    }

    return PAINT_NONE;
}

static void viewer_go_to_line_cb(size_t line)
{
    if (!active_viewer || line >= active_viewer->file->lines)
    {
        viewer_show_error("invalid line");
        return;
    }
    viewer_jump_to(active_viewer, active_viewer->selected_cell.x, line);
    viewer_apply_paint(active_viewer, viewer_jump_paint(active_viewer));
}

static void viewer_go_to_column_cb(size_t column)
{
    if (!active_viewer || column >= active_viewer->file->columns)
    {
        viewer_show_error("invalid column");
        return;
    }
    viewer_jump_to(active_viewer, column, active_viewer->selected_cell.y);
    viewer_apply_paint(active_viewer, viewer_jump_paint(active_viewer));
}

static void viewer_go_to_cell_cb(size_t column, size_t line)
{
    if (!active_viewer || column >= active_viewer->file->columns || line >= active_viewer->file->lines)
    {
        viewer_show_error("invalid cell");
        return;
    }
    viewer_jump_to(active_viewer, column, line);
    viewer_apply_paint(active_viewer, viewer_jump_paint(active_viewer));
}

static void viewer_go_to_first_line_cb(void)
{
    viewer_go_to_line_cb(0);
}

static void viewer_go_to_last_line_cb(void)
{
    if (active_viewer)
    {
        viewer_go_to_line_cb(active_viewer->file->lines - 1);
    }
}

static void viewer_go_to_first_column_cb(void)
{
    viewer_go_to_column_cb(0);
}

static void viewer_go_to_last_column_cb(void)
{
    if (active_viewer)
    {
        viewer_go_to_column_cb(active_viewer->file->columns - 1);
    }
}

static void viewer_search_jump_cb(bool forward)
{
    if (!active_viewer || !active_viewer->search)
    {
        return;
    }

    coordinates_t match;
    bool found = forward ? csvi_search_next(active_viewer->search, &match)
                         : csvi_search_prev(active_viewer->search, &match);
    if (!found)
    {
        viewer_show_error("no search matches");
        return;
    }

    active_viewer->has_search_match = true;
    active_viewer->search_match = match;
    viewer_jump_to(active_viewer, match.x, match.y);
    char msg[64];
    snprintf(msg,
             sizeof(msg),
             "match %zu/%zu",
             csvi_search_match_index(active_viewer->search) + 1,
             csvi_search_match_count(active_viewer->search));
    viewer_set_status(active_viewer, msg);
    viewer_apply_paint(active_viewer, viewer_jump_paint(active_viewer));
}

static void viewer_search_next_cb(void)
{
    viewer_search_jump_cb(true);
}

static void viewer_search_prev_cb(void)
{
    viewer_search_jump_cb(false);
}

static void viewer_set_separator_cb(char sep)
{
    if (!active_viewer)
    {
        return;
    }
    active_viewer->separator = sep;
    csv_reader_set_separator(sep);
    viewer_set_status(active_viewer, "separator updated; reopen file to re-parse");
    viewer_draw_status(active_viewer);
}

static void viewer_set_header_cb(bool enabled)
{
    if (!active_viewer)
    {
        return;
    }
    active_viewer->header_enabled = enabled;
    active_viewer->display.header_enabled = enabled;
    if (enabled && active_viewer->top_cell.y == 0 && active_viewer->selected_cell.y > 0)
    {
        active_viewer->top_cell.y = 1;
    }
    viewer_apply_paint(active_viewer, PAINT_FULL);
}

static void viewer_show_help(void)
{
    const char *lines[] = {
        "Keys: arrows/hjkl move | i/Insert edit cell | Enter commit | Esc cancel",
        "       Home/End row start/end | g/G first/last row | Ctrl+F/B page down/up",
        "       : command | / search | ? help | q quit",
        "Commands: :line N :col N :cell R,C :w :wq :q :q! :top :bottom :left :right",
        "          :set sep=X :set header=on|off :n :N",
        NULL};
    int row = 2;
    for (int i = 0; lines[i]; ++i, ++row)
    {
        mvprintw(row, 2, "%s", lines[i]);
    }
    mvprintw(row + 1, 2, "Press Esc or q to close help");
}

static paint_action_t viewer_handle_normal(csvi_viewer_t *viewer, int key)
{
    coordinates_t top_before = viewer->top_cell;
    coordinates_t cursor_before = viewer->selected_cell;
    viewer->prev_selected = viewer->selected_cell;

    switch (key)
    {
    case 'h':
    case KEY_LEFT:
        return viewer_navigate(viewer,
                               navigate_left(&viewer->top_cell, &viewer->selected_cell, &viewer->presented_matrix),
                               &top_before,
                               &cursor_before);
    case 'l':
    case KEY_RIGHT:
        return viewer_navigate(viewer,
                               navigate_right(&viewer->top_cell,
                                              &viewer->selected_cell,
                                              &viewer->presented_matrix,
                                              viewer->file->columns),
                               &top_before,
                               &cursor_before);
    case 'k':
    case KEY_UP:
        return viewer_navigate(viewer,
                               navigate_up(&viewer->top_cell, &viewer->selected_cell, &viewer->presented_matrix),
                               &top_before,
                               &cursor_before);
    case 'j':
    case KEY_DOWN:
        return viewer_navigate(viewer,
                               navigate_down(&viewer->top_cell,
                                             &viewer->selected_cell,
                                             &viewer->presented_matrix,
                                             viewer->file->lines),
                               &top_before,
                               &cursor_before);
    case KEY_HOME:
        return viewer_navigate(viewer,
                               navigate_row_start(&viewer->top_cell, &viewer->selected_cell, viewer->file->columns),
                               &top_before,
                               &cursor_before);
    case KEY_END:
        return viewer_navigate(viewer,
                               navigate_row_end(&viewer->top_cell,
                                                &viewer->selected_cell,
                                                &viewer->presented_matrix,
                                                viewer->file->columns),
                               &top_before,
                               &cursor_before);
    case KEY_NPAGE:
    case 6:
        return viewer_navigate(viewer,
                               navigate_page_down(&viewer->top_cell,
                                                  &viewer->selected_cell,
                                                  &viewer->presented_matrix,
                                                  viewer->file->lines),
                               &top_before,
                               &cursor_before);
    case KEY_PPAGE:
    case 2:
        return viewer_navigate(viewer,
                               navigate_page_up(&viewer->top_cell, &viewer->selected_cell, &viewer->presented_matrix),
                               &top_before,
                               &cursor_before);
    case 'g':
        return viewer_navigate(viewer, navigate_first_row(&viewer->top_cell, &viewer->selected_cell), &top_before, &cursor_before);
    case 'G':
        return viewer_navigate(viewer,
                               navigate_last_row(&viewer->top_cell, &viewer->selected_cell, viewer->file->lines),
                               &top_before,
                               &cursor_before);
    case 1:
        return viewer_navigate(viewer,
                               navigate_page_left(&viewer->top_cell, &viewer->selected_cell, &viewer->presented_matrix),
                               &top_before,
                               &cursor_before);
    case 5:
        return viewer_navigate(viewer,
                               navigate_page_right(&viewer->top_cell,
                                                   &viewer->selected_cell,
                                                   &viewer->presented_matrix,
                                                   viewer->file->columns),
                               &top_before,
                               &cursor_before);
    case 'q':
        viewer_request_quit();
        return PAINT_NONE;
    case 'i':
    case KEY_IC:
        return viewer_enter_insert(viewer);
    case ':':
        input_mode_set(INPUT_COMMAND);
        viewer->input_buffer[0] = '\0';
        status_bar_draw_input(INPUT_COMMAND, ":", viewer->input_buffer);
        return PAINT_NONE;
    case '/':
        input_mode_set(INPUT_SEARCH);
        viewer->input_buffer[0] = '\0';
        status_bar_draw_input(INPUT_SEARCH, "/", viewer->input_buffer);
        return PAINT_NONE;
    case '?':
        input_mode_set(INPUT_HELP);
        viewer_show_help();
        return PAINT_NONE;
    case 'n':
        viewer_search_next_cb();
        return PAINT_NONE;
    case 'N':
        viewer_search_prev_cb();
        return PAINT_NONE;
    case 10:
    case KEY_ENTER:
        viewer_update_cell_preview(viewer);
        viewer_draw_status(viewer);
        return PAINT_NONE;
    default:
        return PAINT_NONE;
    }
}

static paint_action_t viewer_handle_line_input(csvi_viewer_t *viewer,
                                               int key,
                                               input_mode_t mode,
                                               const char *prefix,
                                               bool execute_on_enter)
{
    if (key == 27)
    {
        input_mode_set(INPUT_NORMAL);
        viewer_clear_status(viewer);
        return PAINT_FULL;
    }

    if (key == KEY_BACKSPACE || key == 127)
    {
        size_t len = strlen(viewer->input_buffer);
        if (len > 0)
        {
            viewer->input_buffer[len - 1] = '\0';
        }
        status_bar_draw_input(mode, prefix, viewer->input_buffer);
        return PAINT_NONE;
    }

    if (key == 10 || key == KEY_ENTER)
    {
        input_mode_set(INPUT_NORMAL);
        if (execute_on_enter)
        {
            if (mode == INPUT_SEARCH)
            {
                if (viewer->search && csvi_search_find(viewer->search, viewer->input_buffer))
                {
                    viewer_search_next_cb();
                }
                else
                {
                    viewer_show_error("pattern not found");
                }
            }
            else
            {
                char command[280];
                snprintf(command, sizeof(command), ":%s", viewer->input_buffer);
                commands_execute(command);
            }
        }
        viewer->input_buffer[0] = '\0';
        return PAINT_NONE;
    }

    if (key > 0 && key < 256)
    {
        size_t len = strlen(viewer->input_buffer);
        if (len + 1 < sizeof(viewer->input_buffer))
        {
            viewer->input_buffer[len] = (char)key;
            viewer->input_buffer[len + 1] = '\0';
            status_bar_draw_input(mode, prefix, viewer->input_buffer);
        }
    }

    return PAINT_NONE;
}

static paint_action_t viewer_on_key(int key)
{
    csvi_viewer_t *viewer = active_viewer;
    paint_action_t action = PAINT_NONE;

    if (!viewer)
    {
        return PAINT_NONE;
    }

    if (key == 0)
    {
        action = PAINT_FULL;
        viewer_apply_paint(viewer, action);
        return action;
    }

    switch (input_mode_get())
    {
    case INPUT_COMMAND:
        action = viewer_handle_line_input(viewer, key, INPUT_COMMAND, ":", true);
        break;
    case INPUT_SEARCH:
        action = viewer_handle_line_input(viewer, key, INPUT_SEARCH, "/", true);
        break;
    case INPUT_INSERT:
        action = viewer_handle_insert(viewer, key);
        break;
    case INPUT_HELP:
        if (key == 27 || key == 'q')
        {
            input_mode_set(INPUT_NORMAL);
            action = PAINT_FULL;
        }
        break;
    default:
        action = viewer_handle_normal(viewer, key);
        break;
    }

    if (action != PAINT_NONE)
    {
        viewer_apply_paint(viewer, action);
    }

    return action;
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
    viewer->viewport_cells.width = 1;
    viewer->viewport_cells.height = 1;
    viewer->properties = (matrix_properties_t){
        .cell_padding_top = 0,
        .cell_padding_right = 1,
        .cell_padding_bottom = 0,
        .cell_padding_left = 1,
        .margin_top = 0,
        .margin_right = 0,
        .margin_bottom = 1,
        .margin_left = 0};
    viewer->separator = ';';
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
        viewer->separator = opts->separator;
        viewer->display.grid_enabled = opts->grid;
        viewer->header_enabled = opts->header;
        viewer->display.header_enabled = opts->header;

        bool color = false;
        if (opts->color_mode == CSVI_COLOR_ALWAYS)
        {
            color = true;
        }
        else if (opts->color_mode == CSVI_COLOR_AUTO && getenv("NO_COLOR") == NULL)
        {
            color = true;
        }
        viewer->display.color_enabled = color;

        if (opts->file_path)
        {
            viewer->file_path = strdup(opts->file_path);
        }
        csv_reader_set_separator(viewer->separator);
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
    viewer->cache = viewport_cache_build(file);
    viewer->search = csvi_search_create(file);
    if (!viewer->file_path)
    {
        viewer->file_path = strdup(path);
    }

    if (viewer->header_enabled && viewer->file->lines > 1)
    {
        viewer->top_cell.y = 1;
        viewer->selected_cell.y = 1;
    }

    csvi_log_info("opened '%s' (%zu lines, %zu columns)\n", path, file->lines, file->columns);
    viewer->read_only = access(path, W_OK) != 0;
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
        .search_next = viewer_search_next_cb,
        .search_prev = viewer_search_prev_cb,
        .set_separator = viewer_set_separator_cb,
        .set_header = viewer_set_header_cb,
        .save_file = viewer_save_cb,
        .show_error = viewer_show_error,
        .exit = viewer_exit_cb,
        .exit_force = viewer_exit_force_cb};

    commands_init(&command_executors);
    matrix_presentation_init(&viewer->display);

    const screen_size_t *scr = matrix_presentation_get_screen_size();
    status_bar_init(scr->width, scr->height - 1);

    matrix_presentation_run(viewer_on_key);
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

    if (viewer->viewport_config)
    {
        matrix_config_dispose(viewer->viewport_config);
    }
    if (viewer->cache)
    {
        viewport_cache_dispose(viewer->cache);
    }
    if (viewer->search)
    {
        csvi_search_dispose(viewer->search);
    }
    free(viewer->file_path);
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
    viewer->cache = viewport_cache_build(file);
    return viewer;
}

int csvi_viewer_test_commit_at(csvi_viewer_t *viewer, size_t x, size_t y, const char *value)
{
    if (!viewer || !viewer->file || !viewer->cache)
    {
        return -1;
    }

    csv_token *token = csv_reader_get_token(x, y, viewer->file);
    const char *previous = (token && token->data) ? token->data : "";
    bool changed = strcmp(previous, value ? value : "") != 0;

    if (csv_reader_set_cell(viewer->file, x, y, value) != 0)
    {
        return -1;
    }

    viewport_cache_update_cell(viewer->cache, viewer->file, x, y);
    if (changed)
    {
        viewer->file_modified = true;
    }

    return 0;
}

bool csvi_viewer_file_modified(const csvi_viewer_t *viewer)
{
    return viewer && viewer->file_modified;
}

const coordinates_t *csvi_viewer_selected_cell(const csvi_viewer_t *viewer)
{
    return &viewer->selected_cell;
}

void csvi_viewer_exec_go_to_last_line(csvi_viewer_t *viewer)
{
    viewer_jump_to(viewer, viewer->selected_cell.x, viewer->file->lines - 1);
}

void csvi_viewer_exec_go_to_last_column(csvi_viewer_t *viewer)
{
    viewer_jump_to(viewer, viewer->file->columns - 1, viewer->selected_cell.y);
}
#endif
