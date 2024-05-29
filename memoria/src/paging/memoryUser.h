#ifndef MEMORY_PAGING_MEMORY_USER_H_
#define MEMORY_PAGING_MEMORY_USER_H_


#include <stdbool.h>
#include <semaphore.h>







// Inicializa la memoria del usuario
void initMemoryUser();


/// @brief Retorna true si el frame especificado está ocupado
/// @param frame El frame a verificar.
bool isFrameBusy(int frame);



/// @brief Reserva la cantidad determinada de bytes en la memoria de usuario si no se produce un Out Of Memory. Si se intenta reservar 0 bytes,
/// La funcion retorna 0 y no hace nada mas.
/// @param bytes Cantidad de bytes a reservar (no se le tiene que pasar el numero total de bytes reservados por el proceso, solo la cantidad que se quiere expandir).
/// @param pages Se le debe pasar la tabla de paginas a expandir, o NULL si no hay tabla de paginas aun.
/// Modifica o crea la tabla de paginas, donde el indice es cada pagina,
/// y el contenido de cada indice es el numero de frame en la memoria de usuario.
/// Retorna el mismo valor que se le pasó por parametro si se intenta reservar 0 bytes, o indefinido si se produjo un Out Of Memory.
/// Usa realloc, es responsabilidad de quien usa esta funcion de liberar el espacio de memoria reservado para este puntero (tipicamente llamando a freeMemory).
/// @param amountOfPages Puntero a la cantidad de paginas que tiene el proceso actualmente (NO debe pasarse NULL).
/// Retorna la nueva cantidad de paginas, o indefinido si ocurre un Out Of Meemory
/// @param internalFragmentation Puntero a una direccion que contenga la fragmentacion interna que tiene el proceso actualmente (NO debe pasarse NULL).
/// Retorna la nueva fragmentacion interna, o indefinido si ocurre un Out Of Memory.
/// @return Retorna la cantidad de paginas reservadas (no la cantidad total de paginas reservadas por el proceso,
/// sino las que se reservaron al llamar a esta funcion), o -1 si se produjo un Out Of Memory
int allocMemory(int bytes, int* pages, int* const amountOfPages, int* const internalFragmentation);



/// @brief Retorna la cantidad de bytes que representan la fragmentacion interna de la ultima pagina de la memoria de usuario.
/// @param bytesAllocated La cantidad de bytes reservados por un proceso.
/// @return Retorna la fragmentacion interna.
int getInternalFragmentation(int bytesAllocated);



/// @brief Retorna la cantidad de bytes que están reservados en base a la cantidad de paginas y la fragmentacion interna
/// @param amountOfPages Cantidad de paginas reservadas
/// @param internalFragmentation Cantidad de bytes que representan la fragmentacion interna que quedó (o quedaría) en la ultima pagina
/// /// (es decir, la cantidad de bytes que faltan para completar la ultima pagina).
/// @return Retorna la cantidad de bytes reservados.
int getAmountOfBytesAllocated(int amountOfPages, int internalFragmentation);



/// @brief Retorna la cantidad de paginas que se necesitan (o que fueron ya reservadas) para reservar la cantidad de bytes especificados.
/// @param bytesAllocated La cantidad de bytes.
/// @return Retorna la cantidad de paginas que se necesitan.
int getAmountOfPages(int bytesAllocated);



/// @brief Ocupa un frame que esté libre en la memoria de usuario
/// @return Retorna el indice del frame que se ocurpó
int allocNextFrameFree();






#endif