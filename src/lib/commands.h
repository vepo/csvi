#ifndef COMMANDS_H_
#define COMMANDS_H_

enum Commands
{
    NO_COMMAND = 1,
    GO_TO_LINE = 2,
    GO_TO_COLUMN = 3,
    EXIT = 98,
    UNKNOWN = 99
};

typedef enum Commands Command;

typedef struct command_executors
{
    void (*go_to_line)(size_t);
    void (*go_to_column)(size_t);
    void (*exit)(int);
    void (*show_error)(char *error_message);
} command_executors_t;

void commands_init(command_executors_t *executors);
void commands_execute(char *command);
void commands_shutdown();
#endif