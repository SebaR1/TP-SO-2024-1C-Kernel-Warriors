#ifndef SHORT_TERM_PLANNING_H_
#define SHORT_TERM_PLANNING_H_

#include "utilsKernel/mutexUtils.h"
#include "utils/utilsGeneral.h"
#include "generalPlanning.h"
#include "utilsKernel/logger.h"
#include "utilsKernel/config.h"

//Hilo para el estado de ready
void readyState();

//a
char* _listPids(t_list *list);

void initShortTermPlanning();

#endif