#ifndef HELPER_H_
#define HELPER_H_

#include <stdio.h>

#define CHECK_FATAL_FN(condition, message, exit_fn) \
    if (condition)                                  \
    {                                               \
        fprintf(stderr, message);                   \
        exit_fn();                                  \
        exit(-1);                                   \
    }

#define CHECK_FATAL(condition, message) \
    if (condition)                      \
    {                                   \
        fprintf(stderr, message);       \
        exit(-1);                       \
    }

//#ifdef LOGGER_INFO_ENABLE
#define LOGGER_INFO(message, ...)                       \
    {                                                   \
        FILE *logger_fp = fopen("csvviewer.log", "a+"); \
        fprintf(logger_fp, message, ##__VA_ARGS__);     \
        fclose(logger_fp);                              \
    }
//#else
//#define LOGGER_INFO(message, ...)
//#endif

#endif