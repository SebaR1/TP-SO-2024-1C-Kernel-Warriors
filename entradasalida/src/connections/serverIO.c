#include "serverIO.h"

bool _finishAllServersSignal = false;

void serverIOForKernel(int *socketClient)
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
            case DO_NOTHING:
                break;

            case KERNEL_SEND_OPERATION_TO_GENERIC_INTERFACE:
                sendResultsFromIOGenSleepToKernel();
                break;

            case KERNEL_SEND_OPERATION_TO_STDIN_INTERFACE:
                sendResultsFromIOStdinReadToKernel();
                break;

            case KERNEL_SEND_OPERATION_TO_STDOUT_INTERFACE:
                sendResultsFromIOStdoutWriteToKernel();
                break;

            case KERNEL_SEND_OPERATION_FOR_IO_FS_CREATE:
                sendResultsFromIOFSCreateToKernel();
                break;

            case KERNEL_SEND_OPERATION_FOR_IO_FS_DELETE:
                sendResultsFromIOFSDeleteToKernel();
                break;

            case KERNEL_SEND_OPERATION_FOR_IO_FS_TRUNCATE:
                sendResultsFromIOFSTruncateToKernel();
                break;

            case KERNEL_SEND_OPERATION_FOR_IO_FS_WRITE:
                sendResultsFromIOFSWriteToKernel();
                break;

            case KERNEL_SEND_OPERATION_FOR_IO_FS_READ:
                sendResultsFromIOFSReadToKernel();
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
    // Si termina el ciclo de escucha de servidor para el Kernel, se habilita que continúe el hilo main y termine el programa
    //sem_post(&semaphoreForModule);
}

void serverIOForMemory(int *socketClient)
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
            case DO_NOTHING:
                break;

            case MEMORY_SEND_DATA:
                receiveDataFromMemory();
                break;

            case MEMORY_WRITE_OK:
                log_info(getLogger(), "Recibida confirmacion desde la memoria.");
                // Una vez recibida la confirmación de la memoria, se habilita a que se confirme al Kernel que se completó la operación
                if(interfaceData.currentOperation.operation == IO_STDIN_READ) sem_post(&semaphoreForStdin);
                else if(interfaceData.currentOperation.operation == IO_FS_READ) sem_post(&semaphoreForIOFSRead);
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
    // Si termina el ciclo de escucha de servidor para la Memoria, se habilita que continúe el hilo main y termine el programa
    sem_post(&semaphoreForModule);
}

void sendResultsFromIOGenSleepToKernel()
{   
    t_list *listPackage = getPackage(socketKernel);

    interfaceData.currentOperation.operation = IO_GEN_SLEEP;
    interfaceData.currentOperation.pid = *((uint32_t*)list_get(listPackage, 0));
    t_paramsForGenericInterface *params = (t_paramsForGenericInterface*)interfaceData.currentOperation.params;
    params->workUnits = *((uint32_t*)list_get(listPackage, 1));

    log_info(getLogger(), "Solicitud de operacion IO_GEN_SLEEP recibida desde el Kernel.");

    executeIOGenSleepAndSendResults();

    list_destroy_and_destroy_elements(listPackage, free);
}

void sendResultsFromIOStdinReadToKernel()
{   
    t_list *listPackage = getPackage(socketKernel);

    int listGetCounter = 0;

    interfaceData.currentOperation.operation = IO_STDIN_READ;
    interfaceData.currentOperation.pid = *((uint32_t*)list_get(listPackage, listGetCounter++));
    t_paramsForStdinInterface *params = (t_paramsForStdinInterface*)interfaceData.currentOperation.params;
    params->amountOfPhysicalAddresses = *((uint32_t*)list_get(listPackage, listGetCounter++));
    params->addressesInfo = malloc(sizeof(physicalAddressInfo) * params->amountOfPhysicalAddresses);

    for (int i = 0; i < params->amountOfPhysicalAddresses; i++)
    {
        params->addressesInfo[i].physicalAddress = *((int*)list_get(listPackage, listGetCounter++));
        params->addressesInfo[i].size = *((int*)list_get(listPackage, listGetCounter++));
    }
    
    params->totalSize = *((int*)list_get(listPackage, listGetCounter++));

    resultsForStdin.resultsForMemory = malloc(sizeof(char) * params->totalSize);

    log_info(getLogger(), "Solicitud de operacion STDIN_READ recibida desde el Kernel.");

    executeIOStdinReadAndSendResults();

    list_destroy_and_destroy_elements(listPackage, free);
}

void sendResultsFromIOStdoutWriteToKernel()
{   
    t_list *listPackage = getPackage(socketKernel);

    interfaceData.currentOperation.operation = IO_STDOUT_WRITE;
    interfaceData.currentOperation.pid = *((uint32_t*)list_get(listPackage, 0));
    t_paramsForStdoutInterface *params = (t_paramsForStdoutInterface*)interfaceData.currentOperation.params;
    params->registerDirection = *((uint32_t*)list_get(listPackage, 1));
    params->registerSize = *((uint32_t*)list_get(listPackage, 2));
    resultsForStdout.resultsForWrite = malloc(sizeof(char) * params->registerSize + 1); // + 1 para imprimir la cadena con el '\0'

    log_info(getLogger(), "Solicitud de operacion STDOUT_WRITE recibida desde el Kernel.");

    executeIOStdoutWriteAndSendResults();

    list_destroy_and_destroy_elements(listPackage, free);
}

