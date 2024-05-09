#include "serverCPU.h"
#include "utils/server/utils.h"
#include "logger.h"
#include "config.h"

int numberOfKernelClientsForDispatch = 0;
int numberOfKernelClientsForInterrupt = 0;

sem_t semaphoreForKernelDispatch;
sem_t semaphoreForKernelInterrupt;

bool _finishAllServersSignal = false;

// Funcion auxiliar que se ejecuta en cada iteracion de una lista para loggear su valor. Usada para el primer checkpoint.
void listIterator(char *element)
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
    case KERNEL_MODULE_TO_CPU_DISPATCH:

        if (numberOfKernelClientsForDispatch >= MAX_KERNEL_CLIENTS_FOR_DISPATCH)
        {
            log_info(getLogger(), "Un cliente Kernel se intento conectar en un servidor para el Dispatch. Fue rechazado debido a que se alcanzo la cantidad maxima de clientes.");
            break;
        }

        log_info(getLogger(), "Se conecto un modulo Kernel en el dispatch");
        pthread_t threadDispatch;
        int *socketClientDispatch = (int *)malloc(sizeof(int));
        *socketClientDispatch = socketClient;
        pthread_create(&threadDispatch, NULL, (void*)serverCPUDispatchForKernel, socketClientDispatch);
        pthread_detach(threadDispatch);

        sem_wait(&semaphoreForKernelDispatch);
        numberOfKernelClientsForDispatch++;
        sem_post(&semaphoreForKernelDispatch);

        break;

    case KERNEL_MODULE_TO_CPU_INTERRUPT:

        if (numberOfKernelClientsForInterrupt >= MAX_KERNEL_CLIENTS_FOR_INTERRUPT)
        {
            log_info(getLogger(), "Un cliente Kernel se intento conectar en un servidor para el Interrupt. Fue rechazado debido a que se alcanzo la cantidad maxima de clientes.");
            break;
        }

        log_info(getLogger(), "Se conecto un modulo Kernel en el Interrupt");
        pthread_t threadInterrupt;
        int *socketClientInterrupt = (int *)malloc(sizeof(int));
        *socketClientInterrupt = socketClient;
        pthread_create(&threadInterrupt, NULL, (void*)serverCPUInterruptForKernel, socketClientInterrupt);
        pthread_detach(threadInterrupt);

        sem_wait(&semaphoreForKernelInterrupt);
        numberOfKernelClientsForInterrupt++;
        sem_post(&semaphoreForKernelInterrupt);

        break;

    case DO_NOTHING:
        break;


    case ERROR:
        log_error(getLogger(), ERROR_CASE_MESSAGE);
        break;

    default:
        log_error(getLogger(), DEFAULT_CASE_MESSAGE);
        exit(EXIT_FAILURE);
    }
}

void serverCPUDispatchForKernel(int *socketClient)
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
            log_info(getLogger(), "Obteniendo paquete por parte del modulo kernel en Dispatch");
            t_list *listPackage = getPackage(*socketClient);
            log_info(getLogger(), "Paquete obtenido con exito del modulo kernel en Dispatch");
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
            exit(EXIT_FAILURE);
        }
    }

    free(socketClient);

    sem_wait(&semaphoreForKernelDispatch);
    numberOfKernelClientsForDispatch--;
    sem_post(&semaphoreForKernelDispatch);
}

void serverCPUInterruptForKernel(int *socketClient)
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
            log_info(getLogger(), "Obteniendo paquete por parte del modulo kernel en Interrput");
            t_list *listPackage = getPackage(*socketClient);
            log_info(getLogger(), "Paquete obtenido con exito del modulo kernel en Interrupt");
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

    sem_wait(&semaphoreForKernelInterrupt);
    numberOfKernelClientsForInterrupt--;
    sem_post(&semaphoreForKernelInterrupt);
}

void operationPackageFromKernel(t_list *package)
{
    list_iterate(package, (void*)listIterator);
}

void finishAllServersSignal()
{
    _finishAllServersSignal = true;
}