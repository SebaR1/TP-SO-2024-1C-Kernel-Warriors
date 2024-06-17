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
    // Si termina el ciclo de escucha de servidor para el Kernel, se habilita que continúe el hilo main y termine el programa
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

            case MEMORY_WRITE_OK:
                log_info(getLogger(), "Recibida confirmacion desde la memoria.");
                // Una vez recibida la confirmación de la memoria, se habilita a que se confirme al Kernel que se completó la operación
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

    interfaceData.currentOperation.operation = IO_STDIN_READ;
    interfaceData.currentOperation.pid = *((uint32_t*)list_get(listPackage, 0));
    t_paramsForStdinInterface *params = (t_paramsForStdinInterface*)interfaceData.currentOperation.params;
    params->registerDirection = *((uint32_t*)list_get(listPackage, 1));
    params->registerSize = *((uint32_t*)list_get(listPackage, 2));
    resultsForStdin.resultsForMemory = malloc(sizeof(char) * params->registerSize);

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

void receiveDataFromMemory()
{   
    t_list *listPackage = getPackage(socketKernel);

    resultsForStdout.resultsForWrite = (char*)list_get(listPackage, 0);

    log_info(getLogger(), "Contenido solicitado a la memoria recibido.");

    // Una vez recibido el contenido de la memoria, se habilita a que se imprima en pantalla en otro hilo
    sem_post(&semaphoreForStdout);

    list_destroy(listPackage);
}

void finishAllServersSignal()
{
    _finishAllServersSignal = true;
}