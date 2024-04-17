#include "logger.h"

t_log* _memoryLogger;


void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level)
{
    _memoryLogger = log_create(file, processName, isActiveConsole, level);
}

t_log* getMemoryLogger()
{
    return _memoryLogger;
}

void destroyLogger()
{
    log_destroy(_memoryLogger);
}