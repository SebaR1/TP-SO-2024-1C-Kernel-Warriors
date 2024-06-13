#include "stdoutInterface.h"

extern t_interfaceData interfaceData;
extern t_resultsForStdin resultsForStdout;
extern int socketKernel;
extern int socketMemory;

void executeIOStdoutWriteAndSendResults()
{
    executeIOStdoutWrite();

    sendIOStdoutWriteResultsToKernel();

    free(resultsForStdout.resultsFromRead);
    resultsForStdout.resultsFromRead = NULL;

    interfaceData.currentOperation.operation = IO_NULL;
    free(interfaceData.currentOperation.params);
    interfaceData.currentOperation.pid = -1;
}

void executeIOStdoutWrite()
{
    sendIOReadRequestToMemory();
    //sem wait

    printf("%s", resultsForStdout.resultsFromRead);
}