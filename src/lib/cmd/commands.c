#include "cmd/commands.h"

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static regex_t regex_go_to_line;
static regex_t regex_go_to_column;
static regex_t regex_go_to_cell;

static command_executors_t *executors = NULL;

static int regmatch2int(const regmatch_t *match, const char *command)
{
    size_t input_length = match->rm_eo - match->rm_so;
    char *s_value = (char *)malloc(input_length + 1);
    s_value[input_length] = '\0';
    strncpy(s_value, &command[match->rm_so], input_length);
    int value = atoi(s_value) - 1;
    free(s_value);
    return value;
}

static void process_command_go_to_line(const char *command, const regmatch_t *pmatch)
{
    executors->go_to_line(regmatch2int(&pmatch[1], command));
}

static void process_command_go_to_column(const char *command, const regmatch_t *pmatch)
{
    executors->go_to_column(regmatch2int(&pmatch[1], command));
}

static void process_command_go_to_cell(const char *command, const regmatch_t *pmatch)
{
    executors->go_to_cell(regmatch2int(&pmatch[1], command),
                          regmatch2int(&pmatch[2], command));
}

void commands_init(command_executors_t *execs)
{
    executors = execs;
    regcomp(&regex_go_to_line, "^\\:([[:digit:]]+)$", REG_EXTENDED);
    regcomp(&regex_go_to_column, "^\\:c([[:digit:]]+)$", REG_EXTENDED);
    regcomp(&regex_go_to_cell, "^\\:([[:digit:]]+)x([[:digit:]]+)$", REG_EXTENDED);
}

void commands_shutdown(void)
{
    regfree(&regex_go_to_line);
    regfree(&regex_go_to_column);
    regfree(&regex_go_to_cell);
    executors = NULL;
}

void commands_execute(char *command)
{
    if (command[0] == ':')
    {
        if (strcmp(":q", command) == 0)
        {
            executors->exit(0);
            return;
        }

        if (strcmp(":0", command) == 0)
        {
            executors->go_to_first_line();
            return;
        }

        if (strcmp(":-1", command) == 0)
        {
            executors->go_to_last_line();
            return;
        }

        if (strcmp(":c0", command) == 0)
        {
            executors->go_to_first_column();
            return;
        }

        if (strcmp(":c-1", command) == 0)
        {
            executors->go_to_last_column();
            return;
        }

        regmatch_t pmatch[3];
        if (!regexec(&regex_go_to_line, command, 2, pmatch, 0))
        {
            process_command_go_to_line(command, pmatch);
            return;
        }

        if (!regexec(&regex_go_to_column, command, 2, pmatch, 0))
        {
            process_command_go_to_column(command, pmatch);
            return;
        }

        if (!regexec(&regex_go_to_cell, command, 3, pmatch, 0))
        {
            process_command_go_to_cell(command, pmatch);
            return;
        }
    }

    char error_message[255];
    snprintf(error_message, 255, "Unknown command: %s", command);
    executors->show_error(error_message);
}
