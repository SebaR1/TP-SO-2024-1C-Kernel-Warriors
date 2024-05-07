#include <stdlib.h>
#include <stdio.h>
#include "logger.h"
#include "config.h"
#include "serverMemory.h"
#include "utils/server/utils.h"
#include <pthread.h>

int main(int argc, char *argv[])
{
    // Inicio el logger general del modulo. Siempre deberia ser la primera sentencia a ejecutar del main.
    initLogger("memoria.log", "memoria", true, LOG_LEVEL_INFO);

    // Obtengo la configuracion general.
    initMemoryConfig("memoria.config");


    // Reservo memoria para los semaforos y lo inicializo
    sem_init(&semaphoreForKernel, 0, 1);
    sem_init(&semaphoreForCPU, 0, 1);
    sem_init(&semaphoreForIO, 0, 1);


    // Creo y pongo a correr el/los threads de el/los servidores de este modulo
    waitClientsLoopParams params;
    params.logger = getLogger();
    params.portToListen = getMemoryConfig()->PUERTO_ESCUCHA;
    params.eachIterationFunc = receiveClientIteration;
    params.finishLoopSignal = &_finishAllServersSignal;
    pthread_t waitClientsLoopThread;
    pthread_create(&waitClientsLoopThread, NULL, (void*)waitClientsLoop, &params);



    // Espero para ver si me llegan mensajes.
    // Esta linea es unicamente para testeo para el primer checkpoint, para saber que efectivamente funcionan las conexiones. Sera eliminada luego
    sleep(60);


    // Lanzando la senial a los servidores de que no deben escuchar mas clientes ni realizar ninguna operacion
    finishAllServersSignal();

    // Liberando todos los recursos
    freeMemoryConfig();
    destroyLogger();

    sem_destroy(&semaphoreForKernel);
    sem_destroy(&semaphoreForCPU);
    sem_destroy(&semaphoreForIO);

    return 0;
}
