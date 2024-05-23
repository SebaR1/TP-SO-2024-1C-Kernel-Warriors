#include "utilsCPU/logger.h"
#include "utilsCPU/config.h"
#include "connections/serverCPU.h"
#include "utils/server/utils.h"
#include "utils/client/utils.h"
#include "instructionCycle/essentials.h"
#include "finish.h"



int main() 
{
    // Inicio el logger general del modulo. Siempre deberia ser la primera sentencia a ejecutar del main.
    initLogger("CPU.log", "CPU", true, LOG_LEVEL_INFO);

    // Obtengo la configuracion general.
    initCPUConfig("CPU.config");

    // Reservo memoria para mi sepaforo y lo inicializo
    sem_init(&semaphoreForKernelDispatch, 0, 1);
    sem_init(&semaphoreForKernelInterrupt, 0, 1);
    sem_init(&semaphoreWaitInstruction, 0, 0);
    sem_init(&semaphoreFinishModule, 0, 0);


    // Creo y pongo a correr el/los threads de el/los servidores de este modulo
    waitClientsLoopParams paramsDispatch;
    paramsDispatch.logger = getLogger();
    paramsDispatch.portToListen = getCPUConfig()->PUERTO_ESCUCHA_DISPATCH;
    paramsDispatch.eachIterationFunc = receiveClientIterationDispatch;
    paramsDispatch.finishLoopSignal = &_finishAllServersSignal;
    pthread_t dispatchWaitClientsLoopThread;
    pthread_create(&dispatchWaitClientsLoopThread, NULL, (void*)waitClientsLoop, &paramsDispatch);
    pthread_detach(dispatchWaitClientsLoopThread);

    waitClientsLoopParams paramsInterrupt;
    paramsInterrupt.logger = getLogger();
    paramsInterrupt.portToListen = getCPUConfig()->PUERTO_ESCUCHA_INTERRUPT;
    paramsInterrupt.eachIterationFunc = receiveClientIterationInterrupt;
    paramsInterrupt.finishLoopSignal = &_finishAllServersSignal;
    pthread_t interruptWaitClientsLoopThread;
    pthread_create(&interruptWaitClientsLoopThread, NULL, (void*)waitClientsLoop, &paramsInterrupt);
    pthread_detach(interruptWaitClientsLoopThread);


    t_package* initialPackage = createPackage(CPU_MODULE);
    log_info(getLogger(), "Creando conexion con la Memoria. Se enviara un mensaje a la Memoria");
    int socketClientMemory = createConection(getLogger(), getCPUConfig()->IP_MEMORIA, getCPUConfig()->PUERTO_MEMORIA);
    sendPackage(initialPackage, socketClientMemory);
    log_info(getLogger(), "Paquete enviado con exito.");
    initServerForASocket(socketClientMemory, serverCPUForMemory);


    destroyPackage(initialPackage);


    // Espero para ver si me llegan mensajes.
    // Esta linea es unicamente para testeo para el primer checkpoint, para saber que efectivamente funcionan las conexiones. Sera eliminada luego
    //sleep(60);

    // Espero a que me manden la señal de que tengo que terminar finalizar este modulo
    sem_wait(&semaphoreFinishModule);

    // Lanzando la senial a los servidores de que no deben escuchar mas clientes ni realizar ninguna operacion
    finishAllServersSignal();

    // Cierro las conexiones del cliente
    releaseConnection(socketClientMemory);

    // Liberando todos los recursos
    freeCPUConfig();
    destroyLogger();
    sem_destroy(&semaphoreForKernelDispatch); 
    sem_destroy(&semaphoreForKernelInterrupt);
    sem_destroy(&semaphoreWaitInstruction);
    sem_destroy(&semaphoreFinishModule);

    return 0;
}