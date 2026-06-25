#ifndef CSVI_INPUT_MODES_H_
#define CSVI_INPUT_MODES_H_

typedef enum input_mode
{
    INPUT_NORMAL = 0,
    INPUT_COMMAND = 1,
    INPUT_SEARCH = 2,
    INPUT_HELP = 3
} input_mode_t;

input_mode_t input_mode_get(void);
void input_mode_set(input_mode_t mode);

#endif
