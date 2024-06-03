
#ifndef GENERAL_PLANNING_H
#define GENERAL_PLANNING_H

#include <commons/collections/list.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include "utils/utilsGeneral.h"
#include "utils/listMutex.h"
#include <commons/temporal.h>
#include "kernel.h"

extern listMutex_t *pcbNewList;
extern listMutex_t *pcbReadyList;
extern listMutex_t *pcbExecList;
extern listMutex_t *pcbBlockList;
extern listMutex_t *pcbExitList;

extern listMutex_t *resourcesBlockList;

extern sem_t semNew;
extern sem_t semExit;
extern sem_t semReady;
extern sem_t semBlock;
extern sem_t semExec;

extern sem_t semMultiProgramming;
extern sem_t semMultiProcessing;
extern sem_t semAddPid;

extern sem_t semQuantum;

extern int pid;

extern t_algorithm algorithm;

extern int socketClientMemory;
extern int socketClientCPUDispatch;
extern int socketClientCPUInterrupt;

#endif
