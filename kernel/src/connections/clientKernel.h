#ifndef CLIENT_KERNEL
#define CLIENT_KERNEL

#include <commons/collections/list.h>
#include <semaphore.h>
#include "utils/client/utils.h"
#include "utilsKernel/logger.h"
#include "utilsKernel/config.h"
#include <pthread.h>
#include <semaphore.h>


/// @brief Le envia el contexto del proceso a la cpu para que ejecute.
/// @param process El pcb del proceso
void sendContextToCPU(pcb_t *process);

#endif