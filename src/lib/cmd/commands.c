#include "cmd/commands.h"
#include "cmd/cell-commands.h"

void commands_init(command_executors_t *executors)
{
    cell_commands_init(executors);
}

void commands_shutdown(void)
{
    cell_commands_shutdown();
}

void commands_execute(char *command)
{
    cell_commands_execute(command);
}
