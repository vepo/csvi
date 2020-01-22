#ifndef MATRIX_PRESENTATION_H_
#define MATRIX_PRESENTATION_H_

#include "actions.h"

typedef struct screen
{
    int width;
    int height;
} screen;

void matrix_presentation_init();
void matrix_presentation_configure_handler(Action action, void (*callback)());
void matrix_presentation_beep();
void matrix_presentation_flash();
void matrix_presentation_exit();
screen * matrix_presentation_get_screen();
void matrix_presentation_handle();

#endif