void sendResultsFromIOFSCreateToKernel()
{
    t_list *listPackage = getPackage(socketKernel);

    interfaceData.currentOperation.params = malloc(sizeof(t_paramsForIOFSCreateOrDelete));

    interfaceData.currentOperation.operation = IO_FS_CREATE;
    interfaceData.currentOperation.pid = *((uint32_t*)list_get(listPackage, 0));
    t_paramsForIOFSCreateOrDelete *params = (t_paramsForIOFSCreateOrDelete*)interfaceData.currentOperation.params;
    params->fileName = (char*)list_get(listPackage, 1);

    log_info(getLogger(), "Solicitud de operacion IO_FS_CREATE recibida desde el Kernel.");

    executeIOFSCreateAndSendResults();

    list_destroy_and_destroy_elements(listPackage, free);
}

void sendResultsFromIOFSDeleteToKernel()
{
    t_list *listPackage = getPackage(socketKernel);

    interfaceData.currentOperation.params = malloc(sizeof(t_paramsForIOFSCreateOrDelete));

    interfaceData.currentOperation.operation = IO_FS_DELETE;
    interfaceData.currentOperation.pid = *((uint32_t*)list_get(listPackage, 0));
    t_paramsForIOFSCreateOrDelete *params = (t_paramsForIOFSCreateOrDelete*)interfaceData.currentOperation.params;
    params->fileName = (char*)list_get(listPackage, 1);

    log_info(getLogger(), "Solicitud de operacion IO_FS_DELETE recibida desde el Kernel.");

    executeIOFSDeleteAndSendResults();

    list_destroy_and_destroy_elements(listPackage, free);
}

void sendResultsFromIOFSTruncateToKernel()
{
    t_list *listPackage = getPackage(socketKernel);

    interfaceData.currentOperation.params = malloc(sizeof(t_paramsForIOFSTruncate));

    interfaceData.currentOperation.operation = IO_FS_TRUNCATE;
    interfaceData.currentOperation.pid = *((uint32_t*)list_get(listPackage, 0));
    t_paramsForIOFSTruncate *params = (t_paramsForIOFSTruncate*)interfaceData.currentOperation.params;
    params->fileName = (char*)list_get(listPackage, 1);
    params->registerSize = *((uint32_t*)list_get(listPackage, 2));

    log_info(getLogger(), "Solicitud de operacion IO_FS_TRUNCATE recibida desde el Kernel.");

    executeIOFSTruncateAndSendResults();

    list_destroy_and_destroy_elements(listPackage, free);
}

void sendResultsFromIOFSWriteToKernel()
{
    t_list *listPackage = getPackage(socketKernel);

    interfaceData.currentOperation.params = malloc(sizeof(t_paramsForIOFSWriteOrRead));

    interfaceData.currentOperation.operation = IO_FS_WRITE;
    interfaceData.currentOperation.pid = *((uint32_t*)list_get(listPackage, 0));
    t_paramsForIOFSWriteOrRead *params = (t_paramsForIOFSWriteOrRead*)interfaceData.currentOperation.params;
    params->fileName = (char*)list_get(listPackage, 1);
    params->registerDirection = *((uint32_t*)list_get(listPackage, 2));
    params->registerSize = *((uint32_t*)list_get(listPackage, 3));
    params->registerFilePointer = *((uint32_t*)list_get(listPackage, 4));

    log_info(getLogger(), "Solicitud de operacion IO_FS_WRITE recibida desde el Kernel.");

    executeIOFSWriteAndSendResults();

    list_destroy_and_destroy_elements(listPackage, free);
}

void sendResultsFromIOFSReadToKernel()
{
    t_list *listPackage = getPackage(socketKernel);

    interfaceData.currentOperation.params = malloc(sizeof(t_paramsForIOFSWriteOrRead));

    interfaceData.currentOperation.operation = IO_FS_READ;
    interfaceData.currentOperation.pid = *((uint32_t*)list_get(listPackage, 0));
    t_paramsForIOFSWriteOrRead *params = (t_paramsForIOFSWriteOrRead*)interfaceData.currentOperation.params;
    params->fileName = (char*)list_get(listPackage, 1);
    params->registerDirection = *((uint32_t*)list_get(listPackage, 2));
    params->registerSize = *((uint32_t*)list_get(listPackage, 3));
    params->registerFilePointer = *((uint32_t*)list_get(listPackage, 4));

    log_info(getLogger(), "Solicitud de operacion IO_FS_READ recibida desde el Kernel.");

    executeIOFSReadAndSendResults();

    list_destroy_and_destroy_elements(listPackage, free);
}

void receiveDataFromMemory()
{   
    if (interfaceData.currentOperation.operation == IO_STDOUT_WRITE)
    {
        t_list *listPackage = getPackage(socketKernel);

        resultsForStdout.resultsForWrite = (char*)list_get(listPackage, 0);

        log_info(getLogger(), "Contenido solicitado a la memoria recibido.");

        // Una vez recibido el contenido de la memoria, se habilita a que se imprima en pantalla en otro hilo
        sem_post(&semaphoreForStdout);

        list_destroy(listPackage);
    }
    else if (interfaceData.currentOperation.operation == IO_FS_WRITE)
    {
        t_list *listPackage = getPackage(socketKernel);

        resultsForIOFSWrite.resultsForWrite = (char*)list_get(listPackage, 0);

        log_info(getLogger(), "Contenido solicitado a la memoria recibido.");

        // Una vez recibido el contenido de la memoria, se habilita a que se imprima en pantalla en otro hilo
        sem_post(&semaphoreForIOFSWrite);

        list_destroy(listPackage);
    }
}

void finishAllServersSignal()
{
    _finishAllServersSignal = true;
}