#include "logger.h"

t_log* ioLogger;


void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level)
{
    ioLogger = log_create(file, processName, isActiveConsole, level);
}

void destroyLogger()
{
    log_destroy(ioLogger);
}