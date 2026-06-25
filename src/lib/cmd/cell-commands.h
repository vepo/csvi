#ifndef CSVI_CELL_COMMANDS_H_
#define CSVI_CELL_COMMANDS_H_

#include "cmd/commands.h"

void cell_commands_init(command_executors_t *executors);
void cell_commands_execute(const char *command);
void cell_commands_shutdown(void);

#endif
