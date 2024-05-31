#ifndef MEMORY_PAGING_MEMORY_USER_H_
#define MEMORY_PAGING_MEMORY_USER_H_


#include <stdbool.h>
#include <semaphore.h>
#include "processManagment/essentials.h"




typedef enum
{
    OUT_OF_MEMORY = -1,
    PROCESS_IS_BEIGN_DESTROYED = -2,
} allocationErrors;






// Semaforo mutex para que solo un hilo a la vez pueda modificar los el memoryUserFrames.
// A pesar de que solo 1 proceso está en ejecucion, es necesario porque puede que un proceso quiera reservar memoria desde la CPU,
// y a la vez se destruya otro proceso desde Kernel (y se tiene que liberar su memoria), provocando que se pueda ocurrir una condicion de carrera
// al modificar el memoryUserFrames entre los 2 eventos mencionados.
extern sem_t semMemoryUserFrames;



// Inicializa la memoria del usuario
void initMemoryUser();


/// @brief Retorna true si el frame especificado está ocupado
/// @param frame El frame a verificar.
bool isFrameBusy(int frame);



/// @brief Reserva la cantidad determinada de bytes en la memoria de usuario si no se produce un Out Of Memory. Si se intenta reservar 0 bytes,
/// La funcion retorna 0 y no hace nada mas.
/// @param bytes Cantidad de bytes a reservar (no se le tiene que pasar el numero total de bytes reservados por el proceso, solo la cantidad que se quiere expandir).
/// @param pages Se le debe pasar el puntero a  la tabla de paginas actual. Si aun no hay tabla de paginas, debe pasarse el puntero de paginas apuntando a NULL.
/// (NO debe pasarse NULL).
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




/// @brief Libera la cantidad determinada de bytes en la memoria de usuario.
/// No debe intentar liberarse mas bytes de los que el proceso tiene reservados.
/// @param bytes Cantidad de bytes a liberar (no se le tiene que pasar el numero total de bytes reservados por el proceso, solo la cantidad que se quiere liberar).
/// @param pages Se le debe pasar el puntero a la tabla de paginas actual (NO debe pasarse NULL)
/// Modifica la tabla de paginas, donde el indice es cada pagina,
/// y el contenido de cada indice es el numero de frame en la memoria de usuario.
/// Retorna el mismo valor que se le pasó por parametro si se intenta liberar 0 bytes.
/// @param amountOfPages Puntero a la cantidad de paginas que tiene el proceso actualmente (NO debe pasarse NULL).
/// Retorna la nueva cantidad de paginas.
/// @param internalFragmentation Puntero a una direccion que contenga la fragmentacion interna que tiene el proceso actualmente (NO debe pasarse NULL).
/// Retorna la nueva fragmentacion interna.
/// @return Retorna la cantidad de paginas liberadas (no la cantidad total de paginas reservadas por el proceso,
/// sino las que se liberaron al llamar a esta funcion).
int freeMemory(int bytes, int* pages, int* const amountOfPages, int* const internalFragmentation);



/// @brief Retorna la cantidad de bytes que representan la fragmentacion interna de la ultima pagina de la memoria de usuario.
/// @param bytesAllocated La cantidad de bytes reservados por un proceso.
/// @return Retorna la fragmentacion interna.
int getInternalFragmentation(int bytesAllocated);



/// @brief Retorna la cantidad de bytes que estan reservados en la ultima pagina.
/// @param internalFragmentation La fragmentacion interna que ocurre en la ultima pagina.
/// @return Retorna la cantidad de byte reservados en la ultima pagina.
int getAmountOfBytesInTheLastPage(int internalFragmentation);


/// @brief Retorna la cantidad de bytes que están reservados en base a la cantidad de paginas y la fragmentacion interna
/// @param amountOfPages Cantidad de paginas reservadas
/// @param internalFragmentation Cantidad de bytes que representan la fragmentacion interna que quedó (o quedaría) en la ultima pagina
/// /// (es decir, la cantidad de bytes que faltan para completar la ultima pagina).
/// @return Retorna la cantidad de bytes reservados.
int getAmountOfBytesAllocated(int amountOfPages, int internalFragmentation);



/// @brief Retorna la cantidad de paginas que se necesitan (o que fueron ya reservadas) para reservar la cantidad de bytes especificados.
/// @param bytesAllocated La cantidad de bytes.
/// @return Retorna la cantidad de paginas que se necesitan.
int getAmountOfPagesAllocated(int bytesAllocated);



/// @brief Retorna la cantidad de paginas que ya no se necesitan para liberar la cantidad de bytes especificados.
/// Se considera que los bytes reservados por la ultima pagina son 0 bytes (es decir, como minimo retornaria 1 la funcion, ya que como minimo se liberaria 1 pagina).
/// @param bytesAllocated La cantidad de bytes.
/// @return Retorna la cantidad de paginas que ya no se necesitan.
int getAmountOfPagesToFree(int bytesToFree);



/// @brief Ocupa un frame que esté libre en la memoria de usuario
/// @return Retorna el indice del frame que se ocurpó
int allocNextFrameFree();



/// @brief Libera un frame el frame especificado. Si ya está liberado, no hace nada.
/// @param frame El frame a liberar (el indice, comenzando desde el 0).
void freeFrame(int frame);



/// @brief Lee de la memoria de usuario la cantidad de bytes especificada a partir de la direccion fisica especificada.
/// @param physicalAddress La direccion fisica a partir de la cual leer
/// @param size La cantidad de bytes a leer
/// @return Retorna un puntero hacia una copia de los datos leidos.
/// @warning Quien llama a esta funcion debe liberar la memoria del puntero retornado.
void* readBytes(int physicalAddress, int size);




/// @brief Escribe en la memoria de usuario la data especificada, con su cantidad de bytes especificados, a partir de la direccion fisica especificada
/// @param data Los datos a escribir
/// @param physicalAddress La direccion fisica a partir de la cual escribir
/// @param size La cantidad de bytes a escribir
void writeBytes(void* data, int physicalAddress, int size);



/// @brief Obtiene el frame de una pagina de un proceso
/// @param PID El PID del proceso donde buscar la pagina
/// @param page La pagina a bsucar
/// @return Retorna el frame correspondiente a la pagina especificada.
int getFrame(int PID, int page);



#endif