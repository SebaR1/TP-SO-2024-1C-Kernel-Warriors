#include "utils.h"

t_log* logger;

void* serializePackage(t_package* package, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(package->opCode), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(package->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, package->buffer->stream, package->buffer->size);
	desplazamiento+= package->buffer->size;

	return magic;
}

int initServer(char* port)
{
	struct addrinfo hints,*servinfo;

	logger=log_create("log.log", "utils_server",1, LOG_LEVEL_INFO );
	if (logger == NULL) 
	{
        printf("Error: No se pudo crear el logger.\n");
        return -1;
    }

	memset(&hints, 0, sizeof(hints));

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
	if(listen(socketServer, SOMAXCONN)==-1)
	{
		log_error(logger, "Error al escuchar las conexiones entrantes");
        close(socketServer);
        freeaddrinfo(servinfo);
        return -1;
	}

	freeaddrinfo(servinfo);

	log_info(logger, "Listo para escuchar a mi cliente");

	return socketServer;
}

int waitClient(int socketServer)
{
	// Aceptamos un nuevo cliente
    int socketClient = accept(socketServer, NULL, NULL);
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

void addAPackage(t_package* package, void* value, int tamanio)
{
	package->buffer->stream = realloc(package->buffer->stream, package->buffer->size + tamanio + sizeof(int));

	memcpy(package->buffer->stream + package->buffer->size, &tamanio, sizeof(int));
	memcpy(package->buffer->stream + package->buffer->size + sizeof(int), value, tamanio);

	package->buffer->size += tamanio + sizeof(int);
}

void sendPackage(t_package* package, int socketClient)
{
	int bytes = package->buffer->size + 2*sizeof(int);
	void* toSend = serializePackage(package, bytes);

	send(socketClient, toSend, bytes, 0);

	free(toSend);
}

void removePackage(t_package* package)
{
	free(package->buffer->stream);
	free(package->buffer);
	free(package);
}

void releaseConnection(int socketClient)
{
	close(socketClient);
}


