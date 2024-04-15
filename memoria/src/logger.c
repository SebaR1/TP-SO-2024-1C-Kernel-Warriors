#include "logger.h"

t_log* memoryLogger;


void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level)
{
    memoryLogger = log_create(file, processName, isActiveConsole, level);
}

void destroyLogger()
{
    log_destroy(memoryLogger);
}