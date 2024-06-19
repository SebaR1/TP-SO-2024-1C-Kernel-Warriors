#ifndef CPU_LOGGER_H_
#define CPU_LOGGER_H_


#include <commons/log.h>
#include <stdint.h>
#include "cpuDebug.h"



///////////////////// FUNCIONES PARA MANEJO DE LOGS /////////////////////


// Inicializa el logger
void initLogger(char* file, char* processName, bool isActiveConsole, t_log_level level);

// Retorna el logger de este modulo
t_log* getLogger();

// Destruye el logger
void destroyLogger();




///////////////////// LOGS ESPECIFICOS /////////////////////


void logFetchInstruction(int pid, uint32_t pc);

void logExecutingInstruction(int pid, char* instruction);

void logTLBHit(int pid, int page);

void logTLBMiss(int pid, int page);

void logGetFrame(int pid, int page, int frame);

void logReadMemoryUint(int pid, int physicalAddress, uint32_t value);

void logReadMemoryString(int pid, int physicalAddress, char* value);

void logWriteMemoryUint(int pid, int physicalAddress, uint32_t value);

void logWriteMemoryString(int pid, int physicalAddress, char* value);




////////////////////// LOGS AUXILIARES GENERALES //////////////////////



#ifndef LINE_SEPARATOR
#define LINE_SEPARATOR "----------------------------------------------------"
#endif


void logLineSeparator();




////////////////////// LOGS ESPECIFICOS PARA EL DEBUG O REALESE //////////////////////


#ifdef DEBUG_CPU


void logPreInitialMessageDebug();

void logInitialMessageDebug();


#else


void logInitialMessageRealese();



#endif






#endif