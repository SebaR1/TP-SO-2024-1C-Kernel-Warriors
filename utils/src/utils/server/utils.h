#ifndef UTILS_HELLO_H_
#define UTILS_HELLO_H_

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
	MESSAGE,
	PACKAGE
} operationCode;


void* getBuffer(int* size, int socketClient);

int initServer(char* port);
int waitClient(int socketServer);
t_list* getPackage(int socketClient);
void getMessage(int socketClient);
int getOperation(int socketClient);





#endif