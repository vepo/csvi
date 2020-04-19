#include <stdio.h>
#include <stdarg.h>

#include "logger.h"

void log_info(char *message, ...)
{
    va_list args;
    va_start(args, message);
    FILE *fp = fopen("csvviewer.log", "a+");
    vfprintf(fp, message, args);
    va_end(args);
    fclose(fp);
}