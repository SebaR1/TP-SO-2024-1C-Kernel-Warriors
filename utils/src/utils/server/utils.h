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
#include "utils/utilsGeneral.h"


// Estructura usada para saber de que tipo es el paquete que envia un cliente.
typedef enum operationCode;


// Funcion auxiliar que recibe el buffer del socket del cliente especificado.
// No deberia ser uasda por fuera su scope de utilidad.
void* _getBuffer(int* size, int socketClient);

// Retorna el socket de servidor, o -1 si hubo algun error.
int initServer(t_log* logger, char* port);


// Espera a que un cliente se conecte con el socket del servidor especificado.
int waitClient(t_log* logger, int socketServer);


// Cuando el socket del cliente especificado envia un paquete, el servidor lo recibe.
// Retorna un lista con todos los mensajes que envio el socket del cliente
t_list* getPackage(int socketClient);


// Cuando el socket del cliente especificado envia un paquete, el servidor lo recibe.
// Esta funcion recibe el paquete e imprime por pantalla su contenido.
// Esta funcion quedo del tp0, no deberia ser usada.
void getMessage(t_log* logger, int socketClient);


// Espera a que le llegue un paquete desde el socket del cliente especificado. Retorna el codigo de operacion de ese paquete
operationCode getOperation(int socketClient);





#endif