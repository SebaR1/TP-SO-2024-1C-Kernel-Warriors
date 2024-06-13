#include "stdinInterface.h"

extern t_interfaceData interfaceData;
extern t_resultsForStdin resultsForStdin;
extern int socketKernel;
extern int socketMemory;

void executeIOStdinReadAndSendResults()
{   
    executeIOStdinRead();

    sendResultsFromStdinToMemory();

    //sem wait

    sendIOStdinReadResultsToKernel();

    free(resultsForStdin.resultsFromRead);
    resultsForStdin.resultsFromRead = NULL;

    interfaceData.currentOperation.operation = IO_NULL;
    free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
    //resultsForStdin.resultsFromRead
}

void executeIOStdinRead()
{
    log_info(getLogger(), "PID: %d - Operacion: IO_STDIN_READ", (int)interfaceData.currentOperation.pid);
    //t_paramsForStdinInterface *params = (t_paramsForStdinInterface*)interfaceData.currentOperation.params;

    scanf(" %s", resultsForStdin.resultsFromRead);
}