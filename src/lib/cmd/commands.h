#ifndef CSVI_COMMANDS_H_
#define CSVI_COMMANDS_H_

#include <stddef.h>
#include <stdbool.h>

typedef struct command_executors
{
    void (*go_to_line)(size_t);
    void (*go_to_column)(size_t);
    void (*go_to_cell)(size_t, size_t);
    void (*go_to_first_line)(void);
    void (*go_to_last_line)(void);
    void (*go_to_first_column)(void);
    void (*go_to_last_column)(void);
    void (*search_next)(void);
    void (*search_prev)(void);
    void (*set_separator)(char);
    void (*set_header)(bool);
    void (*exit)(int);
    void (*show_error)(char *error_message);
} command_executors_t;

void commands_init(command_executors_t *executors);
void commands_execute(char *command);
void commands_shutdown(void);

#endif
