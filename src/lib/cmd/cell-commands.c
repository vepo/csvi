#include "cmd/cell-commands.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static command_executors_t *executors = NULL;

static char *trim(char *s)
{
    while (*s && isspace((unsigned char)*s))
    {
        ++s;
    }
    if (*s == 0)
    {
        return s;
    }
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end))
    {
        *end-- = '\0';
    }
    return s;
}

static long parse_user_number(const char *text)
{
    char *end = NULL;
    long value = strtol(text, &end, 10);
    if (end == text || *end != '\0' || value < 1)
    {
        return -1;
    }
    return value - 1;
}

static void dispatch_go_line(long line)
{
    if (line >= 0)
    {
        executors->go_to_line((size_t)line);
    }
    else
    {
        executors->show_error("invalid line number");
    }
}

void cell_commands_init(command_executors_t *execs)
{
    executors = execs;
}

void cell_commands_shutdown(void)
{
    executors = NULL;
}

void cell_commands_execute(const char *command_raw)
{
    char buffer[256];
    strncpy(buffer, command_raw, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char *command = trim(buffer);
    if (command[0] != ':')
    {
        executors->show_error("commands must start with ':'");
        return;
    }

    ++command;
    command = trim(command);
    if (command[0] == '\0')
    {
        return;
    }

    if (strcmp(command, "q") == 0 || strcmp(command, "quit") == 0)
    {
        executors->exit(0);
        return;
    }

    if (strcmp(command, "top") == 0)
    {
        executors->go_to_first_line();
        return;
    }

    if (strcmp(command, "bottom") == 0)
    {
        executors->go_to_last_line();
        return;
    }

    if (strcmp(command, "left") == 0)
    {
        executors->go_to_first_column();
        return;
    }

    if (strcmp(command, "right") == 0)
    {
        executors->go_to_last_column();
        return;
    }

    if (strcmp(command, "n") == 0 || strcmp(command, "next") == 0)
    {
        if (executors->search_next)
        {
            executors->search_next();
        }
        return;
    }

    if (strcmp(command, "N") == 0 || strcmp(command, "prev") == 0)
    {
        if (executors->search_prev)
        {
            executors->search_prev();
        }
        return;
    }

    if (strncmp(command, "line ", 5) == 0)
    {
        dispatch_go_line(parse_user_number(command + 5));
        return;
    }

    if (strncmp(command, "col ", 4) == 0)
    {
        long col = parse_user_number(command + 4);
        if (col >= 0)
        {
            executors->go_to_column((size_t)col);
        }
        else
        {
            executors->show_error("invalid column number");
        }
        return;
    }

    if (strncmp(command, "cell ", 5) == 0)
    {
        char *args = trim(command + 5);
        char *comma = strchr(args, ',');
        if (!comma)
        {
            executors->show_error("usage: :cell ROW,COL");
            return;
        }
        *comma = '\0';
        long row = parse_user_number(trim(args));
        long col = parse_user_number(trim(comma + 1));
        if (row >= 0 && col >= 0)
        {
            executors->go_to_cell((size_t)col, (size_t)row);
        }
        else
        {
            executors->show_error("invalid cell address");
        }
        return;
    }

    if (strncmp(command, "set ", 4) == 0)
    {
        char *args = trim(command + 4);
        if (strncmp(args, "sep=", 4) == 0 && args[4] != '\0')
        {
            if (executors->set_separator)
            {
                executors->set_separator(args[4]);
            }
            return;
        }
        if (strcmp(args, "header=on") == 0)
        {
            if (executors->set_header)
            {
                executors->set_header(true);
            }
            return;
        }
        if (strcmp(args, "header=off") == 0)
        {
            if (executors->set_header)
            {
                executors->set_header(false);
            }
            return;
        }
        executors->show_error("unknown :set option");
        return;
    }

    if (strchr(command, ',') != NULL)
    {
        char *comma = strchr(command, ',');
        *comma = '\0';
        long row = parse_user_number(trim(command));
        long col = parse_user_number(trim(comma + 1));
        if (row >= 0 && col >= 0)
        {
            executors->go_to_cell((size_t)col, (size_t)row);
            return;
        }
    }

    if (isdigit((unsigned char)command[0]))
    {
        dispatch_go_line(parse_user_number(command));
        return;
    }

    char error_message[255];
    snprintf(error_message, sizeof(error_message), "unknown command: :%s", command);
    executors->show_error(error_message);
}
