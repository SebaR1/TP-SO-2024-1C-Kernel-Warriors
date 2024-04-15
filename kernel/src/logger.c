#include "logger.h"

t_log* kernelLogger;


void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level)
{
    kernelLogger = log_create(file, processName, isActiveConsole, level);
}

void destroyLogger()
{
    log_destroy(kernelLogger);
}