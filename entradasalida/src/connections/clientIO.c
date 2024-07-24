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

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1); // +1 por el caracter nulo \0

    sendPackage(package, socketKernel);

    log_info(getLogger(), "Resultados de la operacion STDIN_READ enviados al Kernel.");
    
    destroyPackage(package);
}

void sendIOStdoutWriteResultsToKernel()
{   
    log_info(getLogger(), "Se enviaran los resultados de la operacion STDOUT_WRITE al Kernel.");
    t_package* package = createPackage(IO_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1); // +1 por el caracter nulo \0

    sendPackage(package, socketKernel);

    log_info(getLogger(), "Resultados de la operacion STDOUT_WRITE enviados al Kernel.");
    
    destroyPackage(package);
}

void sendResultsFromStdinToMemory(void* data, uint32_t physicalDirection, uint32_t size)
{   
    log_info(getLogger(), "Se enviara la lectura de la operacion STDIN_READ a la memoria.");
    t_package* package = createPackage(WRITE_MEMORY);

    t_paramsForStdinInterface *params = (t_paramsForStdinInterface*)interfaceData.currentOperation.params;

    addToPackage(package, &interfaceData.currentOperation.pid, sizeof(int));
    addToPackage(package, data, size);
    addToPackage(package, &physicalDirection, sizeof(uint32_t));
    addToPackage(package, &size, sizeof(uint32_t));

    sendPackage(package, socketMemory);

    log_info(getLogger(), "Lectura de la operacion STDIN_READ enviada a la memoria.");
    
    destroyPackage(package);
}

void sendResultsFromIOFSReadToMemory()
{
    log_info(getLogger(), "Se enviara la lectura de la operacion IO_FS_READ a la memoria.");
    t_package* package = createPackage(WRITE_MEMORY);

    t_paramsForIOFSWriteOrRead *params = (t_paramsForIOFSWriteOrRead*)interfaceData.currentOperation.params;

    addToPackage(package, resultsForIOFSRead.resultsFromRead, params->registerSize);
    addToPackage(package, &params->registerDirection, sizeof(uint32_t));
    addToPackage(package, &params->registerSize, sizeof(uint32_t));

    sendPackage(package, socketKernel);

    log_info(getLogger(), "Lectura de la operacion IO_FS_READ enviada a la memoria.");
    
    destroyPackage(package);
}

void sendIOReadRequestToMemory(uint32_t physicalDirection, uint32_t size)
{   
    if (interfaceData.currentOperation.operation == IO_STDOUT_WRITE)
    {
        log_info(getLogger(), "Se enviara la solicitud de lectura a la memoria para la operacion STDOUT_WRITE.");
        t_package* package = createPackage(READ_MEMORY);

        t_paramsForStdoutInterface *params = (t_paramsForStdoutInterface*)interfaceData.currentOperation.params;

        addToPackage(package, &interfaceData.currentOperation.pid, sizeof(uint32_t));
        addToPackage(package, &physicalDirection, sizeof(uint32_t));
        addToPackage(package, &size, sizeof(uint32_t));

        sendPackage(package, socketMemory);

        log_info(getLogger(), "Solicitud de lectura a la memoria para la operacion STDOUT_WRITE enviada.");
        
        destroyPackage(package);
    }
    else if (interfaceData.currentOperation.operation == IO_FS_WRITE)
    {
        log_info(getLogger(), "Se enviara la solicitud de lectura a la memoria para la operacion IO_FS_WRITE.");
        t_package* package = createPackage(READ_MEMORY);

        t_paramsForIOFSWriteOrRead *params = (t_paramsForIOFSWriteOrRead*)interfaceData.currentOperation.params;

        addToPackage(package, &params->registerDirection, sizeof(uint32_t));
        addToPackage(package, &params->registerSize, sizeof(uint32_t));

        sendPackage(package, socketMemory);

        log_info(getLogger(), "Solicitud de lectura a la memoria para la operacion IO_FS_WRITE enviada.");
        
        destroyPackage(package);
    }
}

void sendIOFSCreateResultsToKernel(int success)
{
    log_info(getLogger(), "Se enviaran los resultados de la operacion IO_FS_CREATE al Kernel.");
    t_package* package;
    if (success) package = createPackage(IO_OK);
    //else package = createPackage(IO_NOT_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1);

    sendPackage(package, socketKernel);

    log_info(getLogger(), "Resultados de la operacion IO_FS_CREATE enviados al Kernel.");
    
    destroyPackage(package);
}

void sendIOFSDeleteResultsToKernel()
{
    log_info(getLogger(), "Se enviaran los resultados de la operacion IO_FS_DELETE al Kernel.");
    t_package* package = createPackage(IO_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1);

    sendPackage(package, socketKernel);

    log_info(getLogger(), "Resultados de la operacion IO_FS_DELETE enviados al Kernel.");
    
    destroyPackage(package);
}

void sendIOFSTruncateResultsToKernel()
{
    log_info(getLogger(), "Se enviaran los resultados de la operacion IO_FS_TRUNCATE al Kernel.");
    t_package* package = createPackage(IO_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1);

    sendPackage(package, socketKernel);

    log_info(getLogger(), "Resultados de la operacion IO_FS_TRUNCATE enviados al Kernel.");
    
    destroyPackage(package);
}

void sendIOFSWriteResultsToKernel()
{
    log_info(getLogger(), "Se enviaran los resultados de la operacion IO_FS_WRITE al Kernel.");
    t_package* package = createPackage(IO_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1);

    sendPackage(package, socketKernel);

    log_info(getLogger(), "Resultados de la operacion IO_FS_WRITE enviados al Kernel.");
    
    destroyPackage(package);
}

void sendIOFSReadResultsToKernel()
{
    log_info(getLogger(), "Se enviaran los resultados de la operacion IO_FS_READ al Kernel.");
    t_package* package = createPackage(IO_OK);

    addToPackage(package, interfaceData.name, string_length(interfaceData.name) + 1);

    sendPackage(package, socketKernel);

    log_info(getLogger(), "Resultados de la operacion IO_FS_READ enviados al Kernel.");
    
    destroyPackage(package);
}