#ifndef SERVER_H_
#define SERVER_H_

#include <stdlib.h>
#include <stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>
#include<netdb.h>


typedef enum
{
	ERROR=-1,
	MESSAGE,
	PACKAGE
} operationCode;


void* getBuffer(int* size, int socketClient);

int initServer(t_log* logger, char* port);
int waitClient(t_log* logger, int socketServer);
t_list* getPackage(int socketClient);
void getMessage(t_log* logger, int socketClient);
int getOperation(int socketClient);





#endif