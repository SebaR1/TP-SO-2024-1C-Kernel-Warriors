#include "cpu.h"
#include "logger.h"


int main() 
{
    // Inicio el logger general del modulo. Siempre deberia ser la primera sentencia a ejecutar del main.
    initLogger("CPU.log", "CPU", true, LOG_LEVEL_INFO);

    // Obtengo la configuracion general.
    initCPUConfig("CPU.config");


    /*
    t_list* list;

    // Iniciar el servidor
    int socketServer = initServer(cpuLogger, port);
    if (socketServer == -1) {
        //log_error(logger,"Error: no se pudo iniciar el servidor.\n" );
        return 1;
    }

    // Esperar la conexión de un cliente
    int socketClient = waitClient(cpuLogger, socketServer);
    if (socketClient == -1) {
        //log_error(logger, "Error al esperar cliente.\n");
        return 1;
    }

    // Recibir opCode y evaluar si la ejecución fue exitosa
    operationCode opCode = getOperation (socketClient);
    switch(opCode)
    {
        case MESSAGE:
            printf("caso 0");
            getMessage(cpuLogger, socketClient);
            break;
        case PACKAGE:
            list= getPackage(socketClient);
            //log_info(logger, "Me llegaron los siguientes valores:\n");
			//list_iterate(list, (void*) iterator);
            break;
        case ERROR:
            printf("holaa");
            //log_info(logger, "el cliente se desconectó.\n");
            break;
        default:
            //log_error(logger, "operación no reconocida.\n");
            break;
    }
    */


    // Liberando todos los recursos
    freeCPUConfig();
    destroyLogger();
    
    return 0;
}

/*void iterator(char* value) 
{
	log_info(logger,"%s", value);
}
*/
        