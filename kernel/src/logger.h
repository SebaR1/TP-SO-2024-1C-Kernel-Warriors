#ifndef KERNEL_LOGGER_H_
#define KERNEL_LOGGER_H_


#include <commons/log.h>


extern t_log* kernelLogger;

// Inicializa el logger
void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level);

// Destruye el logger
void destroyLogger();



#endif