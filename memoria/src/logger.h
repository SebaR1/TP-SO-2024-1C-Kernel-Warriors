#ifndef MEMORIA_LOGGER_H_
#define MEMORIA_LOGGER_H_


#include <commons/log.h>


extern t_log* _memoryLogger;

// Inicializa el logger
void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level);

// Retorna el logger de este modulo
t_log* getMemoryLogger();

// Destruye el logger
void destroyLogger();



#endif