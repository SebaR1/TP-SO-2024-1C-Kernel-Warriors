#include "clientIO.h"

void sendInterfaceToKernel()
{   
    t_package* package = createPackage(IO_SEND_INTERFACE);

    addToPackage(package, interfaceData.name, sizeof(char) * 8);
    addToPackage(package, &interfaceData.type, sizeof(interfaceType));

    sendPackage(package, socketKernel);

    destroyPackage(package);
}

void sendIOGenSleepResultsToKernel()
{
    t_package* package = createPackage(IO_OK);

    sendPackage(package, socketKernel);
    
    destroyPackage(package);
}

void sendIOStdinReadResultsToKernel()
{
    t_package* package = createPackage(IO_OK);

    addToPackage(package, resultsForStdin.resultsFromRead, string_length(resultsForStdin.resultsFromRead));

    sendPackage(package, socketKernel);
    
    destroyPackage(package);
}

void sendIOStdoutWriteResultsToKernel()
{
    t_package* package = createPackage(IO_OK);

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
