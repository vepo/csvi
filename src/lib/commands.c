#include "commands.h"
#include <regex.h>
#include <stdlib.h>
#include <string.h>

regex_t regex_go_to_line;
regex_t regex_go_to_column;

command_executors_t *_executors = NULL;

void commands_init(command_executors_t *executors)
{
    if (!executors)
    {
        printf("Error!");
        exit(-1);
    }
    _executors = executors;
    regcomp(&regex_go_to_line, "^\\:([[:digit:]]+)$", REG_EXTENDED);
    regcomp(&regex_go_to_column, "^\\:c([[:digit:]]+)$", REG_EXTENDED);
}

void commands_shutdown()
{
    regfree(&regex_go_to_line);
    regfree(&regex_go_to_column);
}

void process_command_go_to_line(char *command, regmatch_t *pmatch)
{
    size_t input_length = pmatch[1].rm_eo - pmatch[1].rm_so;
    char *go_to_line = (char *)malloc(input_length + 1);
    go_to_line[input_length] = '\0';
    strncpy(go_to_line, &command[pmatch[1].rm_so], input_length);
    int line = atoi(go_to_line) - 1; // zero index
    free(go_to_line);
    _executors->go_to_line(line);
}

void process_command_go_to_column(char *command, regmatch_t *pmatch)
{
    size_t input_length = pmatch[1].rm_eo - pmatch[1].rm_so;
    char *go_to_column = (char *)malloc(input_length + 1);
    go_to_column[input_length] = '\0';
    strncpy(go_to_column, &command[pmatch[1].rm_so], input_length);
    int column = atoi(go_to_column) - 1; // zero index
    free(go_to_column);
    _executors->go_to_column(column);
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

        regmatch_t pmatch[2];
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
    }

    char error_message[255];
    snprintf(error_message, 255, "Unknown command: %s", command);
    _executors->show_error(error_message);
}