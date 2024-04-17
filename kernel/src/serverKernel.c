#include "serverKernel.h"
#include "utils/server/utils.h"
#include "logger.h"
#include "config.h"






void serverIO(void* args)
{
    // Inicio el servidor
    initServer(_kernelLogger, getKernelConfig()->PUERTO_ESCUCHA);
}