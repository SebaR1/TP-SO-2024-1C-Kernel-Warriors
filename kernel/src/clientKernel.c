#include "serverKernel.h"
#include "utils/server/utils.h"
#include "logger.h"
#include "config.h"

// Inicio el logger general del modulo. Siempre deberia ser la primera sentencia a ejecutar del main.
void clientKernelForIO()
{

/*
    // Creo Logger
    initLogger("kernel.config", "entrada salida", true, LOG_LEVEL_INFO);

    // Obtengo la configuracion general.
    initIOConfig("kernel.config");

    // Creo la conexión 
    int socketClientKernel = createConection(getKernelLogger(), getKernelconfig()->IP_CPU, getIOConfig()->PUERTO_CPU_DISPATCH);

    if (socketClientKernel == -1) 
    {
        log_error(_ioLogger, "Error al crear la conexión.\n");
        exit(1);
    } else {
        log_info(_ioLogger, "Conexión IO/Kernel exitosa");
    }

    t_package *packageToTest = createPackage();
    char *message1 = "Hola1"; 
    char *message2 = "Hola2";
    char *message3 = "Hola3";

    addAPackage(packageToTest, message1, strlen(message1)+1);
    addAPackage(packageToTest, message2, strlen(message2)+1);
    addAPackage(packageToTest, message3, strlen(message3)+1);

    sendPackage(packageToTest, socketClientKernel);
}
*/