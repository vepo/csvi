#ifndef MATRIX_PRESENTATION_H_
#define MATRIX_PRESENTATION_H_

#include "actions.h"
#include "matrix-config.h"

enum COLORS_SCHEME
{
    SELECTED_CELL = 1,
    EVEN_CELL = 2,
    ODD_CELL = 3
};

void matrix_presentation_init();
void matrix_presentation_configure_handler(Action action, void (*callback)());
void matrix_presentation_beep();
void matrix_presentation_flash();
void matrix_presentation_exit();
void matrix_presentation_set_value(coordinates_t *position,
                                   char *data,
                                   bool selected,
                                   matrix_config_t *config,
                                   matrix_properties_t *m_properties);
screen_config_t *matrix_presentation_get_screen_config();
void matrix_presentation_handle();

#endif