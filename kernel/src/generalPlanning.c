#include "generalPlanning.h"

listMutex_t *pcbNewList;
listMutex_t *pcbReadyList;
listMutex_t *pcbExecList;
listMutex_t *pcbBlockList;
listMutex_t *pcbExitList;

sem_t semNew;
sem_t semExit;
sem_t semReady;
sem_t semBlock;
sem_t semExec;

sem_t semMultiProgramming;
sem_t semMultiProcessing;
sem_t semAddPid;

sem_t semQuantum;

int pid;

t_algorithm algorithm;

int socketClientMemory;
int socketClientCPUDispatch;
int socketClientCPUInterrupt;