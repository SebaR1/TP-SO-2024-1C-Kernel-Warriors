#include "serverKernel.h"
#include "utils/server/utils.h"
#include "logger.h"
#include "config.h"






void serverKernelForIO(void* args)
{
    // Inicio el servidor
    initServer(getKernelLogger() getKernelConfig()->PUERTO_ESCUCHA);
}