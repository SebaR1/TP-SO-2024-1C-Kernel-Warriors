#include "logger.h"

t_log* _cpuLogger;


void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level)
{
    _cpuLogger = log_create(file, processName, isActiveConsole, level);
}

t_log* getCPULogger()
{
    return _cpuLogger;
}

void destroyLogger()
{
    log_destroy(_cpuLogger);
}