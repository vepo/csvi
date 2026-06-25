#include "ui/input-modes.h"

static input_mode_t current_mode = INPUT_NORMAL;

input_mode_t input_mode_get(void)
{
    return current_mode;
}

void input_mode_set(input_mode_t mode)
{
    current_mode = mode;
}
