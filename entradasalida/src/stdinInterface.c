#include "stdinInterface.h"

void executeIOStdinReadAndSendResults()
{   
    executeIOStdinRead();

    sendResultsFromStdinToMemory();

    sem_wait(&semaphoreForStdin);

    sendIOStdinReadResultsToKernel();

    free(resultsForStdin.resultsFromRead);
    resultsForStdin.resultsFromRead = NULL;
    free(resultsForStdin.resultsForMemory);
    resultsForStdin.resultsForMemory = NULL;

    interfaceData.currentOperation.operation = IO_NULL;
    free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
}

void executeIOStdinRead()
{   
    log_info(getLogger(), "PID: %d - Operacion: IO_STDIN_READ", (int)interfaceData.currentOperation.pid);

    scanf(" %s", resultsForStdin.resultsFromRead);

    t_paramsForStdinInterface *params = (t_paramsForStdinInterface*)interfaceData.currentOperation.params;
    memmove(resultsForStdin.resultsForMemory, resultsForStdin.resultsFromRead, params->registerSize);
}