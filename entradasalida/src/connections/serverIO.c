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
                sendResultsFromIOGenSleepToKernel(socketClient);
                break;

            case KERNEL_SEND_OPERATION_TO_STDIN_INTERFACE:
                sendResultsFromIOStdinReadToKernel(socketClient);
                break;

            case KERNEL_SEND_OPERATION_TO_STDOUT_INTERFACE:
                sendResultsFromIOStdoutWriteToKernel(socketClient);
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
    sem_post(&semaphoreForModule);
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

            case MEMORY_OK:
                sem_post(&semaphoreForStdin);
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
    sem_post(&semaphoreForModule);
}

void sendResultsFromIOGenSleepToKernel()
{   
    t_list *listPackage = getPackage(socketKernel);

    interfaceData.currentOperation.operation = IO_GEN_SLEEP;
    interfaceData.currentOperation.pid = *((uint32_t*)list_get(listPackage, 0));
    t_paramsForGenericInterface *params = (t_paramsForGenericInterface*)interfaceData.currentOperation.params;
    params->workUnits = *((uint32_t*)list_get(listPackage, 1));

    executeIOGenSleepAndSendResults();

    list_destroy_and_destroy_elements(listPackage, free);
}

void sendResultsFromIOStdinReadToKernel()
{
    t_list *listPackage = getPackage(socketKernel);

    interfaceData.currentOperation.operation = IO_STDIN_READ;
    interfaceData.currentOperation.pid = *((uint32_t*)list_get(listPackage, 0));
    t_paramsForStdinInterface *params = (t_paramsForStdinInterface*)interfaceData.currentOperation.params;
    params->registerDirection = *((uint32_t*)list_get(listPackage, 1));
    params->registerSize = *((uint32_t*)list_get(listPackage, 2));

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

    executeIOStdoutWriteAndSendResults();

    list_destroy_and_destroy_elements(listPackage, free);
}

void receiveDataFromMemory()
{
    t_list *listPackage = getPackage(socketKernel);

    resultsForStdout.resultsForWrite = (char*)list_get(listPackage, 0);

    sem_post(&semaphoreForStdout);

    list_destroy(listPackage);
}