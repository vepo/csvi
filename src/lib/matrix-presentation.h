#ifndef MATRIX_PRESENTATION_H_
#define MATRIX_PRESENTATION_H_

#include "actions.h"
#include "matrix-config.h"

typedef struct screen_config
{
    int width;
    int height;
} screen_config_t;

void matrix_presentation_init();
void matrix_presentation_configure_handler(Action action, void (*callback)());
void matrix_presentation_beep();
void matrix_presentation_flash();
void matrix_presentation_exit();
void matrix_presentation_set_value(size_t x, size_t y, char *data, matrix_config_t *config);
screen_config_t *matrix_presentation_get_screen_config();
void matrix_presentation_handle();

#endif