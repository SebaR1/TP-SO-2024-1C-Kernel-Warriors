#include "logger.h"

t_log* _kernelLogger;


void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level)
{
    _kernelLogger = log_create(file, processName, isActiveConsole, level);
}

t_log* getKernelLogger()
{
    return _kernelLogger;
}

void destroyLogger()
{
    log_destroy(_kernelLogger);
}