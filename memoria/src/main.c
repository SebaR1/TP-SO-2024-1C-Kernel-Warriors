#include <stdlib.h>
#include <stdio.h>
#include <utils/client/utils.h>
#include "logger.h"
#include "config.h"

int main(int argc, char* argv[])
{
    // Inicio el logger general del modulo. Siempre deberia ser la primera sentencia a ejecutar del main.
    initLogger("memoria.log", "memoria", true, LOG_LEVEL_INFO);

    // Obtengo la configuracion general.
    initMemoryConfig("memoria.config");


    // Liberando todos los recursos
    freeMemoryConfig();
    destroyLogger();

    return 0;
}
