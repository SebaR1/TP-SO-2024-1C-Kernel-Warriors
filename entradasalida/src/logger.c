#include "logger.h"

t_log* _ioLogger;


void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level)
{
    _ioLogger = log_create(file, processName, isActiveConsole, level);
}

t_log* getIOLogger()
{
    return _ioLogger;
}

void destroyLogger()
{
    log_destroy(_ioLogger);
}