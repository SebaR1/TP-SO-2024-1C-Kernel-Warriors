#include "serverKernel.h"
#include "utils/server/utils.h"
#include "logger.h"
#include "config.h"






void serverCPUForKernelDispatch(void* args)
{
    // Inicio el servidor
    initServer(getCPULogger(), getCPUConfig()->PUERTO_ESCUCHA_DISPATCH);
}


void serverCPUForKernelInterrupt(void* args)
{
    // Inicio el servidor
    initServer(getCPULogger(), getCPUConfig()->PUERTO_ESCUCHA_INTERRUPT);
}