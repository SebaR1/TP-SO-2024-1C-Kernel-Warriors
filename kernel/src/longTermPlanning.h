#ifndef LONG_TERM_PLANNING_H_
#define LONG_TERM_PLANNING_H_

#include "utilsKernel/mutexUtils.h"
#include "utils/utilsGeneral.h"
#include "generalPlanning.h"
#include "utilsKernel/logger.h"
#include "connections/clientKernel.h"
#include "connections/serverKernel.h"


//Controla la Lista NEW y los manda a la lista READY
void newState();

//Espera la llegade de un proceso a EXIT y lo termina
void exitState();

//Crea un proceso con su PCB
pcb_t* createProcess();

//Agrega un proceso a NEW enviando el Path a memoria
void addPcbToNew(char* path);

//Inicia el planificador a largo plazo
void initLongTermPlanning();

//Destruye el proceso y libera la memoria
void destroyProcess(pcb_t *process);

bool compare_pid(void *data);

void finalizar_proceso(uint32_t pid);


extern int pid_to_find;

#endif