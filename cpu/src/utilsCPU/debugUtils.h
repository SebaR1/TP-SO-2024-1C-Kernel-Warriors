#ifndef DEBUG_UTILS_H_
#define DEBUG_UTILS_H_


#include "cpuDebug.h"


#ifdef DEBUG_CPU

#include "instructionCycle/essentials.h"



/// @brief Muestra todos los registros si se escribe en la consola "regs" o "registers"
void printRegisters();


/// @brief Thread que muestra todos los registros si se escribe en la consola "regs" o "registers". Tambien da ayuda si se escribe "help" o "h"
void printRegistersThread(void* ignore);

#endif








#endif