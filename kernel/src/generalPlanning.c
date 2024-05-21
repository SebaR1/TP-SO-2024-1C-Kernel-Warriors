#include "generalPlanning.h"

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

sem_t semMultiProgramming;
sem_t semMultiProcessing;
sem_t semAddPid;

int pid;
