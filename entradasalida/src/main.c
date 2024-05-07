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
    initIOConfig("IO.config");

    t_package* initialPackage = createPackage(IO_MODULE);

    t_package* testPackageToKernel = createPackage(PACKAGE_FROM_IO);
    char* msg1 = "Holaaaa, soy un mensaje de prueba desde IO.";
    addToPackage(testPackageToKernel, msg1, string_length(msg1) + 1); // (+1) para tener en cuenta el caracter nulo

    t_package* testPackageToMemory = createPackage(PACKAGE_FROM_IO);
    char* msg2 = "Holaaaa, soy un mensaje de prueba desde IO.";
    addToPackage(testPackageToMemory, msg2, string_length(msg2) + 1); // (+1) para tener en cuenta el caracter nulo

    log_info(getLogger(), "Creando conexion con el Kernel. Se enviara un mensaje al Kernel");
    int socketClientKernel = createConection(getLogger(), getIOConfig()->IP_KERNEL, getIOConfig()->PUERTO_KERNEL);
    sendPackage(initialPackage, socketClientKernel);
    sendPackage(testPackageToKernel, socketClientKernel);
    // Espero para desconectarme del cliente
    // Esta linea es unicamente para testeo para el primer checkpoint, para saber que efectivamente funcionan las conexiones. Sera eliminada luego
    sleep(15);
    releaseConnection(socketClientKernel);
    log_info(getLogger(), "Paquete enviado con exito.");


    log_info(getLogger(), "Creando conexion con la Memoria. Se enviara un mensaje a la Memoria");
    int socketClientMemory = createConection(getLogger(), getIOConfig()->IP_MEMORIA, getIOConfig()->PUERTO_MEMORIA);
    sendPackage(initialPackage, socketClientMemory);
    sendPackage(testPackageToMemory, socketClientMemory);
    // Espero para desconectarme del cliente
    // Esta linea es unicamente para testeo para el primer checkpoint, para saber que efectivamente funcionan las conexiones. Sera eliminada luego
    sleep(15);
    releaseConnection(socketClientMemory);
    log_info(getLogger(), "Paquete enviado con exito.");


    destroyPackage(initialPackage);
    destroyPackage(testPackageToKernel);
    destroyPackage(testPackageToMemory);


    // Liberando todos los recursos
    freeIOConfig();
    destroyLogger();

    return 0;
}
