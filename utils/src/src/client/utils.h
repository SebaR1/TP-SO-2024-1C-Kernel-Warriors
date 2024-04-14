#ifndef CLIENT_H_
#define CLIENT_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>


typedef enum
{
	MESSAGE,
	PACKAGE
} operationCode;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	operationCode opCode;
	t_buffer* buffer;
} t_package;


int createConection(char* ip, char* port);
void sendMessage(char* message, int socketClient);
t_package* createPackage(void);
void createBuffer(t_package* package);
void addAPackage(t_package* package, void* value, int tamanio);
void sendPackage(t_package* package, int socketClient);
void releaseConnection(int socketClient);
void removePackage(t_package* package);

void* serializePackage(t_package* package, int bytes);

void sayHello(char* msg);

#endif 