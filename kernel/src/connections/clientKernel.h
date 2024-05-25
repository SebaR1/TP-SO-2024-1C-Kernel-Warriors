#ifndef CLIENT_KERNEL
#define CLIENT_KERNEL

#include <commons/collections/list.h>
#include <semaphore.h>
#include "utils/client/utils.h"
#include "utilsKernel/logger.h"
#include "utilsKernel/config.h"
#include <pthread.h>
#include <semaphore.h>
#include "../kernel.h"


/// @brief Le envia el contexto del proceso a la cpu para que ejecute.
/// @param process El pcb del proceso
void sendContextToCPU(pcb_t *process);


/// @brief Le envia a la memoria el pid del proceso y el path dado por la consola que va a tener 
// la ubicacion de las instrucciones del proceso a ejecutar.
/// @param process El pcb del proceso que contiene su pid.
/// @param path ubicacion del archivo de pseudocodigo con las instrucciones.
void sendProcessPathToMemory(pcb_t *process, char* path);


/// @brief Le envia a CPUInterrupt la interrupcion de fin de Quantum.
void sendInterruptForQuantumEnd();


/// @brief Le envia a CPUInterrupt la interrupcion de finalizar proceso por la consola.
void sendInterruptForConsoleEndProcess();


#endif