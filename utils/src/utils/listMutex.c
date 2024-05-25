#include "listMutex.h"


void* list_find_mutex(listMutex_t* list, bool(*closure)(void*))
{
    pthread_mutex_lock(&(list->mutex));
    void* info = list_find(list->list, closure);
	pthread_mutex_unlock(&(list->mutex));
}

void list_push(listMutex_t *list, void *info)
{
	pthread_mutex_lock(&(list->mutex));
	list_add(list->list, info);
	pthread_mutex_unlock(&(list->mutex));
}

void* list_pop(listMutex_t *list)
{
	pthread_mutex_lock(&(list->mutex));
	void *info = list_remove(list->list, 0);
	pthread_mutex_unlock(&(list->mutex));
	return info;
}

listMutex_t* initListMutex()
{
    listMutex_t *list;
    list = malloc(sizeof(listMutex_t));
    list->list = list_create();
    pthread_mutex_init(&(list->mutex), NULL);
    return list;
}

void destroyListMutex(listMutex_t* list)
{
   	pthread_mutex_lock(&(list->mutex)); 
    list_destroy(list->list);
    pthread_mutex_unlock(&(list->mutex));
    pthread_mutex_destroy(&(list->mutex));
    free(list);
}
