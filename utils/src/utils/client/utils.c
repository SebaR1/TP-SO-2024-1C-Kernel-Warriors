#include "utils.h"

t_log* logger;

void* serializePackage(t_package* package, int bytes)
{
	void * magic = malloc(bytes);
	int offset = 0;

	memcpy(magic + offset, &(package->opCode), sizeof(int));
	offset+= sizeof(int);
	memcpy(magic + offset, &(package->buffer->size), sizeof(int));
	offset+= sizeof(int);
	memcpy(magic + offset, package->buffer->stream, package->buffer->size);
	offset+= package->buffer->size;

	return magic;
}

int createConection(char *ip, char* port)
{
	struct addrinfo hints;
	struct addrinfo *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, port, &hints, &servinfo);

	// Ahora vamos a crear el socket.

	int socketClient = socket(servinfo->ai_family,
                         		servinfo->ai_socktype,
                         		servinfo->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo

	connect(socketClient, servinfo->ai_addr, servinfo->ai_addrlen);


	freeaddrinfo(servinfo);

	return socketClient;
}

void enviar_mensaje(char* message, int socketClient)
{
	t_package* package = malloc(sizeof(t_package));

	package->opCode = MESSAGE;
	package->buffer = malloc(sizeof(t_buffer));
	package->buffer->size = strlen(message) + 1;
	package->buffer->stream = malloc(package->buffer->size);
	memcpy(package->buffer->stream, message, package->buffer->size);

	int bytes = package->buffer->size + 2*sizeof(int);

	void* toSend = serializePackage(package, bytes);

	send(socketClient, toSend, bytes, 0);

	free(toSend);
	removePackage(package);
}


void createBuffer(t_package* package)
{
	package->buffer = malloc(sizeof(t_buffer));
	package->buffer->size = 0;
	package->buffer->stream = NULL;
}

t_package* createPackage(void)
{
	t_package* package = malloc(sizeof(t_package));
	package->opCode = PACKAGE;
	createBuffer(package);
	return package;
}

void addAPackage(t_package* package, void* value, int size)
{
	package->buffer->stream = realloc(package->buffer->stream, package->buffer->size + size + sizeof(int));

	memcpy(package->buffer->stream + package->buffer->size, &size, sizeof(int));
	memcpy(package->buffer->stream + package->buffer->size + sizeof(int), value, size);

	package->buffer->size += size + sizeof(int);
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