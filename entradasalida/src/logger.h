#ifndef IO_LOGGER_H_
#define IO_LOGGER_H_


#include <commons/log.h>


extern t_log* _ioLogger;

// Inicializa el logger
void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level);

// Retorna el logger de este modulo
t_log* getIOLogger();

// Destruye el logger
void destroyLogger();



#endif