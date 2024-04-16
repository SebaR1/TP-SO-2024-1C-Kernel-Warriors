#include <stdlib.h>
#include <stdio.h>
#include <utils/client/utils.h>
#include "logger.h"
#include "config.h"

int main(int argc, char* argv[])
{
    // Inicio el logger general del modulo. Siempre deberia ser la primera sentencia a ejecutar del main.
    initLogger("IO.log", "entrada salida", true, LOG_LEVEL_INFO);

    // Obtengo la configuracion general.
    initIOConfig("../IO.config");


    // Liberando todos los recursos
    freeIOConfig();
    destroyLogger();

    return 0;
}
