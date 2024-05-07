#include "serverKernel.h"
#include "utils/server/utils.h"
#include "logger.h"
#include "config.h"
#include <pthread.h>
#include <semaphore.h>

int numberOfIOClients = 0;

sem_t semaphoreForIO;

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
        pthread_create(&threadIO, NULL, (void*)serverKernelForIO, socketClientIO);
        pthread_detach(threadIO);

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

void serverKernelForIO(int *socketClient)
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

void operationPackageFromIO(t_list *package)
{
    list_iterate(package, (void*)listIterator);
}




void finishAllServersSignal()
{
    _finishAllServersSignal = true;
}