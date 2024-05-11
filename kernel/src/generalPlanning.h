
#ifndef GENERAL_PLANNING_H
#define GENERAL_PLANNING_H

#include <commons/collections/list.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include "utils/utilsGeneral.h"


extern listMutex_t *pcbNewList;
extern listMutex_t *pcbReadyList;
extern listMutex_t *pcbExecList;
extern listMutex_t *pcbBLockList;
extern listMutex_t *pcbExitList;

extern sem_t semNew;
extern sem_t semExit;
extern sem_t semReady;
extern sem_t semBlock;
extern sem_t semExec;
extern sem_t semMulti;

extern sem_t semAddPid;

extern int pid;


#endif
