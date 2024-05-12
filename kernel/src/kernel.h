#ifndef KERNEL_H_
#define KERNEL_H_

#include "utils/client/utils.h"
#include "utils/server/utils.h"
#include "utilsKernel/config.h"
#include "utilsKernel/logger.h"
#include "serverKernel.h"
#include "console.h"
#include "utils/utilsGeneral.h"
#include "longTermPlanning.h"
#include "shortTermPlanning.h"

#include <pthread.h>
#include <commons/string.h>
#include <semaphore.h>
#include <readline/readline.h>
#include <commons/string.h>
#include <stdlib.h>


listMutex_t *pcbNewList;
listMutex_t *pcbReadyList;
listMutex_t *pcbExecList;
listMutex_t *pcbBLockList;
listMutex_t *pcbExitList;

sem_t semNew;
sem_t semExit;
sem_t semReady;
sem_t semBlock;
sem_t semExec;
sem_t semMulti;

sem_t semAddPid;

int pid;


#endif