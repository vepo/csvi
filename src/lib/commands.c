#include "commands.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

regex_t regex_go_to_line;
regex_t regex_go_to_column;
regex_t regex_go_to_cell;

command_executors_t *_executors = NULL;

void commands_init(command_executors_t *executors)
{
    _executors = executors;
    regcomp(&regex_go_to_line, "^\\:([[:digit:]]+)$", REG_EXTENDED);
    regcomp(&regex_go_to_column, "^\\:c([[:digit:]]+)$", REG_EXTENDED);
    regcomp(&regex_go_to_cell, "^\\:([[:digit:]]+)x([[:digit:]]+)$", REG_EXTENDED);
}

void commands_shutdown()
{
    regfree(&regex_go_to_line);
    regfree(&regex_go_to_column);
    regfree(&regex_go_to_cell);
}

int regmatch2int(regmatch_t *match, char *command)
{
    size_t input_length = match->rm_eo - match->rm_so;
    char *s_value = (char *)malloc(input_length + 1);
    s_value[input_length] = '\0';
    strncpy(s_value, &command[match->rm_so], input_length);
    int value = atoi(s_value) - 1; // zero index
    free(s_value);
    return value;
}

void process_command_go_to_line(char *command, regmatch_t *pmatch)
{
    _executors->go_to_line(regmatch2int(&pmatch[1], command));
}

void process_command_go_to_column(char *command, regmatch_t *pmatch)
{
    _executors->go_to_column(regmatch2int(&pmatch[1], command));
}

void process_command_go_to_cell(char *command, regmatch_t *pmatch)
{
    _executors->go_to_cell(regmatch2int(&pmatch[1], command),
                           regmatch2int(&pmatch[2], command));
}

void commands_execute(char *command)
{
    if (command[0] == ':')
    {
        if (strcmp(":q", command) == 0)
        {
            _executors->exit(0);
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
    _executors->show_error(error_message);
}