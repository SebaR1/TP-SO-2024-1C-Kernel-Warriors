#ifndef UTILS_LIST_MUTEX_H
#define UTILS_LIST_MUTEX_H

#include <commons/collections/list.h>
#include <pthread.h>
#include <stdlib.h>


// Lista que solo puede ser accedida o modificada por un hilo o proceso a la vez/
typedef struct 
{
	t_list *list;
	pthread_mutex_t mutex;
} listMutex_t;



/// @brief Retorna el primer valor encontrado que haga que la condicion retorne true
/// @param list La lista mutex
/// @param closure La condicion que debe retornar true
/// @return Retorna el primer valor encontrado que cumpla con la condicion.
void* list_find_mutex(listMutex_t* list, bool(*closure)(void*));


/// @brief Agrega un elemento al final de la lista.
/// @param list La lista.
/// @param info El elemento a agregar.
void list_push(listMutex_t *list, void *info);


/// @brief Remueve el primer elemento de la lista de una determinada posicion y lo retorna.
/// @param list La lista
/// @return Retorna el elemento eliminado.
void* list_pop(listMutex_t *list);



/// @brief Crea e inicializa una lista mutex.
/// @return Retorna la lista mutex inicializada.
listMutex_t* initListMutex();



/// @brief Destruye una lista y su mutex sin liberar los elementos contenidos en los nodos
/// @param list La lista.
void destroyListMutex(listMutex_t* list);

#endif