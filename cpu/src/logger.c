#include "logger.h"

t_log* cpuLogger;


void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level)
{
    cpuLogger = log_create(file, processName, isActiveConsole, level);
}

void destroyLogger()
{
    log_destroy(cpuLogger);
}