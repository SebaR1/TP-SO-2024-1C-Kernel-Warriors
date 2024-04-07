#include "utils.h"

t_log* logger;

int initServer(char* port)
{
	struct addrinfo hints,*servinfo, *p;

	logger=log_create("log.log", "utils_server",1, LOG_LEVEL_INFO );
	if (logger == NULL) 
	{
        printf("Error: No se pudo crear el logger.\n");
        return -1;
    }

	memset(&hints, 0, sizeof(&hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, port, &hints, &servinfo);

	// creamos el socket
	int socketServer = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (socketServer == -1) 
	{
        log_error(logger, "Error al crear el socket del servidor");
        freeaddrinfo(servinfo);
        return -1;
    }

	// Asociamos el socket a un puerto
	if (bind(socketServer, servinfo->ai_addr, servinfo->ai_addrlen) == -1) 
	{
        log_error(logger, "Error al enlazar el socket del servidor");
        close(socketServer);
        freeaddrinfo(servinfo);
        return -1;
    }

	// Escuchamos las conexiones entrantes
	if(listen(socketServer, SOMAXCONN)==-1);
	{
		log_error(logger, "Error al escuchar las conexiones entrantes");
        close(socketServer);
        freeaddrinfo(servinfo);
        return -1;
	}

	freeaddrinfo(servinfo);

	log_trace(logger, "Listo para escuchar a mi cliente");

	log_destroy(logger);

	return socketServer;
}

int waitClient(int socketServer)
{
	//almacenar información sobre la dirección del cliente que se conecta al servidor
	struct sockaddr_in dir_cliente;

    int sizeDirection = sizeof(struct sockaddr_in);

	// Aceptamos un nuevo cliente
    int socketClient = accept(socketServer, (struct sockaddr *)&dir_cliente, sizeDirection);
    if (socketClient == -1) {
        log_error(logger, "Error al aceptar la conexión del cliente");
        return -1;
    }

    log_info(logger, "Se conectó un cliente!");
    return socketClient;
}

int getOperation(int socketClient)
{
	int opCode;
	if(recv(socketClient, &opCode, sizeof(int), MSG_WAITALL) > 0)
		return opCode;
	else
	{
		close(socketClient);
		return -1;
	}
}

void* getBuffer(int* size, int socketClient)
{
	void * buffer;

	recv(socketClient, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socketClient, buffer, *size, MSG_WAITALL);

	return buffer;
}

void getMessage(int socketClient)
{
	int size;
	char* buffer = getBuffer(&size, socketClient);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* getPackage(int socketClient)
{
	int totalSize;
	int offset = 0;
	void * buffer;
	t_list* values = list_create();
	int eachSize;

	buffer = getBuffer(&totalSize, socketClient);
	while(offset < totalSize)
	{
		memcpy(&eachSize, buffer + offset, sizeof(int));
		offset+=sizeof(int);
		char* value = malloc(eachSize);
		memcpy(value, buffer+offset, eachSize);
		offset+=eachSize;
		list_add(values, value);
	}
	free(buffer);
	return values;
}
