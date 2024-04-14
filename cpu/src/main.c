#include "cpu.h"

t_list* list;

int main() 
{

    // Iniciar el servidor
    int socketServer = initServer(port);
    if (socketServer == -1) {
        //log_error(logger,"Error: no se pudo iniciar el servidor.\n" );
        return 1;
    }

    // Esperar la conexión de un cliente
    int socketClient = waitClient(socketServer);
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
            getMessage(socketClient);
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
}

/*void iterator(char* value) 
{
	log_info(logger,"%s", value);
}
*/
        