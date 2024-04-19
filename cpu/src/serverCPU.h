#ifndef CPU_SERVER_H_
#define CPU_SERVER_H_

#include <semaphore.h>
#include <stdbool.h>
#include <commons/collections/list.h>

#define MAX_KERNEL_CLIENTS_FOR_DISPATCH 1
#define MAX_KERNEL_CLIENTS_FOR_INTERRUPT 1


extern sem_t* semaphore;

extern bool _finishAllServersSignal;

void finishAllServersSignal();


// Hilo que funciona como servidor para recibir paquetes de los clientes y realizar determinadas operaciones
void serverCPUDispatchForKernel(int *socketClient);


// Hilo que funciona como servidor para recibir paquetes de los clientes y realizar determinadas operaciones
void serverCPUInterruptForKernel(int *socketClient);


// Espera en un loop a los clientes y les crea su hilo para recibir paquetes correspondiente (si es que no se llego al maximo de clientes)
void receiveClientIteration(int socketServer);


// Funcion que se ejecuta cuando un cliente solicita realizar esta operacion.
void operationPackageFromKernel(t_list* package);








#endif