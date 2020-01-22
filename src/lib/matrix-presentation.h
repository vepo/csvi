#ifndef MATRIX_PRESENTATION_H_
#define MATRIX_PRESENTATION_H_

#include "actions.h"

void matrix_presentation_init();
void matrix_presentation_configure_handler(Action action, void (*callback)());
void matrix_presentation_exit();
void matrix_presentation_handle();

#endif