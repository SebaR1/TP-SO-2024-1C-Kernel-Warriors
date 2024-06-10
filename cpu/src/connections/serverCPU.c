#include "serverCPU.h"
#include "utils/server/utils.h"
#include "utilsCPU/logger.h"
#include "utilsCPU/config.h"
#include "utils/utilsGeneral.h"
#include "instructionCycle/fetch.h"
#include "MMU/MMU.h"

int numberOfKernelClientsForDispatch = 0;
int numberOfKernelClientsForInterrupt = 0;

sem_t semaphoreForKernelDispatch;
sem_t semaphoreForKernelInterrupt;

bool _finishAllServersSignal = false;


int socketMemory;
int socketKernelDispatch;
int socketKernelInterrupt;



// Funcion auxiliar que se ejecuta en cada iteracion de una lista para loggear su valor. Usada para el primer checkpoint.
void listIterator(char *element)
{
    log_info(getLogger(), "%s", element);
}

void receiveClientIterationDispatch(int socketServer)
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

        initServerForASocket(socketClient, serverCPUDispatchForKernel);

        sem_wait(&semaphoreForKernelDispatch);
        numberOfKernelClientsForDispatch++;
        sem_post(&semaphoreForKernelDispatch);

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

void receiveClientIterationInterrupt(int socketServer)
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
    case KERNEL_MODULE_TO_CPU_INTERRUPT:

        if (numberOfKernelClientsForInterrupt >= MAX_KERNEL_CLIENTS_FOR_INTERRUPT)
        {
            log_info(getLogger(), "Un cliente Kernel se intento conectar en un servidor para el Interrupt. Fue rechazado debido a que se alcanzo la cantidad maxima de clientes.");
            break;
        }

        log_info(getLogger(), "Se conecto un modulo Kernel en el Interrupt");

        initServerForASocket(socketClient, serverCPUInterruptForKernel);

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
    socketKernelDispatch = *socketClient;

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
    socketKernelInterrupt = *socketClient;

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

void serverCPUForMemory(int *socketClient)
{
    socketMemory = *socketClient;

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
        case MEMORY_NEXT_INSTRUCTION:
            memoryNextInstruction(socketClient);
            break;

        case MEMORY_TAM_PAGINA:
            memoryTamPaginaRecive(socketClient);
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
}

void operationPackageFromKernel(t_list *package)
{
    list_iterate(package, (void*)listIterator);
}

void memoryNextInstruction(int* socketClient)
{
    // Recibo el mensaje por parte de la memoria, lo almaceno en el lugar correspondiente y destruyo la lista (aun hay que liberar la memoria del string, pero eso es responsabilidad de quien lo use)
    t_list *listPackage = getPackage(*socketClient);

    currentInstructionString = malloc(sizeof(instructionString));
    currentInstructionString->string = (char*)list_get(listPackage, 0); // Obtengo la instruccion en forma de string

    sem_post(&semaphoreWaitInstruction); // Le aviso a la fase fetch del ciclo de instruccion que ya se obtuvo la instruccion en forma de string

    list_destroy(listPackage);
}

void memoryTamPaginaRecive(int* socketClient)
{
    // Recibo el mensaje por parte de la memoria, lo almaceno en el lugar correspondiente y destruyo la lista.
    t_list *listPackage = getPackage(*socketClient);

    memoryTamPagina tamPagina;
    tamPagina.tamPagina = *((int*)list_get(listPackage, 0)); // Obtengo el tamaño de pagina

    setTamPagina(tamPagina.tamPagina);

    list_destroy_and_destroy_elements(listPackage, free);
}

void finishAllServersSignal()
{
    _finishAllServersSignal = true;
}