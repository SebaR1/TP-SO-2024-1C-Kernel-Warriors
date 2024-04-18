#include "serverMemory.h"
#include "utils/server/utils.h"
#include "logger.h"
#include "config.h"






void serverMemoryForKernel(void* args)
{
    // Inicio el servidor
    initServer(getMemoryLogger(), getMemoryConfig()->PUERTO_ESCUCHA);
}

void serverMemoryForCPU(void* args)
{
    // Inicio el servidor
    initServer(getMemoryLogger(), getMemoryConfig()->PUERTO_ESCUCHA);
}

void serverMemoryForIO(void* args)
{
    // Inicio el servidor
    initServer(getMemoryLogger(), getMemoryConfig()->PUERTO_ESCUCHA);
}