#ifndef CSVI_STATUS_BAR_H_
#define CSVI_STATUS_BAR_H_

#include <stdbool.h>
#include <stddef.h>

#include "ui/input-modes.h"

void status_bar_init(int screen_width, int screen_row);
void status_bar_resize(int screen_width, int screen_row);
void status_bar_draw(const char *filename,
                     size_t row,
                     size_t total_rows,
                     size_t col,
                     size_t total_cols,
                     char separator,
                     input_mode_t mode,
                     bool file_modified,
                     const char *line_text,
                     const char *message);
void status_bar_draw_input(input_mode_t mode, const char *prefix, const char *input);

#endif
