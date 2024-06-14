#include "clientIO.h"

extern int socketKernel;
extern int socketMemory;
extern t_interfaceData interfaceData;
extern t_resultsForStdin resultsForStdin;

void sendIOGenSleepResultsToKernel()
{
    t_package* package = createPackage(GEN_IO_SLEEP_OK);

    sendPackage(package, socketKernel);
    
    destroyPackage(package);
}

void sendIOStdinReadResultsToKernel()
{
    t_package* package = createPackage(STDIN_READ_OK);

    addToPackage(package, resultsForStdin.resultsFromRead, string_length(resultsForStdin.resultsFromRead));

    sendPackage(package, socketKernel);
    
    destroyPackage(package);
}

void sendIOStdoutWriteResultsToKernel()
{
    t_package* package = createPackage(STDOUT_WRITE_OK);
    //cambiar codigo de operacion

    sendPackage(package, socketKernel);
    
    destroyPackage(package);
}

void sendResultsFromStdinToMemory()
{
    t_package* package = createPackage(WRITE_MEMORY);

    t_paramsForStdinInterface *params = (t_paramsForStdinInterface*)interfaceData.currentOperation.params;

    addToPackage(package, resultsForStdin.resultsFromRead, string_length(resultsForStdin.resultsFromRead) + 1);
    addToPackage(package, &params->registerDirection, sizeof(uint32_t));
    addToPackage(package, &params->registerSize, sizeof(uint32_t));

    sendPackage(package, socketKernel);
    
    destroyPackage(package);
}

void sendIOReadRequestToMemory()
{
    t_package* package = createPackage(READ_MEMORY);

    t_paramsForStdoutInterface *params = (t_paramsForStdoutInterface*)interfaceData.currentOperation.params;

    addToPackage(package, &params->registerDirection, sizeof(uint32_t));
    addToPackage(package, &params->registerSize, sizeof(uint32_t));

    sendPackage(package, socketMemory);
    
    destroyPackage(package);
}
