#ifndef MEMORY_SERVER_H_
#define MEMORY_SERVER_H_


#include <semaphore.h>
#include <stdbool.h>
#include <commons/collections/list.h>


#define MAX_KERNEL_CLIENTS 1
#define MAX_CPU_CLIENTS 1
#define MAX_IO_CLIENTS 4096


extern sem_t semaphoreForKernel;
extern sem_t semaphoreForCPU;
extern sem_t semaphoreForIO;

extern bool _finishAllServersSignal;



// Hilo que funciona como servidor para recibir paquetes de los clientes y realizar determinadas operaciones
void serverMemoryForKernel(int* socketClient);


// Hilo que funciona como servidor para recibir paquetes de los clientes y realizar determinadas operaciones
void serverMemoryForCPU(int* socketClient);


// Hilo que funciona como servidor para recibir paquetes de los clientes y realizar determinadas operaciones
void serverMemoryForIO(int* socketClient);


// Funcion que se ejecuta cuando un cliente solicita realizar esta operacion.
void operationPackageFromKernel(t_list* package);


// Funcion que se ejecuta cuando un cliente solicita realizar esta operacion.
void operationPackageFromCPU(t_list* package);


// Funcion que se ejecuta cuando un cliente solicita realizar esta operacion.
void operationPackageFromIO(t_list* package);



void finishAllServersSignal();


// Espera en un loop a los clientes y les crea su hilo para recibir paquetes correspondiente (si es que no se llego al maximo de clientes)
void receiveClientIteration(int socketServer);


#endif