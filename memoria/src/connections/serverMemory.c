#include "serverMemory.h"
#include "utils/server/utils.h"
#include "utilsMemory/logger.h"
#include "utilsMemory/config.h"
#include "utils/utilsGeneral.h"
#include "clientMemory.h"
#include "processManagment/processLoader.h"
#include <pthread.h>
#include "paging/memoryUser.h"


int numberOfKernelClients = 0;
int numberOfCPUClients = 0;
int numberOfIOClients = 0;

sem_t semaphoreForKernel;
sem_t semaphoreForCPU;
sem_t semaphoreForIO;

int socketKernel;
int socketCPU;

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

        initServerForASocket(socketClient, serverMemoryForKernel);

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

        sendTamPagina(&socketClient, getMemoryConfig()->TAM_PAGINA);

        initServerForASocket(socketClient, serverMemoryForCPU);

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

        initServerForASocket(socketClient, serverMemoryForIO);

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
    socketKernel = *socketClient;

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


        case KERNEL_SEND_PROCESS_PATH:
            receiveNewProcessFromKernel(socketClient);
            break;

        case KERNEL_END_PROCESS:
            receiveEndProcessFromKernel(socketClient);
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
    socketCPU = *socketClient;

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

        case CPU_GIVE_ME_NEXT_INSTRUCTION:
            cpuWantsNextInstruction(socketClient);
            break;

        case CPU_RESIZE_MEMORY:
            requestResizeMemory(socketClient);
            break;

        case READ_MEMORY:
            requestReadMemory(socketClient);
            break;

        case WRITE_MEMORY:
            requestWriteMemory(socketClient);
            break;

        case CPU_GET_FRAME:
            requestFrame(socketClient);
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


        case READ_MEMORY:
            requestReadMemory(socketClient);
            break;

        case WRITE_MEMORY:
            requestWriteMemory(socketClient);
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
    list_iterate(package, (void*)listIterator);
}


void operationPackageFromCPU(t_list* package)
{
    list_iterate(package, (void*)listIterator);
}



void operationPackageFromIO(t_list* package)
{
    list_iterate(package, (void*)listIterator);
}


void cpuWantsNextInstruction(int* socketClient)
{
    // Recibo el mensaje por parte de la CPU, lo almaceno en el lugar correspondiente y destruyo la lista con sus elementos
    t_list *listPackage = getPackage(*socketClient);


    cpuGiveMeNextInstruction nextInstructionRequest;
    nextInstructionRequest.PID = *((int*)list_get(listPackage, 0)); // Obtengo el PID
    nextInstructionRequest.PC = *((int*)list_get(listPackage, 1)); // Obtengo el PC

    // Le envio la instruccion directamente en el mismo hilo porque solo puedo enviar las instrucciones una por una,
    // entonces no va a pasar que me pida otra instruccion el mismo socket cuando ni siquiera le mandé la que me pidió anres la CPU.
    sendInstructionToCpu(*socketClient, &nextInstructionRequest);

    list_destroy_and_destroy_elements(listPackage, free);
}

void receiveNewProcessFromKernel(int* socketClient)
{
    // Recibo el mensaje por parte del Kernel, lo almaceno en el lugar correspondiente y destruyo la lista con sus elementos.
    t_list *listPackage = getPackage(*socketClient);

    kernelPathProcess* processPath = malloc(sizeof(kernelPathProcess)); // Es responsabilidad del hilo que se creó y está ejecutando el liberar la memoria cuando ya no la usé.
    int* pidPointer = (int*)list_get(listPackage, 0);
    processPath->pid = *pidPointer;
    processPath->path = (char*)list_get(listPackage, 1);

    // Creo un hilo que se encarga de agregar el codigo del proceso a la lista con todos los codigos de los procesos.
    pthread_t loadCodeThread;
    pthread_create(&loadCodeThread, NULL, loadProcessByPathWithParams, processPath);
    pthread_detach(&loadCodeThread);

    free(pidPointer);
    list_destroy(listPackage);
}


void receiveEndProcessFromKernel(int* socketClient)
{
    // Recibo el mensaje por parte del Kernel, lo almaceno en el lugar correspondiente y destruyo la lista con sus elementos.
    t_list *listPackage = getPackage(*socketClient);

    kernelEndProcess* processEnd = malloc(sizeof(kernelEndProcess)); // Es responsabilidad del hilo que se creó y está ejecutando el liberar la memoria cuando ya no la usé.
    int* pidPointer = (int*)list_get(listPackage, 0);
    processEnd->pid = *pidPointer;

    // Creo un hilo que se encarga de eliminar el codigo del proceso de la lista con todos los codigos de los procesos.
    pthread_t destroyCodeThread;
    pthread_create(&destroyCodeThread, NULL, destroyProcessByParams, processEnd);
    pthread_detach(&destroyCodeThread);

    free(pidPointer);
    list_destroy(listPackage);
}


void requestResizeMemory(int* socketClient)
{
    t_list* listPackage = getPackage(*socketClient);

    requestResizeMemoryInfo request;


    request.pid = *((int*)list_get(listPackage, 0));
    request.bytes = *((int*)list_get(listPackage, 1));

    // Hago el resize de la memoria del pid directamente, sin crear otro hilo, ya que el mismo socket va a necesitar terminar este resize antes de poder hacer
    // otra peticion.
    resizeMemory(request.pid, request.bytes);


    list_destroy_and_destroy_elements(listPackage, free);
}


void requestReadMemory(int* socketClient)
{
    t_list* listPackage = getPackage(*socketClient);

    requestReadMemoryInfo request;

    request.pid = *((int*)list_get(listPackage, 0));
    request.physicalAddress = *((int*)list_get(listPackage, 1));
    request.size = *((int*)list_get(listPackage, 2));

    // Leo de la memoria directamente, sin crear otro hilo, ya que el mismo socket va a necesitar terminar esta lectura antes de poder hacer
    // otra peticion de lectura o escritura.
    void* data = readBytes(request.pid, request.physicalAddress, request.size);

    sendData(*socketClient, data, request.size);

    free(data);

    list_destroy_and_destroy_elements(listPackage, free);
}



void requestWriteMemory(int* socketClient)
{
    t_list* listPackage = getPackage(*socketClient);

    requestWriteMemoryInfo request;

    request.pid = *((int*)list_get(listPackage, 0));
    request.data = list_get(listPackage, 1);
    request.physicalAddress = *((int*)list_get(listPackage, 2));
    request.size = *((int*)list_get(listPackage, 3));

    // Escribo en la memoria directamente, sin crear otro hilo, ya que el mismo socket va a necesitar terminar esta escritura antes de poder hacer
    // otra peticion de lectura o escritura.
    writeBytes(request.pid, request.data, request.physicalAddress, request.size);

    sendWriteConfirmation(*socketClient);

    list_destroy_and_destroy_elements(listPackage, free);
}


void requestFrame(int* socketClient)
{
    t_list* listPackage = getPackage(*socketClient);

    getFrameInfo info;

    
    info.PID = *((int*)list_get(listPackage, 0));
    info.page = *((int*)list_get(listPackage, 1));

    // Le mando el frame directamente, sin crear otro hilo, ya que el mismo socket (CPU en este caso) va a necesitar
    // terminar esta peticion antes de poder hacer otra.
    sendFrame(*socketClient, getFrame(info.PID, info.page));

    list_destroy_and_destroy_elements(listPackage, free);
}



void finishAllServersSignal()
{
    _finishAllServersSignal = true;
}