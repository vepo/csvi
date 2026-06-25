#include "common/log.h"

#include <stdio.h>
#include <stdarg.h>

static bool verbose_enabled = false;

void csvi_log_set_verbose(bool enabled)
{
    verbose_enabled = enabled;
}

bool csvi_log_is_verbose(void)
{
    return verbose_enabled;
}

void csvi_log_info(const char *fmt, ...)
{
    if (!verbose_enabled)
    {
        return;
    }

    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "csvi: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void csvi_log_error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "csvi: error: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}
