#include "listMutex.h"

int list_mutex_size(listMutex_t *list)
{
	pthread_mutex_lock(&(list->mutex));
	int size = list_size(list->list);
	pthread_mutex_unlock(&(list->mutex));
	return size;
}


bool list_mutex_is_empty(listMutex_t *list)
{
	pthread_mutex_lock(&(list->mutex));
	bool is_empty = list_is_empty(list->list);
	pthread_mutex_unlock(&(list->mutex));
	return is_empty;
}

void* list_find_mutex(listMutex_t* list, bool(*closure)(void*))
{
    pthread_mutex_lock(&(list->mutex));
    void* info = list_find(list->list, closure);
	pthread_mutex_unlock(&(list->mutex));
    return info;
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


void* list_remove_by_condition_mutex(listMutex_t *list, bool(*condition)(void*))
{
    pthread_mutex_lock(&(list->mutex));
	void *info = list_remove_by_condition(list->list, condition);
	pthread_mutex_unlock(&(list->mutex));
	return info;
}



void destroyListMutex(listMutex_t* list)
{
   	pthread_mutex_lock(&(list->mutex)); 
    list_destroy(list->list);
    pthread_mutex_unlock(&(list->mutex));
    pthread_mutex_destroy(&(list->mutex));
    free(list);
}
