#ifndef CSVI_MATRIX_PRESENTATION_H_
#define CSVI_MATRIX_PRESENTATION_H_

#include <stdbool.h>

#include "layout/matrix-config.h"
#include "layout/types.h"
#include "ui/paint.h"

typedef struct matrix_display_options
{
    bool color_enabled;
    bool grid_enabled;
    bool header_enabled;
} matrix_display_options_t;

void matrix_presentation_init(const matrix_display_options_t *options);
void matrix_presentation_shutdown(void);
void matrix_presentation_exit(void);
void matrix_presentation_request_stop(void);

screen_size_t *matrix_presentation_get_screen_size(void);
screen_size_t *matrix_presentation_get_grid_size(const matrix_properties_t *properties);

void matrix_presentation_clear_grid(const matrix_properties_t *properties);
void matrix_presentation_draw_cell(const coordinates_t *viewport_pos,
                                   size_t abs_x,
                                   size_t abs_y,
                                   const char *data,
                                   bool selected,
                                   bool search_match,
                                   const matrix_config_t *config,
                                   const matrix_properties_t *properties);

void matrix_presentation_beep(void);
void matrix_presentation_refresh_partial(void);

typedef paint_action_t (*matrix_key_handler_t)(int key);
void matrix_presentation_run(matrix_key_handler_t handler);

#endif
