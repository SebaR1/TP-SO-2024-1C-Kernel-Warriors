#include "kernel.h"
//#include <../../utils/src/utils/client/utils.h>
#include "utils/client/utils.h"
#include "config.h"
#include "logger.h"



int main ()
{
    // Inicio el logger general del modulo. Siempre deberia ser la primera sentencia a ejecutar del main.
    initLogger("kernel.log", "kernel", true, LOG_LEVEL_INFO);

    // Obtengo la configuracion general del kernel.
    initKernelConfig("kernel.config");






    /*







    int socketClient = createConection(IP, Port);
    if (socketClient == -1) 
    {
        //log_error(logger, "Error al crear la conexión.\n");
        return 1;
    }

    // Crear un paquete con la información de conexión
    t_package *connectionPackage = createPackage();
    addAPackage(connectionPackage, IP, strlen(IP) + 1);
    addAPackage(connectionPackage, Port, strlen(Port) + 1);

    // Serializar el paquete de conexión
    int connectionPackageSize = connectionPackage->buffer->size + 2 * sizeof(int);
    void *serializedConnectionPackage = serializePackage(connectionPackage, connectionPackageSize);

    // Enviar el paquete serializado al servidor
    send(socketClient, serializedConnectionPackage, connectionPackageSize, 0);

    // Liberar recursos
    free(serializedConnectionPackage);
    removePackage(connectionPackage);

     // Cerrar la conexión
    releaseConnection(socketClient);





    */





    return 0;
}