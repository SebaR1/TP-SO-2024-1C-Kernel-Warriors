#include "clientIO.h"

void sendInterfaceToKernel()
{   
    t_package* package = createPackage(IO_SEND_INTERFACE);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1);
    addToPackage(package, &interfaceData.type, sizeof(interfaceType));

    sendPackage(package, socketKernel);

    destroyPackage(package);
}

void sendIOGenSleepResultsToKernel()
{   
    log_info(getLogger(), "Se enviaran los resultados de la operacion IO_GEN_SLEEP al Kernel.");
    t_package* package = createPackage(IO_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1);

    sendPackage(package, socketKernel);

    log_info(getLogger(), "Resultados de la operacion IO_GEN_SLEEP enviados al Kernel.");
    
    destroyPackage(package);
}

void sendIOStdinReadResultsToKernel()
{   
    log_info(getLogger(), "Se enviaran los resultados de la operacion STDIN_READ al Kernel.");
    t_package* package = createPackage(IO_OK);

    addToPackage(package, resultsForStdin.resultsFromRead, string_length(resultsForStdin.resultsFromRead));

    sendPackage(package, socketKernel);

    log_info(getLogger(), "Resultados de la operacion STDIN_READ enviados al Kernel.");
    
    destroyPackage(package);
}

void sendIOStdoutWriteResultsToKernel()
{   
    log_info(getLogger(), "Se enviaran los resultados de la operacion STDOUT_WRITE al Kernel.");
    t_package* package = createPackage(IO_OK);

    sendPackage(package, socketKernel);

    log_info(getLogger(), "Resultados de la operacion STDOUT_WRITE enviados al Kernel.");
    
    destroyPackage(package);
}

void sendResultsFromStdinToMemory()
{   
    log_info(getLogger(), "Se enviara la lectura de la operacion STDIN_READ a la memoria.");
    t_package* package = createPackage(WRITE_MEMORY);

    t_paramsForStdinInterface *params = (t_paramsForStdinInterface*)interfaceData.currentOperation.params;

    addToPackage(package, resultsForStdin.resultsForMemory, params->registerSize);
    addToPackage(package, &params->registerDirection, sizeof(uint32_t));
    addToPackage(package, &params->registerSize, sizeof(uint32_t));

    sendPackage(package, socketKernel);

    log_info(getLogger(), "Lectura de la operacion STDIN_READ enviada a la memoria.");
    
    destroyPackage(package);
}

void sendIOReadRequestToMemory()
{   
    log_info(getLogger(), "Se enviara la solicitud de lectura a la memoria para la operacion STDOUT_WRITE.");
    t_package* package = createPackage(READ_MEMORY);

    t_paramsForStdoutInterface *params = (t_paramsForStdoutInterface*)interfaceData.currentOperation.params;

    addToPackage(package, &params->registerDirection, sizeof(uint32_t));
    addToPackage(package, &params->registerSize, sizeof(uint32_t));

    sendPackage(package, socketMemory);

    log_info(getLogger(), "Solicitud de lectura a la memoria para la operacion STDOUT_WRITE enviada.");
    
    destroyPackage(package);
}
