#include "serverMemory.h"
#include "utils/server/utils.h"
#include "logger.h"
#include "config.h"
#include "utils/utilsGeneral.h"
#include <pthread.h>


int numberOfKernelClients = 0;
int numberOfCPUClients = 0;
int numberOfIOClients = 0;

sem_t semaphoreForKernel;
sem_t semaphoreForCPU;
sem_t semaphoreForIO;

bool _finishAllServersSignal = false;


// Funcion auxiliar que se ejecuta en cada iteracion de una lista para loggear su valor. Usada para el primer checkpoint.
void listIterator(char* element)
{
    log_info(getLogger(), "%s", element);
}


void receiveClientIteration(int socketServer)
{
    if (_finishAllServersSignal)
    {
        return;
    }

    // Esperar la conexión de un cliente
    int socketClient = waitClient(getLogger(), socketServer);
    if (socketClient == -1)
    {
        log_error(getLogger(), "Error al esperar cliente.\n");
        exit(EXIT_FAILURE);
    }

    // Recibir el codigo de operacion para saber de que tipo es el cliente que se quiere conectar
    operationCode opCode = getOperation(socketClient);
    if (_finishAllServersSignal)
    {
        return;
    }

    switch (opCode)
    {
    case KERNEL_MODULE:

        if (numberOfKernelClients >= MAX_KERNEL_CLIENTS)
        {
            log_info(getLogger(), "Un cliente Kernel se intento conectar. Fue rechazado debido a que se alcanzo la cantidad maxima de clientes.");
            break;
        }

        log_info(getLogger(), "Se conecto un modulo Kernel");
        pthread_t threadKernel;
        int *socketClientKernel = (int *)malloc(sizeof(int));
        *socketClientKernel = socketClient;
        pthread_create(&threadKernel, NULL, serverMemoryForKernel, socketClientKernel);
        pthread_detach(&threadKernel);

        sem_wait(&semaphoreForKernel);
        numberOfKernelClients++;
        sem_post(&semaphoreForKernel);

        break;

    case CPU_MODULE:

        if (numberOfCPUClients >= MAX_CPU_CLIENTS)
        {
            log_info(getLogger(), "Un cliente CPU se intento conectar. Fue rechazado debido a que se alcanzo la cantidad maxima de clientes.");
            break;
        }

        log_info(getLogger(), "Se conecto un modulo CPU");
        pthread_t threadCPU;
        int *socketClientCPU = (int *)malloc(sizeof(int));
        *socketClientCPU = socketClient;
        pthread_create(&threadCPU, NULL, serverMemoryForCPU, socketClientCPU);
        pthread_detach(&threadCPU);

        sem_wait(&semaphoreForCPU);
        numberOfCPUClients++;
        sem_post(&semaphoreForCPU);

        break;

    case IO_MODULE:

        if (numberOfIOClients >= MAX_IO_CLIENTS)
        {
            log_info(getLogger(), "Un cliente IO se intento conectar. Fue rechazado debido a que se alcanzo la cantidad maxima de clientes.");
            break;
        }

        log_info(getLogger(), "Se conecto un modulo IO");
        pthread_t threadIO;
        int *socketClientIO = (int *)malloc(sizeof(int));
        *socketClientIO = socketClient;
        pthread_create(&threadIO, NULL, serverMemoryForIO, socketClientIO);
        pthread_detach(&threadIO);

        sem_wait(&semaphoreForIO);
        numberOfIOClients++;
        sem_post(&semaphoreForIO);

        break;

    case DO_NOTHING:
        break;


    case ERROR:
        log_error(getLogger(), ERROR_CASE_MESSAGE);
        break;

    default:
        log_error(getLogger(), DEFAULT_CASE_MESSAGE);
        break;
    }
}


void serverMemoryForKernel(int* socketClient)
{
    bool exitLoop = false;
    while (!exitLoop || _finishAllServersSignal)
    {
        // Recibir el codigo de operacion y hacer la operacion recibida.
        operationCode opCode = getOperation(*socketClient);
        if (_finishAllServersSignal)
        {
            break;
        }

        switch (opCode)
        {
        case PACKAGE_FROM_KERNEL:
            log_info(getLogger(), "Obteniendo paquete por parte del modulo Kernel");
            t_list *listPackage = getPackage(*socketClient);
            log_info(getLogger(), "Paquete obtenido con exito del modulo Kernel");
            operationPackageFromKernel(listPackage);
            break;

        case DO_NOTHING:
            break;


        case ERROR:
            log_error(getLogger(), ERROR_CASE_MESSAGE);
            exitLoop = true;
            break;

        default:
            log_error(getLogger(), DEFAULT_CASE_MESSAGE);
            break;
        }
    }

    free(socketClient);

    sem_wait(&semaphoreForKernel);
    numberOfKernelClients--;
    sem_post(&semaphoreForKernel);
}


void serverMemoryForCPU(int* socketClient)
{
    bool exitLoop = false;
    while (!exitLoop || _finishAllServersSignal)
    {
        // Recibir el codigo de operacion y hacer la operacion recibida.
        operationCode opCode = getOperation(*socketClient);
        if (_finishAllServersSignal)
        {
            break;
        }

        switch (opCode)
        {
        case PACKAGE_FROM_CPU:
            log_info(getLogger(), "Obteniendo paquete por parte del modulo CPU");
            t_list *listPackage = getPackage(*socketClient);
            log_info(getLogger(), "Paquete obtenido con exito del modulo CPU");
            operationPackageFromCPU(listPackage);
            break;

        case DO_NOTHING:
            break;

        case ERROR:
            log_error(getLogger(), ERROR_CASE_MESSAGE);
            exitLoop = true;
            break;

        default:
            log_error(getLogger(), DEFAULT_CASE_MESSAGE);
            break;
        }
    }

    free(socketClient);

    sem_wait(&semaphoreForCPU);
    numberOfCPUClients--;
    sem_post(&semaphoreForCPU);
}

void serverMemoryForIO(int* socketClient)
{
    bool exitLoop = false;
    while (!exitLoop || _finishAllServersSignal)
    {
        // Recibir el codigo de operacion y hacer la operacion recibida.
        operationCode opCode = getOperation(*socketClient);
        if (_finishAllServersSignal)
        {
            break;
        }

        switch (opCode)
        {
        case PACKAGE_FROM_IO:
            log_info(getLogger(), "Obteniendo paquete por parte del modulo IO");
            t_list *listPackage = getPackage(*socketClient);
            log_info(getLogger(), "Paquete obtenido con exito del modulo IO");
            operationPackageFromIO(listPackage);
            break;

        case DO_NOTHING:
            break;

        case ERROR:
            log_error(getLogger(), ERROR_CASE_MESSAGE);
            exitLoop = true;
            break;

        default:
            log_error(getLogger(), DEFAULT_CASE_MESSAGE);
            break;
        }
    }

    free(socketClient);

    sem_wait(&semaphoreForIO);
    numberOfIOClients--;
    sem_post(&semaphoreForIO);
}


void operationPackageFromKernel(t_list* package)
{
    list_iterate(package, listIterator);
}


void operationPackageFromCPU(t_list* package)
{
    list_iterate(package, listIterator);
}



void operationPackageFromIO(t_list* package)
{
    list_iterate(package, listIterator);
}


void finishAllServersSignal()
{
    _finishAllServersSignal = true;
}