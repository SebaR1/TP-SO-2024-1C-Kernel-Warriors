#ifndef CPU_LOGGER_H_
#define CPU_LOGGER_H_


#include <commons/log.h>


extern t_log* cpuLogger;

// Inicializa el logger
void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level);

// Destruye el logger
void destroyLogger();



#endif