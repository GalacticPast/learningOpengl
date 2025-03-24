#pragma once

typedef enum log_levels
{
    LOG_LEVEL_FATAL,
    LOG_LEVEL_DERROR,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_TRACE,
    LOG_LEVEL_MAX_LEVEL
} log_levels;

void log_message(log_levels level, const char *msg, ...);

#define DFATAL(msg, ...) log_message(LOG_LEVEL_FATAL, msg, ##__VA_ARGS__)
#define DERROR(msg, ...) log_message(LOG_LEVEL_DERROR, msg, ##__VA_ARGS__)

#ifdef _DEBUG
#define DEBUG(msg, ...) log_message(LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)
#else
#define DEBUG(msg, ...)
#endif

#define DWARN(msg, ...) log_message(LOG_LEVEL_WARN, msg, ##__VA_ARGS__)
#define DINFO(msg, ...) log_message(LOG_LEVEL_INFO, msg, ##__VA_ARGS__)
#define DTRACE(msg, ...) log_message(LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)
