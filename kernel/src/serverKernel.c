#include "serverKernel.h"
#include "utils/server/utils.h"
#include "logger.h"
#include "config.h"



t_list * list;

void iterator(char* value) 
{
	log_info(getKernelLogger(),"%s", value);
}

void serverKernelForIO()
{
    // Inicio el servidor
    int socketServerKernelForIO = initServer(getKernelLogger(), getKernelConfig()->PUERTO_ESCUCHA);

    int socketClientIO = waitClient(getKernelLogger(), socketServerKernelForIO);
    if (socketClientIO == -1) {
        log_error(getKernelLogger(), "Error al esperar el cliente IO.\n");
        exit(1);
    }   else {
        log_info(getKernelLogger(), "Se conecto el cliente IO");
    }

    while(1) {
        operationCode codOP = getOperation(socketClientIO);

        switch(codOP)
        {
            case MESSAGE:
                printf("caso 0");
                getMessage(getKernelLogger(), socketClientIO);
                break;
            case PACKAGE:
                list= getPackage(socketClientIO);
                log_info(getKernelLogger(), "Me llegaron los siguientes valores:\n");
                list_iterate(list, (void*) iterator);
                break;
            case ERROR:
                log_error(getKernelLogger(), "el cliente se desconectó. Termina server\n");
                exit(EXIT_FAILURE);
                break;
            default:
                log_error(getKernelLogger(), "operación no reconocida.\n");
                break;
                exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
}