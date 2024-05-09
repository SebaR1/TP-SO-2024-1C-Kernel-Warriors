#include "logger.h"
#include "config.h"
#include "serverCPU.h"
#include "utils/server/utils.h"
#include "utils/client/utils.h"



int main() 
{
    // Inicio el logger general del modulo. Siempre deberia ser la primera sentencia a ejecutar del main.
    initLogger("CPU.log", "CPU", true, LOG_LEVEL_INFO);

    // Obtengo la configuracion general.
    initCPUConfig("CPU.config");

    // Reservo memoria para mi sepaforo y lo inicializo
    sem_init(&semaphoreForKernelDispatch, 0, 1);
    sem_init(&semaphoreForKernelInterrupt, 0, 1);


    // Creo y pongo a correr el/los threads de el/los servidores de este modulo
    waitClientsLoopParams paramsDispatch;
    paramsDispatch.logger = getLogger();
    paramsDispatch.portToListen = getCPUConfig()->PUERTO_ESCUCHA_DISPATCH;
    paramsDispatch.eachIterationFunc = receiveClientIteration;
    paramsDispatch.finishLoopSignal = &_finishAllServersSignal;
    pthread_t dispatchWaitClientsLoopThread;
    pthread_create(&dispatchWaitClientsLoopThread, NULL, (void*)waitClientsLoop, &paramsDispatch);
    pthread_detach(dispatchWaitClientsLoopThread);

    waitClientsLoopParams paramsInterrupt;
    paramsInterrupt.logger = getLogger();
    paramsInterrupt.portToListen = getCPUConfig()->PUERTO_ESCUCHA_INTERRUPT;
    paramsInterrupt.eachIterationFunc = receiveClientIteration;
    paramsInterrupt.finishLoopSignal = &_finishAllServersSignal;
    pthread_t interruptWaitClientsLoopThread;
    pthread_create(&interruptWaitClientsLoopThread, NULL, (void*)waitClientsLoop, &paramsInterrupt);
    pthread_join(interruptWaitClientsLoopThread, NULL);

/*
    t_package* initialPackage = createPackage(CPU_MODULE);

    t_package* testPackageToMemory = createPackage(PACKAGE_FROM_CPU);
    char* msg1 = "Holaaaa, soy un mensaje de prueba desde CPU.";
    addToPackage(testPackageToMemory, msg1, string_length(msg1) + 1); // (+1) para tener en cuenta el caracter nulo

    log_info(getLogger(), "Creando conexion con la Memoria. Se enviara un mensaje a la Memoria");
    int socketClientMemory = createConection(getLogger(), getCPUConfig()->IP_MEMORIA, getCPUConfig()->PUERTO_MEMORIA);
    sendPackage(initialPackage, socketClientMemory);
    sendPackage(testPackageToMemory, socketClientMemory);
    releaseConnection(socketClientMemory);
    log_info(getLogger(), "Paquete enviado con exito.");


    destroyPackage(initialPackage);
    destroyPackage(testPackageToMemory);
*/

    // Espero para ver si me llegan mensajes.
    // Esta linea es unicamente para testeo para el primer checkpoint, para saber que efectivamente funcionan las conexiones. Sera eliminada luego
    //sleep(60);


    // Lanzando la senial a los servidores de que no deben escuchar mas clientes ni realizar ninguna operacion
    finishAllServersSignal();


    // Liberando todos los recursos
    freeCPUConfig();
    destroyLogger();
    sem_destroy(&semaphoreForKernelDispatch); 
    sem_destroy(&semaphoreForKernelInterrupt); 

    return 0;
}