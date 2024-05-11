#ifndef MUTEX_UTILS_H
#define MUTEX_UTILS_H

#include <commons/collections/list.h>
#include <pthread.h>
#include "utils/utilsGeneral.h"

void list_push(listMutex_t *list, void *info);

void* list_pop(listMutex_t *list);

listMutex_t* initListMutex();

void destroyListMutex(listMutex_t* list);

#endif