#ifndef LONG_TERM_PLANNING_H_
#define LONG_TERM_PLANNING_H_

#include "utilsKernel/mutexUtils.h"
#include "utils/utilsGeneral.h"
#include "generalPlanning.h"
#include "utilsKernel/logger.h"


//Controla la Lista NEW y los manda a la lista READY
void newState();

//Espera la llegade de un proceso a EXIT y lo termina
void exitState();

//Crea un proceso con su PCB
pcb_t* createProcess();

//Agrega un proceso a NEW
void addPcbToNew();

//Inicia el planificador a largo plazo
void initLongTermPlanning();


#endif