#ifndef CSVI_HELPER_H_
#define CSVI_HELPER_H_

#include <stdio.h>
#include <stdlib.h>

#include "common/errors.h"

#define NORETURN __attribute__((noreturn))

#define CHECK_FATAL(condition, message)          \
    do                                           \
    {                                            \
        if (condition)                           \
        {                                        \
            fprintf(stderr, "csvi: error: ");    \
            fprintf(stderr, message);            \
            exit(CSVI_EXIT_ERR);                 \
        }                                        \
    } while (0)

#define CHECK_FATAL_FN(condition, message, exit_fn) \
    do                                              \
    {                                               \
        if (condition)                              \
        {                                           \
            fprintf(stderr, "csvi: error: ");       \
            fprintf(stderr, message);               \
            exit_fn();                              \
            exit(CSVI_EXIT_ERR);                    \
        }                                           \
    } while (0)

#endif
